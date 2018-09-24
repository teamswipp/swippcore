// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2012 The Bitcoin Developers
// Copyright (c) 2017-2018 The Swipp developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

// We use base58 instead of standard base64 to avoid 0OIl characters that look the same in some fonts
// and could be used to create visually identical looking account numbers.

#ifndef BITCOIN_BASE58_H
#define BITCOIN_BASE58_H

#include <string>
#include <vector>

#include "chainparams.h"
#include "bignum.h"
#include "key.h"
#include "script.h"
#include "allocators.h"
#include "util.h"

static const char* pszBase58 = "123456789ABCDEFGHJKLMNPQRSTUVWXYZabcdefghijkmnopqrstuvwxyz";

inline std::string EncodeBase58(const unsigned char* pbegin, const unsigned char* pend)
{
    CAutoBN_CTX pctx;
    CBigNum bn58 = 58;
    CBigNum bn0 = 0;

    // Convert big endian data to little endian
    // Extra zero at the end make sure bignum will interpret as a positive number
    std::vector<unsigned char> vchTmp(pend - pbegin + 1, 0);
    reverse_copy(pbegin, pend, vchTmp.begin());

    CBigNum bn;
    bn.setvch(vchTmp);
    std::string str;

    // Expected size increase from base58 conversion is approximately 137% - use 138% to be safe
    str.reserve((pend - pbegin) * 138 / 100 + 1);

    CBigNum dv;
    CBigNum rem;

    while (bn > bn0)
    {
        if (!BN_div(&dv, &rem, &bn, &bn58, pctx))
            throw bignum_error("EncodeBase58 : BN_div failed");

        bn = dv;
        unsigned int c = rem.getulong();
        str += pszBase58[c];
    }

    // Leading zeroes encoded as base58 zeros
    for (const unsigned char* p = pbegin; p < pend && *p == 0; p++)
        str += pszBase58[0];

    // Convert little endian std::string to big endian
    reverse(str.begin(), str.end());
    return str;
}

inline std::string EncodeBase58(const std::vector<unsigned char>& vch)
{
    return EncodeBase58(&vch[0], &vch[0] + vch.size());
}

inline bool DecodeBase58(const char* psz, std::vector<unsigned char>& vchRet)
{
    CAutoBN_CTX pctx;
    vchRet.clear();

    CBigNum bn58 = 58;
    CBigNum bn = 0;
    CBigNum bnChar;

    while (isspace(*psz))
        psz++;

    // Convert big endian string to bignum
    for (const char* p = psz; *p; p++)
    {
        const char* p1 = strchr(pszBase58, *p);

        if (p1 == NULL)
        {
            while (isspace(*p))
                p++;

            if (*p != '\0')
                return false;

            break;
        }

        bnChar.setulong(p1 - pszBase58);

        if (!BN_mul(&bn, &bn, &bn58, pctx))
            throw bignum_error("DecodeBase58 : BN_mul failed");

        bn += bnChar;
    }

    // Get bignum as little endian data
    std::vector<unsigned char> vchTmp = bn.getvch();

    // Trim off sign byte if present
    if (vchTmp.size() >= 2 && vchTmp.end()[-1] == 0 && vchTmp.end()[-2] >= 0x80)
        vchTmp.erase(vchTmp.end() - 1);

    int nLeadingZeros = 0;

    // Restore leading zeros
    for (const char* p = psz; *p == pszBase58[0]; p++)
        nLeadingZeros++;

    vchRet.assign(nLeadingZeros + vchTmp.size(), 0);

    // Convert little endian data to big endian
    reverse_copy(vchTmp.begin(), vchTmp.end(), vchRet.end() - vchTmp.size());
    return true;
}

inline bool DecodeBase58(const std::string& str, std::vector<unsigned char>& vchRet)
{
    return DecodeBase58(str.c_str(), vchRet);
}

inline std::string EncodeBase58WithChecksum(const std::vector<unsigned char>& vchIn)
{
    std::vector<unsigned char> vch(vchIn);
    uint256 hash = Hash(vch.begin(), vch.end());

    vch.insert(vch.end(), (unsigned char *) &hash, (unsigned char *) &hash + 4);
    return EncodeBase58(vch);
}

inline bool DecodeBase58WithChecksum(const char* psz, std::vector<unsigned char>& vchRet)
{
    if (!DecodeBase58(psz, vchRet))
        return false;

    if (vchRet.size() < 4)
    {
        vchRet.clear();
        return false;
    }

    uint256 hash = Hash(vchRet.begin(), vchRet.end() - 4);

    if (memcmp(&hash, &vchRet.end()[-4], 4) != 0)
    {
        vchRet.clear();
        return false;
    }

    vchRet.resize(vchRet.size() - 4);
    return true;
}

inline bool DecodeBase58WithChecksum(const std::string& str, std::vector<unsigned char>& vchRet)
{
    return DecodeBase58WithChecksum(str.c_str(), vchRet);
}

class CBase58Data
{
protected:
    std::vector<unsigned char> vchVersion;
    typedef std::vector<unsigned char, zero_after_free_allocator<unsigned char> > vector_uchar;
    vector_uchar vchData;

    CBase58Data()
    {
        vchVersion.clear();
        vchData.clear();
    }

    void SetData(const std::vector<unsigned char> &vchVersionIn, const void* pdata, size_t nSize)
    {
        vchVersion = vchVersionIn;
        vchData.resize(nSize);

        if (!vchData.empty())
            memcpy(&vchData[0], pdata, nSize);
    }

    void SetData(const std::vector<unsigned char> &vchVersionIn, const unsigned char *pbegin, const unsigned char *pend)
    {
        SetData(vchVersionIn, (void*)pbegin, pend - pbegin);
    }

public:
    bool SetString(const char* psz, unsigned int nVersionBytes = 1)
    {
        std::vector<unsigned char> vchTemp;
        DecodeBase58WithChecksum(psz, vchTemp);

        if (vchTemp.size() < nVersionBytes)
        {
            vchData.clear();
            vchVersion.clear();

            return false;
        }

        vchVersion.assign(vchTemp.begin(), vchTemp.begin() + nVersionBytes);
        vchData.resize(vchTemp.size() - nVersionBytes);

        if (!vchData.empty())
            memcpy(&vchData[0], &vchTemp[nVersionBytes], vchData.size());

        OPENSSL_cleanse(&vchTemp[0], vchData.size());
        return true;
    }

    bool SetString(const std::string& str)
    {
        return SetString(str.c_str());
    }

    std::string ToString() const
    {
        std::vector<unsigned char> vch = vchVersion;
        vch.insert(vch.end(), vchData.begin(), vchData.end());

        return EncodeBase58WithChecksum(vch);
    }

    int CompareTo(const CBase58Data& b58) const
    {
        if (vchVersion < b58.vchVersion)
            return -1;
        else if (vchVersion > b58.vchVersion)
            return  1;
        else if (vchData < b58.vchData)
            return -1;
        else if (vchData > b58.vchData)
            return  1;

        return 0;
    }

    bool operator==(const CBase58Data& b58) const { return CompareTo(b58) == 0; }
    bool operator<=(const CBase58Data& b58) const { return CompareTo(b58) <= 0; }
    bool operator>=(const CBase58Data& b58) const { return CompareTo(b58) >= 0; }
    bool operator< (const CBase58Data& b58) const { return CompareTo(b58) <  0; }
    bool operator> (const CBase58Data& b58) const { return CompareTo(b58) >  0; }
};

/** base58-encoded addresses.
 * Public-key-hash-addresses have version 25 (or 111 testnet).
 * The data vector contains RIPEMD160(SHA256(pubkey)), where pubkey is the serialized public key.
 * Script-hash-addresses have version 85 (or 196 testnet).
 * The data vector contains RIPEMD160(SHA256(cscript)), where cscript is the serialized redemption script.
 */
class CBitcoinAddress;

class CBitcoinAddressVisitor : public boost::static_visitor<bool>
{
private:
    CBitcoinAddress *addr;

public:
    CBitcoinAddressVisitor(CBitcoinAddress *addrIn) : addr(addrIn) { }

    bool operator()(const CKeyID &id) const;
    bool operator()(const CScriptID &id) const;
    bool operator()(const CStealthAddress &stxAddr) const;
    bool operator()(const CNoDestination &no) const;
};

class CBitcoinAddress : public CBase58Data
{
public:
    bool Set(const CKeyID &id)
    {
        SetData(Params().Base58Prefix(CChainParams::PUBKEY_ADDRESS), &id, 20);
        return true;
    }

    bool Set(const CScriptID &id)
    {
        SetData(Params().Base58Prefix(CChainParams::SCRIPT_ADDRESS), &id, 20);
        return true;
    }

    bool Set(const CTxDestination &dest)
    {
        return boost::apply_visitor(CBitcoinAddressVisitor(this), dest);
    }

    bool IsValid() const
    {
        bool fCorrectSize = vchData.size() == 20;
        bool fKnownVersion = vchVersion == Params().Base58Prefix(CChainParams::PUBKEY_ADDRESS) ||
                             vchVersion == Params().Base58Prefix(CChainParams::SCRIPT_ADDRESS);

        return fCorrectSize && fKnownVersion;
    }

    CBitcoinAddress() { }

    CBitcoinAddress(const CTxDestination &dest)
    {
        Set(dest);
    }

    CBitcoinAddress(const std::string& strAddress)
    {
        SetString(strAddress);
    }

    CBitcoinAddress(const char* pszAddress)
    {
        SetString(pszAddress);
    }

    CTxDestination Get() const
    {
        if (!IsValid())
            return CNoDestination();

        uint160 id;
        memcpy(&id, &vchData[0], 20);

        if (vchVersion == Params().Base58Prefix(CChainParams::PUBKEY_ADDRESS))
            return CKeyID(id);
        else if (vchVersion == Params().Base58Prefix(CChainParams::SCRIPT_ADDRESS))
            return CScriptID(id);
        else
            return CNoDestination();
    }

    bool GetKeyID(CKeyID &keyID) const
    {
        if (!IsValid() || vchVersion != Params().Base58Prefix(CChainParams::PUBKEY_ADDRESS))
            return false;

        uint160 id;
        memcpy(&id, &vchData[0], 20);
        keyID = CKeyID(id);

        return true;
    }

    bool IsScript() const
    {
        return IsValid() && vchVersion == Params().Base58Prefix(CChainParams::SCRIPT_ADDRESS);
    }
};

bool inline CBitcoinAddressVisitor::operator()(const CKeyID &id) const { return addr->Set(id); }
bool inline CBitcoinAddressVisitor::operator()(const CScriptID &id) const { return addr->Set(id); }
bool inline CBitcoinAddressVisitor::operator()(const CStealthAddress &stxAddr) const { return false; }
bool inline CBitcoinAddressVisitor::operator()(const CNoDestination &id) const { return false; }

class CBitcoinSecret : public CBase58Data
{
public:
    void SetKey(const CKey& vchSecret)
    {
        assert(vchSecret.IsValid());
        SetData(Params().Base58Prefix(CChainParams::SECRET_KEY), vchSecret.begin(), vchSecret.size());

        if (vchSecret.IsCompressed())
            vchData.push_back(1);
    }

    CKey GetKey()
    {
        CKey ret;
        ret.Set(&vchData[0], &vchData[32], vchData.size() > 32 && vchData[32] == 1);

        return ret;
    }

    bool IsValid() const
    {
        bool fExpectedFormat = vchData.size() == 32 || (vchData.size() == 33 && vchData[32] == 1);
        bool fCorrectVersion = vchVersion == Params().Base58Prefix(CChainParams::SECRET_KEY);

        return fExpectedFormat && fCorrectVersion;
    }

    bool SetString(const char* pszSecret)
    {
        return CBase58Data::SetString(pszSecret) && IsValid();
    }

    bool SetString(const std::string& strSecret)
    {
        return SetString(strSecret.c_str());
    }

    CBitcoinSecret(const CKey& vchSecret)
    {
        SetKey(vchSecret);
    }

    CBitcoinSecret() { }
};


template<typename K, int Size, CChainParams::Base58Type Type> class CBitcoinExtKeyBase : public CBase58Data
{
public:
    void SetKey(const K &key)
    {
        unsigned char vch[Size];
        key.Encode(vch);
        SetData(Params().Base58Prefix(Type), vch, vch+Size);
    }

    K GetKey()
    {
        K ret;
        ret.Decode(&vchData[0], &vchData[Size]);
        return ret;
    }

    CBitcoinExtKeyBase(const K &key)
    {
        SetKey(key);
    }

    CBitcoinExtKeyBase() { }
};

typedef CBitcoinExtKeyBase<CExtKey, 74, CChainParams::EXT_SECRET_KEY> CBitcoinExtKey;
typedef CBitcoinExtKeyBase<CExtPubKey, 74, CChainParams::EXT_PUBLIC_KEY> CBitcoinExtPubKey;

#endif // BITCOIN_BASE58_H
