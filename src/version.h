// Copyright (c) 2012 The Bitcoin developers
// Copyright (c) 2017-2018 The Swipp developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.
#ifndef BITCOIN_VERSION_H
#define BITCOIN_VERSION_H

#include "clientversion.h"
#include <sstream>
#include <string>

//
// client versioning
//

static const int CLIENT_VERSION =
                           1000000 * CLIENT_VERSION_MAJOR
                         +   10000 * CLIENT_VERSION_MINOR
                         +     100 * CLIENT_VERSION_REVISION
                         +       1 * CLIENT_VERSION_BUILD;

extern const std::string CLIENT_NAME;
extern const std::string CLIENT_BUILD;
extern const std::string CLIENT_DATE;

//
// database format versioning
//
static const int DATABASE_VERSION = 71500;

//
// network protocol versioning
//

static const int PROTOCOL_VERSION = 69210;

// intial proto version, to be increased after version/verack negotiation
static const int INIT_PROTO_VERSION = 209;

// nTime field added to CAddress, starting with this version;
// if possible, avoid requesting addresses nodes older than this
static const int CADDR_TIME_VERSION = 31402;

// only request blocks from nodes outside this range of versions
static const int NOBLKS_VERSION_START = 60002;
static const int NOBLKS_VERSION_END = 60006;

// BIP 0031, pong message, is enabled for all versions AFTER this one
static const int BIP0031_VERSION = 60000;

// "mempool" command, enhanced "getdata" behavior starts with this version:
static const int MEMPOOL_GD_VERSION = 60005;

enum BlockBreakVersionType
{
    INSTANTX,
    MASTERNODE,
    PEER
};

bool isVersionCompatible(BlockBreakVersionType fbVersionType, int version, int nHeight);
int getBlockVersion(BlockBreakVersionType fbVersionType, int nHeight);

// minimum protocol versions required for the last block break.

static const int MIN_PEER_PROTO_VERSION = 69210;
static const int MIN_INSTANTX_PROTO_VERSION = 69210;
static const int MIN_MN_PROTO_VERSION = 69210;

static const int LAST_BLOCK_BREAK = 140000;

struct ComparableVersion
{
    int major = 0, minor = 0, revision = 0, build = 0;

    ComparableVersion(std::string version)
    {
        std::sscanf(version.c_str(), "%d.%d.%d.%d", &major, &minor, &revision, &build);
    }

    bool operator < (const ComparableVersion& other) const
    {
        if (major < other.major)
            return true;
        else if (minor < other.minor)
            return true;
        else if (revision < other.revision)
            return true;
        else if (build < other.build)
            return true;
        return false;
    }

    bool operator == (const ComparableVersion& other)
    {
        return major == other.major
            && minor == other.minor
            && revision == other.revision
            && build == other.build;
    }

    friend std::ostream& operator << (std::ostream& stream, const ComparableVersion& ver)
    {
        stream << ver.major;
        stream << '.';
        stream << ver.minor;
        stream << '.';
        stream << ver.revision;
        stream << '.';
        stream << ver.build;
        return stream;
    }

    std::string ToString() const
    {
        std::stringstream s;
        s << major << "." << minor << "." << revision << "." << build;
        return s.str();
    }
};
#endif
