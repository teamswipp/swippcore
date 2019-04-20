// Copyright (c) 2017-2019 The Swipp developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING.daemon or http://www.opensource.org/licenses/mit-license.php.

#include <cstdio>
#include <cstring>
#include <openssl/sha.h>
#include <sstream> 
#include <iomanip>

#include "bsarchive.h"
#include "xz/xz.h"
#include "util.h"

BSArchive::BSArchive(std::FILE *file) : file(file)
{
    xz_crc32_init();
}

bool BSArchive::verifyHash()
{
    char fileHash[SHA256_DIGEST_LENGTH * 2 + 1];
    unsigned char computedHash[SHA256_DIGEST_LENGTH];

    std::rewind(file);
    std::fread(fileHash, 1, SHA256_DIGEST_LENGTH * 2, file);

    char buffer[1024 * 1024 * 2]; /* 2MB */
    SHA256_CTX sha256;
    SHA256_Init(&sha256);

    while (true) {
        size_t count = std::fread(buffer, 1, sizeof(buffer), file);

        if (count > 0) {
            SHA256_Update(&sha256, buffer, count);
        }

        if (count < sizeof(buffer))
            break;
    }

    SHA256_Final(computedHash, &sha256);
    std::stringstream computedHashStream;

    for (size_t i = 0; i < SHA256_DIGEST_LENGTH; i++)
       computedHashStream << std::hex << std::setfill('0') << std::setw(2) << (unsigned short) computedHash[i];

    LogPrintf("Computed file hash: %s\nInput file hash: %s\n", computedHashStream.str(), fileHash);
    return std::string(fileHash) == computedHashStream.str();
}

int BSArchive::unarchive(std::FILE *destination)
{
    unsigned char in[1024 * 1024]; /* 1MB */
    unsigned char out[1024 * 1024]; /* 1MB */
    struct xz_buf b;
    enum xz_ret ret;
    struct xz_dec *s = xz_dec_init(XZ_DYNALLOC, 1 << 24); /* 16MB dictionary */

    if(s) {
        b.in = in;
        b.in_pos = 0;
        b.in_size = sizeof(in);

        b.out = out;
        b.out_pos = 0;
        b.out_size = sizeof(out);

        std::fseek(file, SHA256_DIGEST_LENGTH * 2, SEEK_SET);
        std::fread((void *) b.in, 1, b.in_size, file);

        while (true) {
            ret = xz_dec_run(s, &b);

            if(b.out_pos == sizeof(out)) {
                std::fwrite(b.out, 1, b.out_size, destination);
                b.out_pos = 0;
            }

            if(b.in_pos == sizeof(in)) {
                std::fread((void *) b.in, 1, b.in_size, file);
                b.in_pos = 0;
            }

            if(ret == XZ_OK)
                continue;

            std::fwrite(b.out, 1, b.out_pos, destination);
            std::fread((void *) b.in, 1, b.in_pos, file);

            if(ret == XZ_STREAM_END) {
                xz_dec_end(s);
                std::fflush(destination);
                return 0;
            }

            break;
        }
    }

    xz_dec_end(s);
    return -1;
}

