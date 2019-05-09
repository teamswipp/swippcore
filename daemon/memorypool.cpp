// Copyright (c) 2017-2019 The Swipp developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING.daemon or http://www.opensource.org/licenses/mit-license.php.

#include <functional>
#include <map>
#include <set>
#include <sparsehash/dense_hash_set>

#include "memorypool.h"
#include "sync.h"
#include "txdb.h"
#include "uint256.h"

template class MemoryPool<std::set<uint256>>;
template class MemoryPool<google::dense_hash_set<std::tuple<uint160, uint256>>>;

template <class T> MemoryPool<T>::MemoryPool(std::function<void(T *object)> initializer) : initializer(initializer)
{
    /* Intentionally left blank */
}

template <class T> MemoryPool<T>::~MemoryPool()
{
    LOCK(cs_memoryPool);
    for (auto object : objects) {
        delete object;
    }
}

template <class T> T *MemoryPool<T>::fetch()
{
    T *object = nullptr;

    {
        LOCK(cs_memoryPool);

        if (!objects.empty()) {
            object = objects.back();
            objects.pop_back();
        }
    }

    if (object == nullptr) {
        object = new T();
        initializer(object);
    }

    return object;
}

template <class T> void MemoryPool<T>::put(T *object)
{
    LOCK(cs_memoryPool);
    objects.push_back(object);
}

