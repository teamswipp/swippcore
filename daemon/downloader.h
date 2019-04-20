// Copyright (c) 2017-2019 The Swipp developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING.daemon or http://www.opensource.org/licenses/mit-license.php.

#include <cstdio>
#include <curl/curl.h>
#include <string>

class Downloader
{
private:
    CURL *curl = NULL;
    FILE *file = NULL;
    std::string url;

    Downloader(std::string url);

public:
    Downloader(std::string url, std::string& destination);
    Downloader(std::string url, std::FILE *destination);
    ~Downloader();

    void fetch();
};
