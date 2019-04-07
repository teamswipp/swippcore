// Copyright (c) 2014-2015 The Darkcoin developers
// Copyright (c) 2017-2018 The Swipp developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef DARKSEND_H
#define DARKSEND_H

#include "main.h"
#include "masternode.h"
#include "activemasternode.h"

class CTxIn;
class CDarkSendPool;
class CDarkSendSigner;
class CMasterNodeVote;
class CBitcoinAddress;
class CDarksendQueue;
class CDarksendBroadcastTx;
class CActiveMasternode;

#define POOL_MAX_TRANSACTIONS                  3 // Wait for X transactions to merge and publish
#define POOL_STATUS_UNKNOWN                    0 // Waiting for update
#define POOL_STATUS_IDLE                       1 // Waiting for update
#define POOL_STATUS_QUEUE                      2 // Waiting in a queue
#define POOL_STATUS_ACCEPTING_ENTRIES          3 // Accepting entries
#define POOL_STATUS_FINALIZE_TRANSACTION       4 // Master node will broadcast what it accepted
#define POOL_STATUS_SIGNING                    5 // Check inputs/outputs, sign final tx
#define POOL_STATUS_TRANSMISSION               6 // Transmit transaction
#define POOL_STATUS_ERROR                      7 // Error
#define POOL_STATUS_SUCCESS                    8 // Success

// Status update message constants
#define MASTERNODE_ACCEPTED                    1
#define MASTERNODE_REJECTED                    0
#define MASTERNODE_RESET                       -1

#define DARKSEND_QUEUE_TIMEOUT                 120
#define DARKSEND_SIGNING_TIMEOUT               30

extern CDarkSendPool darkSendPool;
extern CDarkSendSigner darkSendSigner;
extern std::vector<CDarksendQueue> vecDarksendQueue;
extern std::string strMasterNodePrivKey;
extern map<uint256, CDarksendBroadcastTx> mapDarksendBroadcastTxes;
extern CActiveMasternode activeMasternode;

void ProcessMessageDarksend(CNode* pfrom, std::string& strCommand, CDataStream& vRecv);
int GetInputDarksendRounds(CTxIn in, int rounds=0);

// An input in the darksend pool
class CDarkSendEntryVin
{
public:
    bool isSigSet;
    CTxIn vin;

    CDarkSendEntryVin()
    {
        isSigSet = false;
        vin = CTxIn();
    }
};

// A clients transaction in the darksend pool
class CDarkSendEntry
{
public:
    bool isSet;
    std::vector<CDarkSendEntryVin> sev;
    int64_t amount;
    CTransaction collateral;
    std::vector<CTxOut> vout;
    CTransaction txSupporting;
    int64_t addedTime;

    CDarkSendEntry()
    {
        isSet = false;
        collateral = CTransaction();
        amount = 0;
    }

    bool Add(const std::vector<CTxIn> vinIn, int64_t amountIn, const CTransaction collateralIn, const std::vector<CTxOut> voutIn)
    {
        if (isSet)
            return false;

        BOOST_FOREACH(const CTxIn v, vinIn)
        {
            CDarkSendEntryVin s = CDarkSendEntryVin();
            s.vin = v;
            sev.push_back(s);
        }

        vout = voutIn;
        amount = amountIn;
        collateral = collateralIn;
        isSet = true;
        addedTime = GetTime();
        return true;
    }

    bool AddSig(const CTxIn& vin)
    {
        BOOST_FOREACH(CDarkSendEntryVin& s, sev)
        {
            if (s.vin.prevout == vin.prevout && s.vin.nSequence == vin.nSequence)
            {
                if (s.isSigSet)
                    return false;

                s.vin.scriptSig = vin.scriptSig;
                s.vin.prevPubKey = vin.prevPubKey;
                s.isSigSet = true;
                return true;
            }
        }

        return false;
    }

    bool IsExpired()
    {
        return (GetTime() - addedTime) > DARKSEND_QUEUE_TIMEOUT;
    }
};

// Currently inprogress darksend merge and denomination information
class CDarksendQueue
{
public:
    CTxIn vin;
    int64_t time;
    int nDenom;
    bool ready; // Ready for submit
    std::vector<unsigned char> vchSig;

    CDarksendQueue()
    {
        nDenom = 0;
        vin = CTxIn();
        time = 0;
        vchSig.clear();
        ready = false;
    }

    ADD_SERIALIZE_METHODS;

    template <typename Stream, typename Operation>
    inline void SerializationOp(Stream& s, Operation ser_action, int nType, int nVersion)
    {
        unsigned int nSerSize = 0;

        READWRITE(nDenom);
        READWRITE(vin);
        READWRITE(time);
        READWRITE(ready);
        READWRITE(vchSig);
    }

    bool GetAddress(CService &addr)
    {
        BOOST_FOREACH(CMasterNode mn, vecMasternodes)
        {
            if (mn.vin == vin)
            {
                addr = mn.addr;
                return true;
            }
        }

        return false;
    }

    bool GetProtocolVersion(int &protocolVersion)
    {
        BOOST_FOREACH(CMasterNode mn, vecMasternodes)
        {
            if (mn.vin == vin)
            {
                protocolVersion = mn.protocolVersion;
                return true;
            }
        }

        return false;
    }

    bool Sign();
    bool Relay();

    bool IsExpired()
    {
        return (GetTime() - time) > DARKSEND_QUEUE_TIMEOUT;
    }

    bool CheckSignature();
};

// Store darksend tx signature information
class CDarksendBroadcastTx
{
public:
    CTransaction tx;
    CTxIn vin;
    vector<unsigned char> vchSig;
    int64_t sigTime;
};

class CDarkSendSigner
{
public:
    bool IsVinAssociatedWithPubkey(CTxIn& vin, CPubKey& pubkey);
    bool SetKey(std::string strSecret, std::string& errorMessage, CKey& key, CPubKey& pubkey);
    bool SignMessage(std::string strMessage, std::string& errorMessage, std::vector<unsigned char>& vchSig, CKey key);
    bool VerifyMessage(CPubKey pubkey, std::vector<unsigned char>& vchSig, std::string strMessage, std::string& errorMessage);
};

class CDarksendSession { };

class CDarkSendPool
{
public:
    std::vector<CDarkSendEntry> myEntries; // Clients entries
    std::vector<CDarkSendEntry> entries;   // Masternode entries
    CTransaction finalTransaction;

    int64_t lastTimeChanged;
    int64_t lastAutoDenomination;

    unsigned int state;
    unsigned int entriesCount;
    unsigned int lastEntryAccepted;
    unsigned int countEntriesAccepted;

    // Where collateral should be made out to
    CScript collateralPubKey;

    std::vector<CTxIn> lockedCoins;
    uint256 masterNodeBlockHash;
    std::string lastMessage;
    bool completedTransaction;
    bool unitTest;
    CService submittedToMasternode;

    int sessionID;
    int sessionDenom; // Users must submit an denom matching this
    int sessionUsers; // N Users have said they'll join
    bool sessionFoundMasternode; // If we've found a compatible masternode
    int64_t sessionTotalValue;   // Used for autoDenom
    std::vector<CTransaction> vecSessionCollateral;

    int cachedLastSuccess;
    int cachedNumBlocks; // Used for the overview screen
    int minBlockSpacing; // Required blocks between mixes
    CTransaction txCollateral;
    int64_t lastNewBlock;
    std::string strAutoDenomResult;
    int64_t nDsqCount;

    CDarkSendPool()
    {
        // DarkSend uses collateral addresses to trust parties entering the pool
        // to behave themselves. If they don't it takes their money
        cachedLastSuccess = 0;
        cachedNumBlocks = 0;
        unitTest = false;
        txCollateral = CTransaction();
        minBlockSpacing = 1;
        nDsqCount = 0;
        lastNewBlock = 0;

        SetNull();
    }

    void InitCollateralAddress()
    {
        std::string strAddress = "";
        strAddress = "AW6ak6DXZFfCRwYQ7H68sE1HJbruLLdUTA";
        SetCollateralAddress(strAddress);
    }

    void SetMinBlockSpacing(int minBlockSpacingIn)
    {
        minBlockSpacing = minBlockSpacingIn;
    }

    bool SetCollateralAddress(std::string strAddress);
    void Reset();
    void SetNull(bool clearEverything=false);
    void UnlockCoins();

    bool IsNull() const
    {
        return (state == POOL_STATUS_ACCEPTING_ENTRIES && entries.empty() && myEntries.empty());
    }

    int GetState() const
    {
        return state;
    }

    int GetEntriesCount() const
    {
        if (fMasterNode)
            return entries.size();
        else
            return entriesCount;
    }

    int GetLastEntryAccepted() const
    {
        return lastEntryAccepted;
    }

    int GetCountEntriesAccepted() const
    {
        return countEntriesAccepted;
    }

    int GetMyTransactionCount() const
    {
        return myEntries.size();
    }

    void UpdateState(unsigned int newState)
    {
        if (fMasterNode && (newState == POOL_STATUS_ERROR || newState == POOL_STATUS_SUCCESS))
        {
            LogPrintf("CDarkSendPool::UpdateState() - Can't set state to ERROR or SUCCESS as a masternode.\n");
            return;
        }

        LogPrintf("CDarkSendPool::UpdateState() == %d | %d\n", state, newState);

        if (state != newState)
        {
            lastTimeChanged = GetTimeMillis();

            if (fMasterNode)
                RelayDarkSendStatus(darkSendPool.sessionID, darkSendPool.GetState(), darkSendPool.GetEntriesCount(), MASTERNODE_RESET);
        }

        state = newState;
    }

    int GetMaxPoolTransactions()
    {
        return POOL_MAX_TRANSACTIONS;
    }

    bool IsSessionReady()
    {
        return sessionUsers >= GetMaxPoolTransactions();
    }

    bool IsCompatibleWithEntries(std::vector<CTxOut> vout);
    bool IsCompatibleWithSession(int64_t nAmount, CTransaction txCollateral, std::string& strReason);
    bool DoAutomaticDenominating(bool fDryRun=false, bool ready=false);
    bool PrepareDarksendDenominate();

    void Check();            // Check for process in Darksend
    void ChargeFees();       // Charge fees to bad actors
    void ChargeRandomFees(); // Rarely charge fees to pay miners
    void CheckTimeout();

    bool SignatureValid(const CScript& newSig, const CTxIn& newVin);
    bool IsCollateralValid(const CTransaction& txCollateral);

    bool AddEntry(const std::vector<CTxIn>& newInput, const int64_t& nAmount, const CTransaction& txCollateral,
                  const std::vector<CTxOut>& newOutput, std::string& error);

    bool AddScriptSig(const CTxIn newVin);
    bool SignaturesComplete();
    void SendDarksendDenominate(std::vector<CTxIn>& vin, std::vector<CTxOut>& vout, int64_t amount);

    // Get masternode updates about the progress of darksend
    bool StatusUpdate(int newState, int newEntriesCount, int newAccepted, std::string& error, int newSessionID=0);

    bool SignFinalTransaction(CTransaction& finalTransactionNew, CNode* node);
    bool GetLastValidBlockHash(uint256& hash, int mod=1, int nBlockHeight=0);
    void NewBlock();
    void CompletedTransaction(bool error, std::string lastMessageNew);
    void ClearLastMessage();    
    bool SendRandomPaymentToSelf(); // Used for liquidity providers
    bool MakeCollateralAmounts();
    bool CreateDenominated(int64_t nTotalValue);

    int GetDenominations(const std::vector<CTxOut>& vout);
    void GetDenominationsToString(int nDenom, std::string& strDenomComplete);
    int GetDenominationsByAmount(int64_t nAmount, int nDenomTarget=0);
    int GetDenominationsByAmounts(std::vector<int64_t>& vecAmount);
};

void ConnectToDarkSendMasterNodeWinner();
void ThreadCheckDarkSendPool();

#endif /* DARKSEND_H */
