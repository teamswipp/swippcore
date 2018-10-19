// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2012 The Bitcoin developers
// Copyright (c) 2017-2018 The Swipp developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef SWIPP_TRANSACTION_H
#define SWIPP_TRANSACTION_H

#include "constraints.h"
#include "core.h"
#include "disk.h"
#include "db.h"
#include "tinyformat.h"
#include "txdb-leveldb.h"
#include "uint256.h"

#include <stdio.h>
#include <vector>

class CBlockIndex;
class CTxDB;
class CTxIndex;
class CTransaction;

typedef std::map<uint256, std::pair<CTxIndex, CTransaction> > MapPrevTx;

bool GetTransaction(const uint256 &hash, CTransaction &tx, uint256 &hashBlock);
bool IsStandardTx(const CTransaction& tx, std::string& reason);
bool IsFinalTx(const CTransaction &tx, int nBlockHeight = 0, int64_t nBlockTime = 0);

class CDiskTxPos
{
public:
    unsigned int nFile;
    unsigned int nBlockPos;
    unsigned int nTxPos;

    CDiskTxPos()
    {
        SetNull();
    }

    CDiskTxPos(unsigned int nFileIn, unsigned int nBlockPosIn, unsigned int nTxPosIn)
    {
        nFile = nFileIn;
        nBlockPos = nBlockPosIn;
        nTxPos = nTxPosIn;
    }

    IMPLEMENT_SERIALIZE(
        READWRITE(FLATDATA(*this));
    )

    void SetNull()
    {
        nFile = (unsigned int) -1;
        nBlockPos = 0;
        nTxPos = 0;
    }

    bool IsNull() const
    {
        return nFile == (unsigned int) -1;
    }

    friend bool operator==(const CDiskTxPos& a, const CDiskTxPos& b)
    {
        return (a.nFile == b.nFile && a.nBlockPos == b.nBlockPos && a.nTxPos == b.nTxPos);
    }

    friend bool operator!=(const CDiskTxPos& a, const CDiskTxPos& b)
    {
        return !(a == b);
    }

    std::string ToString() const
    {
        if (IsNull())
            return "null";
        else
            return strprintf("(nFile=%u, nBlockPos=%u, nTxPos=%u)", nFile, nBlockPos, nTxPos);
    }
};

class CTransaction
{
public:
    static const int CURRENT_VERSION = 1;
    int nVersion;
    unsigned int nTime;
    std::vector<CTxIn> vin;
    std::vector<CTxOut> vout;
    unsigned int nLockTime;

    // Denial-of-service detection
    mutable int nDoS;

    bool DoS(int nDoSIn, bool fIn) const
    {
        nDoS += nDoSIn;
        return fIn;
    }

    CTransaction()
    {
        SetNull();
    }

    CTransaction(int nVersion, unsigned int nTime, const std::vector<CTxIn>& vin, const std::vector<CTxOut>& vout, unsigned int nLockTime)
        : nVersion(nVersion), nTime(nTime), vin(vin), vout(vout), nLockTime(nLockTime), nDoS(0)
    {
    }

    IMPLEMENT_SERIALIZE
    (
        READWRITE(this->nVersion);
        nVersion = this->nVersion;
        READWRITE(nTime);
        READWRITE(vin);
        READWRITE(vout);
        READWRITE(nLockTime);
    )

    void SetNull()
    {
        nVersion = CTransaction::CURRENT_VERSION;
        nTime = GetAdjustedTime();
        vin.clear();
        vout.clear();
        nLockTime = 0;
        nDoS = 0;  // Denial-of-service prevention
    }

    bool IsNull() const
    {
        return vin.empty() && vout.empty();
    }

    uint256 GetHash() const
    {
        return SerializeHash(*this);
    }

    bool IsCoinBase() const
    {
        return vin.size() == 1 && vin[0].prevout.IsNull() && vout.size() >= 1;
    }

    bool IsCoinStake() const
    {
        // The coin stake transaction is marked with the first output empty
        return vin.size() > 0 && (!vin[0].prevout.IsNull()) && vout.size() >= 2 && vout[0].IsEmpty();
    }

    // Amount spent by this transaction. Returns the sum of all outputs (note: does not include fees)
    int64_t GetValueOut() const
    {
        int64_t nValueOut = 0;

        BOOST_FOREACH(const CTxOut& txout, vout)
        {
            nValueOut += txout.nValue;

            if (!MoneyRange(txout.nValue) || !MoneyRange(nValueOut))
                throw std::runtime_error("CTransaction::GetValueOut() : value out of range");
        }

        return nValueOut;
    }

    /** Amount of bitcoins coming in to this transaction
        Note that lightweight clients may not know anything besides the hash of previous transactions,
        so may not be able to calculate this.

        @param[in] mapInputs Map of previous transactions that have outputs we're spending
        @return Sum of value of all inputs (scriptSigs)
        @see CTransaction::FetchInputs
     */
    int64_t GetValueIn(const MapPrevTx& mapInputs) const;

    bool ReadFromDisk(CDiskTxPos pos, FILE** pfileRet=NULL)
    {
        CAutoFile filein = CAutoFile(OpenBlockFile(pos.nFile, 0, pfileRet ? "rb+" : "rb"), SER_DISK, CLIENT_VERSION);

        if (!filein)
            return error("CTransaction::ReadFromDisk() : OpenBlockFile failed");

        // Read transaction
        if (fseek(filein, pos.nTxPos, SEEK_SET) != 0)
            return error("CTransaction::ReadFromDisk() : fseek failed");

        try
        {
            filein >> *this;
        }
        catch (std::exception &e)
        {
            return error("%s() : deserialize or I/O error", __PRETTY_FUNCTION__);
        }

        // Return file pointer
        if (pfileRet)
        {
            if (fseek(filein, pos.nTxPos, SEEK_SET) != 0)
                return error("CTransaction::ReadFromDisk() : second fseek failed");

            *pfileRet = filein.release();
        }

        return true;
    }

    friend bool operator==(const CTransaction& a, const CTransaction& b)
    {
        return a.nVersion  == b.nVersion && a.nTime == b.nTime &&
               a.vin == b.vin && a.vout == b.vout && a.nLockTime == b.nLockTime;
    }

    friend bool operator!=(const CTransaction& a, const CTransaction& b)
    {
        return !(a == b);
    }

    std::string ToString() const
    {
        std::string str;
        str += IsCoinBase()? "Coinbase" : (IsCoinStake()? "Coinstake" : "CTransaction");
        str += strprintf("(hash=%s, nTime=%d, ver=%d, vin.size=%u, vout.size=%u, nLockTime=%d)\n",
                         GetHash().ToString(), nTime, nVersion, vin.size(), vout.size(), nLockTime);

        for (unsigned int i = 0; i < vin.size(); i++)
            str += "    " + vin[i].ToString() + "\n";
        for (unsigned int i = 0; i < vout.size(); i++)
            str += "    " + vout[i].ToString() + "\n";

        return str;
    }

    bool ReadFromDisk(CTxDB& txdb, COutPoint prevout, CTxIndex& txindexRet);
    bool ReadFromDisk(CTxDB& txdb, COutPoint prevout);
    bool ReadFromDisk(COutPoint prevout);
    bool DisconnectInputs(CTxDB& txdb);

    /** Fetch from memory and/or disk. inputsRet keys are transaction hashes.

        @param[in] txdb Transaction database
        @param[in] mapTestPool List of pending changes to the transaction index database
        @param[in] fBlock True if being called to add a new best-block to the chain
        @param[in] fMiner True if being called by CreateNewBlock
        @param[out] inputsRet Pointers to this transaction's inputs
        @param[out] fInvalid Returns true if transaction is invalid
        @return Returns true if all inputs are in txdb or mapTestPool
     */
    bool FetchInputs(CTxDB& txdb, const std::map<uint256, CTxIndex>& mapTestPool,
                     bool fBlock, bool fMiner, MapPrevTx& inputsRet, bool& fInvalid);

    /** Sanity check previous transactions, then, if all checks succeed,
        mark them as spent by this transaction.

        @param[in] inputs Previous transactions (from FetchInputs)
        @param[out] mapTestPool Keeps track of inputs that need to be updated on disk
        @param[in] posThisTx Position of this transaction on disk
        @param[in] pindexBlock
        @param[in] fBlock true if called from ConnectBlock
        @param[in] fMiner true if called from CreateNewBlock
        @return Returns true if all checks succeed
     */
    bool ConnectInputs(CTxDB& txdb, MapPrevTx inputs,
                       std::map<uint256, CTxIndex>& mapTestPool, const CDiskTxPos& posThisTx,
                       const CBlockIndex* pindexBlock, bool fBlock, bool fMiner,
                       unsigned int flags = STANDARD_SCRIPT_VERIFY_FLAGS, bool fValidateSig = true);

    bool CheckTransaction() const;
    bool GetCoinAge(CTxDB& txdb, uint64_t& nCoinAge) const;  // ppcoin: Get transaction coin age
    const CTxOut& GetOutputFor(const CTxIn& input, const MapPrevTx& inputs) const;
};

#endif /* SWIPP_TRANSACTION_H */
