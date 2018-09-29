// Copyright (c) 2009-2012 The Darkcoin developers
// Copyright (c) 2017-2018 The Swipp developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "bignum.h"
#include "sync.h"
#include "net.h"
#include "key.h"
#include "util.h"
#include "script.h"
#include "base58.h"
#include "protocol.h"
#include "spork.h"
#include "main.h"
#include <boost/lexical_cast.hpp>

using namespace std;
using namespace boost;

class CSporkMessage;
class CSporkManager;

std::map<Spork, int> DEFAULT_ACTIVE_TIME =
{
    { Spork::SPORK_MASTERNODE_PAYMENTS_ENFORCEMENT, 1541030400 }, // 2018-11-01 (00:00:00 GMT)
    { Spork::SPORK_MASTERNODE_ADAPTIVE_NETWORK,     1577836800 }  // 2020-01-01 (00:00:00 GMT)
};

std::map<Spork, int> DEFAULT_VALUES =
{
    { Spork::SPORK_MAX_INSTANTX_VALUE, 10000 } // 10 000 Swipp
};

std::map<uint256, CSporkMessage> mapSporks;
std::map<Spork, CSporkMessage> mapSporksActive;
CSporkManager sporkManager;

void ProcessSpork(CNode* pfrom, std::string& strCommand, CDataStream& vRecv)
{
    // Disable all darksend/masternode related functionality
    if (fLiteMode)
        return;

    if (strCommand == "spork")
    {
        CDataStream vMsg(vRecv);
        CSporkMessage spork;
        vRecv >> spork;

        if (pindexBest == NULL)
            return;

        uint256 hash = spork.GetHash();

        if (mapSporks.count(hash) && mapSporksActive.count(static_cast<Spork>(spork.nSporkID)))
        {
            if (mapSporksActive[static_cast<Spork>(spork.nSporkID)].nTimeSigned >= spork.nTimeSigned)
            {
                if (fDebug)
                    LogPrintf("spork - seen %s block %d \n", hash.ToString().c_str(), pindexBest->nHeight);

                return;
            }
            else if (fDebug)
                LogPrintf("spork - got updated spork %s block %d \n", hash.ToString().c_str(), pindexBest->nHeight);
        }

        LogPrintf("spork - new %s ID %d Time %d bestHeight %d\n",
                  hash.ToString().c_str(), spork.nSporkID, spork.nValue, pindexBest->nHeight);

        if (!sporkManager.CheckSignature(spork))
        {
            LogPrintf("spork - invalid signature\n");
            Misbehaving(pfrom->GetId(), 100);
            return;
        }

        mapSporks[hash] = spork;
        mapSporksActive[static_cast<Spork>(spork.nSporkID)] = spork;
        sporkManager.Relay(spork);

        // Does a task if needed
        ExecuteSpork(static_cast<Spork>(spork.nSporkID), spork.nValue);
    }
    if (strCommand == "getsporks")
    {
        std::map<Spork, CSporkMessage>::iterator it = mapSporksActive.begin();

        while (it != mapSporksActive.end())
        {
            pfrom->PushMessage("spork", it->second);
            it++;
        }
    }
}

// Grab the spork, otherwise say it's off
bool IsSporkActive(Spork spork)
{
    int64_t r = 4070908800; // 2099-1-1 by default

    if (mapSporksActive.count(spork))
        r = mapSporksActive[spork].nValue;
    else
    {
        try
        {
            r = DEFAULT_ACTIVE_TIME.at(spork);
        }
        catch (const std::out_of_range& oor)
        {
            if (fDebug)
                LogPrintf("IsSporkActive::Unknown Spork %d, %s\n", static_cast<int>(spork), oor.what());
        }
    }

    return r < GetTime();
}

// Grab the value of the spork on the network, or the default
int GetSporkValue(Spork spork)
{
    int r = 0;

    if (mapSporksActive.count(spork))
        r = mapSporksActive[spork].nValue;
    else
    {
        try
        {
            r = DEFAULT_VALUES.at(spork);
        }
        catch (const std::out_of_range& oor)
        {
            if (fDebug)
                LogPrintf("GetSporkValueq::Unknown Spork %d, %s\n", static_cast<int>(spork), oor.what());
        }
    }

    return r;
}

void ExecuteSpork(Spork spork, int nValue)
{
    // Empty at the moment. Called whenever a spork is processed (via a network spork command) -
    // so we can do global spork related tasks in here
}

bool CSporkManager::CheckSignature(CSporkMessage& spork)
{
    //NOTE: Need to investigate why this is failing
    std::string strMessage = boost::lexical_cast<std::string>(spork.nSporkID) +
                             boost::lexical_cast<std::string>(spork.nValue) +
                             boost::lexical_cast<std::string>(spork.nTimeSigned);

    std::string strPubKey = strMainPubKey;
    CPubKey pubkey(ParseHex(strPubKey));
    std::string errorMessage = "";

    if(!darkSendSigner.VerifyMessage(pubkey, spork.vchSig, strMessage, errorMessage))
        return false;

    return true;
}

bool CSporkManager::Sign(CSporkMessage& spork)
{
    std::string strMessage = boost::lexical_cast<std::string>(spork.nSporkID) +
                             boost::lexical_cast<std::string>(spork.nValue) +
                             boost::lexical_cast<std::string>(spork.nTimeSigned);

    CKey key2;
    CPubKey pubkey2;
    std::string errorMessage = "";

    if (!darkSendSigner.SetKey(strMasterPrivKey, errorMessage, key2, pubkey2))
    {
        LogPrintf("CMasternodePayments::Sign - ERROR: Invalid masternodeprivkey: '%s'\n", errorMessage.c_str());
        return false;
    }

    if (!darkSendSigner.SignMessage(strMessage, errorMessage, spork.vchSig, key2))
    {
        LogPrintf("CMasternodePayments::Sign - Sign message failed");
        return false;
    }

    if(!darkSendSigner.VerifyMessage(pubkey2, spork.vchSig, strMessage, errorMessage))
    {
        LogPrintf("CMasternodePayments::Sign - Verify message failed");
        return false;
    }

    return true;
}

bool CSporkManager::UpdateSpork(Spork spork, int64_t nValue)
{
    CSporkMessage msg;
    msg.nSporkID = static_cast<int>(spork);
    msg.nValue = nValue;
    msg.nTimeSigned = GetTime();

    if (Sign(msg))
    {
        Relay(msg);
        mapSporks[msg.GetHash()] = msg;
        mapSporksActive[spork] = msg;
        return true;
    }

    return false;
}

void CSporkManager::Relay(CSporkMessage& msg)
{
    CInv inv(MSG_SPORK, msg.GetHash());
    vector<CInv> vInv;
    vInv.push_back(inv);

    LOCK(cs_vNodes);

    BOOST_FOREACH(CNode* pnode, vNodes)
        pnode->PushMessage("inv", vInv);
}

bool CSporkManager::SetPrivKey(std::string strPrivKey)
{
    CSporkMessage msg;

    // Test signing successful, proceed
    strMasterPrivKey = strPrivKey;

    Sign(msg);

    if (CheckSignature(msg))
    {
        LogPrintf("CSporkManager::SetPrivKey - Successfully initialized as spork signer\n");
        return true;
    }
    else
        return false;
}

Spork CSporkManager::GetSporkByName(std::string strName)
{
    if (strName == "SPORK_MASTERNODE_PAYMENTS_ENFORCEMENT")
        return Spork::SPORK_MASTERNODE_PAYMENTS_ENFORCEMENT;
    else if (strName == "SPORK_MAX_INSTANTX_VALUE")
        return Spork::SPORK_MAX_INSTANTX_VALUE;
    else if (strName == "SPORK_MASTERNODE_ADAPTIVE_NETWORK")
        return Spork::SPORK_MASTERNODE_ADAPTIVE_NETWORK;

    return Spork::SPORK_UNDEFINED;
}

std::string CSporkManager::GetNameBySpork(Spork spork)
{
    if (spork == Spork::SPORK_MASTERNODE_PAYMENTS_ENFORCEMENT)
        return "SPORK_MASTERNODE_PAYMENTS_ENFORCEMENT";
    else if (spork == Spork::SPORK_MAX_INSTANTX_VALUE)
        return "SPORK_MAX_INSTANTX_VALUE";
    else if (spork == Spork::SPORK_MASTERNODE_ADAPTIVE_NETWORK)
        return "SPORK_MASTERNODE_ADAPTIVE_NETWORK";

    return "Unknown";
}
