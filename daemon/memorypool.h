// Copyright (c) 2017-2019 The Swipp developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING.daemon or http://www.opensource.org/licenses/mit-license.php.

#include <functional>
#include <vector>

#include "sync.h"

template <class T> class MemoryPool
{
private:
    CCriticalSection cs_memoryPool;
    std::vector<T *> objects;
    std::function<void(T *object)> initializer;

public:
    MemoryPool(std::function<void(T *object)> initializer = [](T *object) -> void { });
    ~MemoryPool();
    MemoryPool(const MemoryPool& m) { /* NOP */ }
    T *fetch();
    void put(T *object);
};

