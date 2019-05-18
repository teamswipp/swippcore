// Copyright (c) 2017-2019 The Swipp developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING.daemon or http://www.opensource.org/licenses/mit-license.php.

#include <cstdio>
#include <curl/curl.h>
#include <functional>
#include <string>

class Downloader
{
private:
    CURL *curl = NULL;
    FILE *file = NULL;
    std::string url;
    std::function<void(double percentage)> progress;

    Downloader(std::string url, std::function<void(double percentage)> progress);

public:
    Downloader(std::string url, std::string *destination,
               std::function<void(double percentage)> progress = [](double percentage) -> void { });
    Downloader(std::string url, std::FILE *destination,
               std::function<void(double percentage)> progress = [](double percentage) -> void { });
    ~Downloader();

    friend int handle_progress(void *downloader, curl_off_t dltotal, curl_off_t dlnow, curl_off_t ultotal, curl_off_t ulnow);
    void fetch();
};
