// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2012 The Bitcoin developers
// Copyright (c) 2017-2018 The Swipp developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <boost/filesystem.hpp>
#include <boost/filesystem/path.hpp>
#include <string>
#include <stdlib.h>

#include "tinyformat.h"
#include "util.h"

static unsigned int nCurrentBlockFile = 1;

static inline boost::filesystem::path BlockFilePath(unsigned int nFile)
{
    std::string strBlockFn = strprintf("blk%04u.dat", nFile);
    return GetDataDir() / strBlockFn;
}

FILE* OpenBlockFile(unsigned int nFile, unsigned int nBlockPos, const char* pszMode)
{
    if ((nFile < 1) || (nFile == (unsigned int) - 1))
        return NULL;

    FILE* file = fopen(BlockFilePath(nFile).string().c_str(), pszMode);

    if (!file)
        return NULL;

    if (nBlockPos != 0 && !strchr(pszMode, 'a') && !strchr(pszMode, 'w'))
    {
        if (fseek(file, nBlockPos, SEEK_SET) != 0)
        {
            fclose(file);
            return NULL;
        }
    }

    return file;
}

FILE* AppendBlockFile(unsigned int& nFileRet)
{
    nFileRet = 0;

    while (true)
    {
        FILE* file = OpenBlockFile(nCurrentBlockFile, 0, "ab");

        if (!file)
            return NULL;
        if (fseek(file, 0, SEEK_END) != 0)
            return NULL;

        // FAT32 file size max 4GB, fseek and ftell max 2GB, so we must stay under 2GB
        if (ftell(file) < (long)(0x7F000000 - MAX_SIZE))
        {
            nFileRet = nCurrentBlockFile;
            return file;
        }

        fclose(file);
        nCurrentBlockFile++;
    }
}
