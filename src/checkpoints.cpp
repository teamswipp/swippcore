// Copyright (c) 2009-2012 The Bitcoin developers
// Copyright (c) 2017-2018 The Swipp developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <boost/assign/list_of.hpp> // For 'map_list_of()'
#include <boost/foreach.hpp>

#include "checkpoints.h"
#include "txdb.h"
#include "main.h"
#include "uint256.h"

static const int nCheckpointSpan = 500;

namespace Checkpoints
{
    typedef std::map<int, uint256> MapCheckpoints;

    // What makes a good checkpoint block?
    // * Is surrounded by blocks with reasonable timestamps
    //   (no blocks before with a timestamp after, none after with timestamp before)
    // * Contains no strange transactions
    static MapCheckpoints mapCheckpoints = boost::assign::map_list_of
        ( 0,       uint256("0xe94755b352f86725342f816a148ba490e07df1d7ae3a135fc0632ae4a83f8e81"))
        ( 100,     uint256("0x0000000064300e993d53071ecb44a86464d920f2e867f9cb3343a5fa1dd6dbfd"))
        ( 86900,   uint256("0xb92c39195ccbadafb2d1481ddce36bf02c638190444e31bbb2d5c195d8e9ad1f"))
        ( 129900,  uint256("0x00000000000016510b773f104a7570e6182aee8d3ca043259b9b322b63f17a12"))
        ( 200000,  uint256("00000000000037ce274603723ee73def283ab67a485cffeed9d7e4f46340bf74"))
        ( 215000,  uint256("00000000000073b140f1bcc473e7682d0dd3e76f838968b61bb277f95173e7d2"))
        ( 250000,  uint256("000000000002b5d8f396e133908c3fcaa46b8e462ea24f220e585ea2677cb41f"))
        ( 300000,  uint256("496468e6e9d554d92f07321a683e29ef4f2b3b10826af931674f165af4fd9846"))
        ( 350000,  uint256("d9b8b964eeb8d1931495de2e2488f07a7d4fba50f9fdbefe8e7c23eba11a25d1"))
        ( 400000,  uint256("b1a87f4e91903d4e5adff4498d97be8fc4fe7b59506fc913bdb585df07b22acb"))
        ( 412500,  uint256("000000000006cd1fc6577888a54952b4bb4a033ddcbb54d7d2d436e534d27308"))
        ( 431000,  uint256("00000000000d2d511716a25436269c2e1076b877ca64530643558487f1a29bc0"))
        ( 500000,  uint256("000000000005eda1277caafc429ff9c6f6a5add63773bf4e57fb3487d5b7629c"))
        ( 540000,  uint256("00000000001117d47acea84bc2c54136dbb7b088946d3256acfc1e5593fd5cd0"))
        ( 600001,  uint256("00000000000a3ad6ee8590980fe3e63e5ea94f5acb2e942fbca1baefb1ab0155"))
        ( 650001,  uint256("00000000000f764b0ec96fd91263ee971c01c7916ec6757264f05e831f01c05d"))
        ( 800001,  uint256("00000000000707da3369e86fd2be5baf196ef5dcce5bf3eba5e2015dd4f6f886"))
        ( 950011,  uint256("6d53dbb48a1bbed7caebd7bd2b552ae1d38638b8895bfb125685f978d37f4d21"))
        ( 1000555, uint256("e0c4062dae8f3acd831291ec0952bd0915fd3da9a20bf00b78d4765e5f9a7d82"))
        ( 1150021, uint256("19113353a8b6ce530985e833da0e5dd516b0ece7aa8f6a54364ce97484286573"))
        ( 1255051, uint256("012ce3ff85e30da375047fe842e49721c78c5e828f027ee60ba69d82350d0595"));

    // TestNet has no checkpoints
    static MapCheckpoints mapCheckpointsTestnet;

    bool CheckHardened(int nHeight, const uint256& hash)
    {
        MapCheckpoints& checkpoints = (TestNet() ? mapCheckpointsTestnet : mapCheckpoints);
        MapCheckpoints::const_iterator i = checkpoints.find(nHeight);

        if (i == checkpoints.end())
            return true;

        return hash == i->second;
    }

    int GetTotalBlocksEstimate()
    {
        MapCheckpoints& checkpoints = (TestNet() ? mapCheckpointsTestnet : mapCheckpoints);

        if (checkpoints.empty())
            return 0;

        return checkpoints.rbegin()->first;
    }

    CBlockIndex* GetLastCheckpoint(const std::map<uint256, CBlockIndex*>& mapBlockIndex)
    {
        MapCheckpoints& checkpoints = (TestNet() ? mapCheckpointsTestnet : mapCheckpoints);

        BOOST_REVERSE_FOREACH(const MapCheckpoints::value_type& i, checkpoints)
        {
            const uint256& hash = i.second;
            std::map<uint256, CBlockIndex*>::const_iterator t = mapBlockIndex.find(hash);

            if (t != mapBlockIndex.end())
                return t->second;
        }

        return NULL;
    }

    // Automatically select a suitable sync-checkpoint
    const CBlockIndex* AutoSelectSyncCheckpoint()
    {
        const CBlockIndex *pindex = pindexBest;

        // Search backward for a block within max span and maturity window
        while (pindex->pprev && pindex->nHeight + nCheckpointSpan > pindexBest->nHeight)
            pindex = pindex->pprev;

        return pindex;
    }

    // Check against synchronized checkpoint
    bool CheckSync(int nHeight)
    {
        const CBlockIndex* pindexSync = AutoSelectSyncCheckpoint();

        if (nHeight <= pindexSync->nHeight)
            return false;

        return true;
    }
}
