// Copyright (c) 2017-2019 The Swipp developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING.daemon or http://www.opensource.org/licenses/mit-license.php.

#include <cstdio>

class BSArchive
{
private:
    std::FILE *file;

public:
    BSArchive(std::FILE *file);

    bool verifyHash();
    int unarchive(std::FILE *destination);
};
