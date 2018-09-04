// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2013 The Bitcoin developers
// Copyright (c) 2017-2018 The Swipp developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef BITCOIN_CORE_H
#define BITCOIN_CORE_H

#include "uint256.h"
#include "serialize.h"
#include "util.h"
#include "script.h"

#include <stdio.h>

class CScript;
class CTransaction;

// A combination of a transaction hash and an index n into its vout
class COutPoint
{
public:
    uint256 hash;
    unsigned int n;

    COutPoint();
    COutPoint(uint256 hashIn, unsigned int nIn);

    IMPLEMENT_SERIALIZE
    (
        READWRITE(FLATDATA(*this));
    )

    void SetNull();
    bool IsNull() const;
    std::string ToString() const;

    friend bool operator<(const COutPoint& a, const COutPoint& b);
    friend bool operator==(const COutPoint& a, const COutPoint& b);
    friend bool operator!=(const COutPoint& a, const COutPoint& b);
};

// A combination of a transaction and an index n into its vin
class CInPoint
{
public:
    CTransaction* ptx;
    unsigned int n;

    CInPoint();
    CInPoint(CTransaction* ptxIn, unsigned int nIn);

    void SetNull();
    bool IsNull() const;
};

// An input of a transaction. It contains the location of the previous transaction output that it claims
// and a signature that matches the output's public key
class CTxIn
{
public:
    COutPoint prevout;
    CScript scriptSig;
    CScript prevPubKey;
    unsigned int nSequence;

    CTxIn();

    explicit CTxIn(COutPoint prevoutIn, CScript scriptSigIn=CScript(),
                   unsigned int nSequenceIn=std::numeric_limits<unsigned int>::max());

    explicit CTxIn(uint256 hashPrevTx, unsigned int nOut, CScript scriptSigIn=CScript(),
                   unsigned int nSequenceIn=std::numeric_limits<unsigned int>::max());

    IMPLEMENT_SERIALIZE
    (
        READWRITE(prevout);
        READWRITE(scriptSig);
        READWRITE(nSequence);
    )

    bool IsFinal() const;
    std::string ToString() const;

    friend bool operator==(const CTxIn& a, const CTxIn& b);
    friend bool operator!=(const CTxIn& a, const CTxIn& b);
};

// An output of a transaction. It contains the public key that the next input must be able to sign with to claim it
class CTxOut
{
public:
    int64_t nValue;
    CScript scriptPubKey;

    CTxOut();
    CTxOut(int64_t nValueIn, CScript scriptPubKeyIn);

    IMPLEMENT_SERIALIZE
    (
        READWRITE(nValue);
        READWRITE(scriptPubKey);
    )

    void SetNull();
    bool IsNull() const;
    void SetEmpty();
    bool IsEmpty() const;
    uint256 GetHash() const;
    std::string ToString() const;

    friend bool operator==(const CTxOut& a, const CTxOut& b);
    friend bool operator!=(const CTxOut& a, const CTxOut& b);
};

#endif
