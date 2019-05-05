// Copyright (c) 2017-2019 The Swipp developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING.daemon or http://www.opensource.org/licenses/mit-license.php.

#ifndef SWIPP_COLLECTIONHASHING_H
#define SWIPP_COLLECTIONHASHING_H

#include <cstddef>
#include <utility>
#include <tr1/functional>
#include <boost/functional/hash.hpp>

#include "core.h"
#include "uint256.h"

namespace std {
    namespace tr1 {
        template<> inline std::size_t hash<uint256>::operator()(uint256 v) const
        {
            return boost::hash_range(v.begin(), v.end());
        }

        template<> inline std::size_t hash<std::tuple<uint160, uint256>>::operator()(std::tuple<uint160, uint256> v) const
        {
            std::size_t dest = boost::hash_range(std::get<0>(v).begin(), std::get<0>(v).end());
            boost::hash_combine(dest, boost::hash_range(std::get<1>(v).begin(), std::get<1>(v).end()));
            return dest;
        }
    }
}

#endif

