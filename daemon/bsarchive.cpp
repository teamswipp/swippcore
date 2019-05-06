// Copyright (c) 2017-2019 The Swipp developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING.daemon or http://www.opensource.org/licenses/mit-license.php.

#include <cstdio>
#include <cstring>
#include <openssl/sha.h>
#include <sstream> 
#include <iomanip>

#include "bsarchive.h"
#include "init.h"
#include "xz/xz.h"
#include "util.h"

#define BUFFER_SIZE 1024 * 1024 /* 1MB */

BSArchive::BSArchive(std::FILE *file, std::function<void(double percentage)> progress) : file(file), progress(progress)
{
    in = new unsigned char[BUFFER_SIZE];
    out = new unsigned char[BUFFER_SIZE];
    xz_crc32_init();
}

BSArchive::~BSArchive()
{
    delete[] in;
    delete[] out;
}

bool BSArchive::verifyHash()
{
    unsigned char fileHash[SHA256_DIGEST_LENGTH * 2 + 1] = { 0 };
    unsigned char *computedHash = new unsigned char[SHA256_DIGEST_LENGTH];

    std::rewind(file);

    if (std::fread(fileHash, 1, SHA256_DIGEST_LENGTH * 2, file) == 0) {
        if (std::ferror(file)) {
            LogPrintf("Failed to read hash from bootstrap archive");
            delete[] computedHash;
            return false;
        }
    }

    unsigned char *buffer = new unsigned char[BUFFER_SIZE];
    SHA256_CTX sha256;
    SHA256_Init(&sha256);

    while (true) {
        size_t count = std::fread(buffer, 1, BUFFER_SIZE, file);

        if (count > 0) {
            SHA256_Update(&sha256, buffer, count);
        }

        if (count < BUFFER_SIZE)
            break;
    }

    SHA256_Final(computedHash, &sha256);
    std::stringstream computedHashStream;

    for (size_t i = 0; i < SHA256_DIGEST_LENGTH; i++)
       computedHashStream << std::hex << std::setfill('0') << std::setw(2) << (unsigned short) computedHash[i];

    LogPrintf("Computed file hash: %s\nInput file hash: %s\n", computedHashStream.str(), fileHash);

    delete[] buffer;
    delete[] computedHash;
    return std::string((char *) fileHash) == computedHashStream.str();
}

int BSArchive::unarchive(std::FILE *destination)
{
    progress(0.0);

    struct xz_buf b;
    enum xz_ret ret;
    struct xz_dec *s = xz_dec_init(XZ_DYNALLOC, 1 << 24); /* 16MB dictionary */

    if(s) {
        b.in = in;
        b.in_pos = 0;
        b.in_size = BUFFER_SIZE;

        b.out = out;
        b.out_pos = 0;
        b.out_size = BUFFER_SIZE;

        std::fseek(file, 0, SEEK_END);
        size_t size = std::ftell(file);
        int pos = 0;

        std::fseek(file, SHA256_DIGEST_LENGTH * 2, SEEK_SET);

        if (std::fread((void *) b.in, 1, b.in_size, file) == 0) {
            if (std::ferror(file)) {
                LogPrintf("Failed to read intial data from bootstrap archive");
                return -1;
            }
        }

        while (!ShutdownRequested()) {
            ret = xz_dec_run(s, &b);

            if(b.out_pos == BUFFER_SIZE) {
                std::fwrite(b.out, 1, b.out_size, destination);
                b.out_pos = 0;
            }

            if(b.in_pos == BUFFER_SIZE) {
                if (std::fread((void *) b.in, 1, b.in_size, file) == 0) {
                    if (std::ferror(file)) {
                        LogPrintf("Failed to read continous data from bootstrap archive");
                        break;
                    }
                }

                pos += b.in_pos;
                progress((double) pos / size * 100);
                b.in_pos = 0;
            }

            if(ret == XZ_OK)
                continue;

            std::fwrite(b.out, 1, b.out_pos, destination);

            if (std::fread((void *) b.in, 1, b.in_pos, file) == 0) {
                if (std::ferror(file)) {
                    LogPrintf("Failed to read stub data from bootstrap archive");
                    break;
                }
            }

            if(ret == XZ_STREAM_END) {
                xz_dec_end(s);
                std::fflush(destination);
                progress(100.0);
                return 0;
            }

            break;
        }
    }

    xz_dec_end(s);
    return -1;
}

