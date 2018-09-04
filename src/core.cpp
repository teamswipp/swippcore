// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2013 The Bitcoin developers
// Copyright (c) 2017-2018 The Swipp developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "core.h"

COutPoint::COutPoint() {
    SetNull();
}

COutPoint::COutPoint(uint256 hashIn, unsigned int nIn) {
    hash = hashIn; n = nIn;
}

void COutPoint::SetNull() {
    hash = 0;
    n = (unsigned int) -1;
}

bool COutPoint::IsNull() const {
    return hash == 0 && (n == (unsigned int) -1);
}

std::string COutPoint::ToString() const
{
    return strprintf("COutPoint(%s, %u)", hash.ToString().substr(0, 10), n);
}

CInPoint::CInPoint() {
    SetNull();
}

CInPoint::CInPoint(CTransaction* ptxIn, unsigned int nIn) {
    ptx = ptxIn; n = nIn;
}

void CInPoint::SetNull() {
    ptx = NULL;
    n = (unsigned int) -1;
}

bool CInPoint::IsNull() const {
    return (ptx == NULL && n == (unsigned int) -1);
}

CTxIn::CTxIn()
{
    nSequence = std::numeric_limits<unsigned int>::max();
}

CTxIn::CTxIn(COutPoint prevoutIn, CScript scriptSigIn, unsigned int nSequenceIn)
{
    prevout = prevoutIn;
    scriptSig = scriptSigIn;
    nSequence = nSequenceIn;
}

CTxIn::CTxIn(uint256 hashPrevTx, unsigned int nOut, CScript scriptSigIn, unsigned int nSequenceIn)
{
    prevout = COutPoint(hashPrevTx, nOut);
    scriptSig = scriptSigIn;
    nSequence = nSequenceIn;
}

bool CTxIn::IsFinal() const
{
    return (nSequence == std::numeric_limits<unsigned int>::max());
}

std::string CTxIn::ToString() const
{
    std::string str;
    str += "CTxIn(";
    str += prevout.ToString();

    if (prevout.IsNull())
        str += strprintf(", coinbase %s", HexStr(scriptSig));
    else
        str += strprintf(", scriptSig=%s", scriptSig.ToString().substr(0,24));

    if (nSequence != std::numeric_limits<unsigned int>::max())
        str += strprintf(", nSequence=%u", nSequence);

    str += ")";
    return str;
}

CTxOut::CTxOut()
{
    SetNull();
}

CTxOut::CTxOut(int64_t nValueIn, CScript scriptPubKeyIn)
{
    nValue = nValueIn;
    scriptPubKey = scriptPubKeyIn;
}

void CTxOut::SetNull()
{
    nValue = -1;
    scriptPubKey.clear();
}

bool CTxOut::IsNull() const
{
    return nValue == -1;
}

void CTxOut::SetEmpty()
{
    nValue = 0;
    scriptPubKey.clear();
}

bool CTxOut::IsEmpty() const
{
    return nValue == 0 && scriptPubKey.empty();
}

uint256 CTxOut::GetHash() const
{
    return SerializeHash(*this);
}

std::string CTxOut::ToString() const
{
    if (IsEmpty())
        return "CTxOut(empty)";

    return strprintf("CTxOut(nValue=%s, scriptPubKey=%s)", FormatMoney(nValue), scriptPubKey.ToString());
}

bool operator<(const COutPoint& a, const COutPoint& b)
{
    return (a.hash < b.hash || (a.hash == b.hash && a.n < b.n));
}

bool operator==(const COutPoint& a, const COutPoint& b)
{
    return (a.hash == b.hash && a.n == b.n);
}

bool operator==(const CTxIn& a, const CTxIn& b)
{
    return a.prevout == b.prevout && a.scriptSig == b.scriptSig && a.nSequence == b.nSequence;
}

bool operator==(const CTxOut& a, const CTxOut& b)
{
    return a.nValue == b.nValue && a.scriptPubKey == b.scriptPubKey;
}

bool operator!=(const COutPoint& a, const COutPoint& b)
{
    return !(a == b);
}

bool operator!=(const CTxIn& a, const CTxIn& b)
{
    return !(a == b);
}

bool operator!=(const CTxOut& a, const CTxOut& b)
{
    return !(a == b);
}
