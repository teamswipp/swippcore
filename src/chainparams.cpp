// Copyright (c) 2010 Satoshi Nakamoto
// Copyright (c) 2009-2012 The Bitcoin developers
// Copyright (c) 2017-2018 The Swipp developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "assert.h"
#include "chainparams.h"
#include "main.h"
#include "util.h"

#include <boost/assign/list_of.hpp>

using namespace boost::assign;

struct SeedSpec6 {
    uint8_t addr[16];
    uint16_t port;
};

#include "chainparamsseeds.h"

// Convert the pnSeeds6 array into usable address objects.
static void convertSeed6(std::vector<CAddress> &vSeedsOut, const SeedSpec6 *data, unsigned int count)
{
    // Will only connect to one or two seed nodes because once it connects,
    // it will get a pile of addresses with newer timestamps.
    // Seed nodes are given a random 'last seen time' of between one and two
    // weeks ago.
    const int64_t nOneWeek = 7 * 24 * 60 * 60;

    for (unsigned int i = 0; i < count; i++)
    {
        struct in6_addr ip;
        memcpy(&ip, data[i].addr, sizeof(ip));
        CAddress addr(CService(ip, data[i].port));

        addr.nTime = GetTime() - GetRand(nOneWeek) - nOneWeek;
        vSeedsOut.push_back(addr);
    }
}

// Main network

class CMainParams : public CChainParams
{
public:
    CMainParams()
    {
        // The message start string is designed to be unlikely to occur in normal data.
        // The characters are rarely used upper ASCII, not valid as UTF-8, and produce
        // a large 4-byte int at any alignment.
        pchMessageStart[0] = 0x12;
        pchMessageStart[1] = 0xe2;
        pchMessageStart[2] = 0xc5;
        pchMessageStart[3] = 0x54;

        bnProofOfWorkLimit = CBigNum(~uint256(0) >> 20);
        vAlertPubKey = ParseHex("048b05ed906551eaeaa184d109e1509e4f4193e95e85cd8c3a21f65cc00a"
                                "2b473ce5df498164f0e8f9ba6502d302db4bdff167d146df7ad6112128e67640ce96a5");
        nDefaultPort = 24055;
        nRPCPort = 35075;

        // Build the genesis block. Note that the output of the genesis coinbase cannot
        // be spent as it did not originally exist in the database.

        const char* pszTimestamp = "Swipp - The Beginning 5/9/2017";
        std::vector<CTxIn> vin;

        vin.resize(1);
        vin[0].scriptSig = CScript() << 0 << CBigNum(42) <<
            vector<unsigned char>((const unsigned char*)pszTimestamp,
            (const unsigned char*)pszTimestamp + strlen(pszTimestamp));

        std::vector<CTxOut> vout;
        vout.resize(1);
        vout[0].SetEmpty();

        CTransaction txNew(1, 1496692022, vin, vout, 0);

        genesis.vtx.push_back(txNew);
        genesis.hashPrevBlock = 0;
        genesis.hashMerkleRoot = genesis.BuildMerkleTree();
        genesis.nVersion = 1;
        genesis.nTime    = 1504550000;
        genesis.nBits    = 0x1e0fffff;
        genesis.nNonce   = 1454059;

        hashGenesisBlock = genesis.GetHash();
        assert(hashGenesisBlock == uint256("0xe94755b352f86725342f816a148ba490e07df1d7ae3a135fc0632ae4a83f8e81"));
        assert(genesis.hashMerkleRoot == uint256("0x5cd583483a3f368810466dd6ac054dc5bc75b46ef03952e8ed7eefd3b32a8160"));

        vSeeds.push_back(CDNSSeedData("node1.swippcoin.com", "node1.swippcoin.com"));
        vSeeds.push_back(CDNSSeedData("node2.swippcoin.com", "node2.swippcoin.com"));

        base58Prefixes[PUBKEY_ADDRESS] = std::vector<unsigned char>(1,63);
        base58Prefixes[SCRIPT_ADDRESS] = std::vector<unsigned char>(1,5);
        base58Prefixes[SECRET_KEY] =     std::vector<unsigned char>(1,155);
        base58Prefixes[EXT_PUBLIC_KEY] = list_of(0x04)(0x88)(0xB2)(0x1E).convert_to_container<std::vector<unsigned char> >();
        base58Prefixes[EXT_SECRET_KEY] = list_of(0x04)(0x88)(0xAD)(0xE4).convert_to_container<std::vector<unsigned char> >();

        convertSeed6(vFixedSeeds, pnSeed6_main, ARRAYLEN(pnSeed6_main));
    }

    virtual const CBlock& GenesisBlock() const { return genesis; }
    virtual Network NetworkID() const { return CChainParams::MAIN; }
    virtual const vector<CAddress>& FixedSeeds() const { return vFixedSeeds; }

protected:
    CBlock genesis;
    vector<CAddress> vFixedSeeds;
};

static CMainParams mainParams;

// Testnet

class CTestNetParams : public CMainParams
{
public:
    CTestNetParams()
    {
        pchMessageStart[0] = 0xe1;
        pchMessageStart[1] = 0xc4;
        pchMessageStart[2] = 0xf8;
        pchMessageStart[3] = 0x21;

        bnProofOfWorkLimit = CBigNum(~uint256(0) >> 16);
        vAlertPubKey = ParseHex("04ef763402f735d3da325e884fa011074f621f3cdaa8f7281b2d4449a6d5"
                                "ecfc4728347688eb33784e1cc09d950f107fce7b218c970b80db2b8f072f6f8f532296");
        nDefaultPort = 18065;
        nRPCPort = 15075;
        strDataDir = "testnet";

        // Modify the testnet genesis block so the timestamp is valid for a later start

        genesis.nBits  = 0x1e0fffff; //504365055 Decimal Number
        genesis.nTime  = 1504550001;
        genesis.nNonce = 1454059;

        assert(hashGenesisBlock == uint256("0xe94755b352f86725342f816a148ba490e07df1d7ae3a135fc0632ae4a83f8e81"));

        vFixedSeeds.clear();
        vSeeds.clear();

        base58Prefixes[PUBKEY_ADDRESS] = std::vector<unsigned char>(1,77);
        base58Prefixes[SCRIPT_ADDRESS] = std::vector<unsigned char>(1,192);
        base58Prefixes[SECRET_KEY]     = std::vector<unsigned char>(1,239);
        base58Prefixes[EXT_PUBLIC_KEY] = list_of(0x04)(0x35)(0x87)(0xCF).convert_to_container<std::vector<unsigned char> >();
        base58Prefixes[EXT_SECRET_KEY] = list_of(0x04)(0x35)(0x83)(0x94).convert_to_container<std::vector<unsigned char> >();

        convertSeed6(vFixedSeeds, pnSeed6_test, ARRAYLEN(pnSeed6_test));
    }

    virtual Network NetworkID() const { return CChainParams::TESTNET; }
};

static CTestNetParams testNetParams;
static CChainParams *pCurrentParams = &mainParams;

const CChainParams &Params()
{
    return *pCurrentParams;
}

void SelectParams(CChainParams::Network network)
{
    switch (network)
    {
        case CChainParams::MAIN:
            pCurrentParams = &mainParams;
            break;
        case CChainParams::TESTNET:
            pCurrentParams = &testNetParams;
            break;
        default:
            assert(false && "Unimplemented network");
            return;
    }
}

bool SelectParamsFromCommandLine()
{
    bool fTestNet = GetBoolArg("-testnet", false);

    if (fTestNet)
        SelectParams(CChainParams::TESTNET);
    else
        SelectParams(CChainParams::MAIN);

    return true;
}
