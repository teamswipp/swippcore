// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2012 The Bitcoin developers
// Copyright (c) 2017-2018 The Swipp developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef SWIPP_CONSTRAINTS_H
#define SWIPP_CONSTRAINTS_H

#include <cstdint>

static const int64_t COIN = 100000000;
static const int64_t CENT = 1000000;

inline bool MoneyRange(int64_t nValue)
{
    //TODO: Add check for both
    return nValue >= 0 && nValue <= 5000000 * COIN;
}

#endif /* SWIPP_CONSTRAINTS_H */
