// Copyright (c) 2009-2012 The Darkcoin developers
// Copyright (c) 2017-2018 The Swipp developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef SPORK_H
#define SPORK_H

#include "bignum.h"
#include "darksend.h"
#include "sync.h"
#include "net.h"
#include "key.h"

#include "util.h"
#include "script.h"
#include "base58.h"
#include "main.h"

#include <boost/lexical_cast.hpp>

using namespace boost;

enum class Spork : int
{
    SPORK_UNDEFINED = -1,
    SPORK_MASTERNODE_PAYMENTS_ENFORCEMENT = 10000,
    SPORK_MAX_INSTANTX_VALUE = 10002,
    SPORK_MASTERNODE_ADAPTIVE_NETWORK = 10003
};

class CSporkMessage;
class CSporkManager;

extern std::map<Spork, int> DEFAULT_ACTIVE_TIME;
extern std::map<Spork, int> DEFAULT_VALUES;

extern std::map<uint256, CSporkMessage> mapSporks;
extern std::map<Spork, CSporkMessage> mapSporksActive;
extern CSporkManager sporkManager;

void ProcessSpork(CNode* pfrom, std::string& strCommand, CDataStream& vRecv);
int GetSporkValue(Spork spork);
bool IsSporkActive(Spork spork);
void ExecuteSpork(Spork spork, int nValue);

class CSporkMessage
{
public:
    std::vector<unsigned char> vchSig;
    int nSporkID;
    int64_t nValue;
    int64_t nTimeSigned;

    uint256 GetHash()
    {
        uint256 n = Hash(BEGIN(nSporkID), END(nTimeSigned));
        return n;
    }

    ADD_SERIALIZE_METHODS;

    template <typename Stream, typename Operation>
    inline void SerializationOp(Stream& s, Operation ser_action, int nType, int nVersion)
    {
        unsigned int nSerSize = 0;
        READWRITE(nSporkID);
        READWRITE(nValue);
        READWRITE(nTimeSigned);
        READWRITE(vchSig);
    }
};

class CSporkManager
{
private:
    std::vector<unsigned char> vchSig;
    std::string strMasterPrivKey;
    std::string strTestPubKey;
    std::string strMainPubKey;

public:
    CSporkManager()
    {
        strMainPubKey = "04a6a044940a141aa186ccd316c31b000ed85545e8c19e5e28978e09fd43"
                        "d744dcd3d46fb445c29b0890dc54ca46a3a39beb8e9b4c866bba8ac557e30fbb030c56";
        strTestPubKey = "0452530188dbcb3d88eb0a59dec060ed1275d545a82dc62a7e928cec7743"
                        "1a3693cddde4f2809203175f72589d2cea769d563b8552a0a92cc1fb4c31d7b0a770c3";
    }

    Spork GetSporkByName(std::string strName);
    std::string GetNameBySpork(Spork spork);
    bool UpdateSpork(Spork spork, int64_t nValue);
    bool SetPrivKey(std::string strPrivKey);
    bool CheckSignature(CSporkMessage& spork);
    bool Sign(CSporkMessage& spork);
    void Relay(CSporkMessage& msg);
};

#endif
