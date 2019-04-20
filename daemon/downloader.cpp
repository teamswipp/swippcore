// Copyright (c) 2017-2019 The Swipp developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING.daemon or http://www.opensource.org/licenses/mit-license.php.

#include <curl/curl.h>

#include "downloader.h"
#include "util.h"

#define CURL_CONNECTION_TIMEOUT (5 * 1000)

static size_t handle_file_chunk(void *downloaded, size_t size, size_t nmemb, void *destination)
{
    return fwrite(downloaded, size, nmemb, (FILE *) destination);
}

static size_t handle_string_chunk(void *downloaded, size_t size, size_t nmemb, void *destination)
{
    ((std::string *) destination)->append((char *) downloaded);
    return size * nmemb;
}

Downloader::Downloader(std::string url) : url(url)
{
    curl = curl_easy_init();

    if(curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
        curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, CURL_CONNECTION_TIMEOUT);

        #ifdef WIN32
        // NOTE: The data here is not critical, so disabling verification should not have any unforseen consequences.
        // However, we only do this under Windows, as certificates are usually bundled with Linux and OS X distributions.
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
        #endif
    }
}

Downloader::Downloader(std::string url, std::string& destination) : Downloader(url)
{
    if(curl) {
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, handle_string_chunk);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, destination);
    }
}

Downloader::Downloader(std::string url, FILE *destination) : Downloader(url)
{
    if(curl) {
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, handle_file_chunk);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, destination);
    }
}

Downloader::~Downloader()
{
    if (curl) {
        curl_easy_cleanup(curl);
    }
}

void Downloader::fetch()
{
    CURLcode res = curl_easy_perform(curl);

    if(res != CURLE_OK) {
        LogPrintf("Download of file failed: %s\n", curl_easy_strerror(res));
    }

}

