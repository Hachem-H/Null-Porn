#include <curl/curl.h>

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "NullPorn.h"

static size_t s_WriteCallback(char* pointer, size_t size, size_t nmemb, void* userPointer)
{
    return size*nmemb;
}

bool HTTPGetRequest(const char* url)
{
    CURL* curl = curl_easy_init();
    bool retValue = true;

    if (!curl)
        return false;
    
    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, s_WriteCallback);

    CURLcode result = curl_easy_perform(curl);
    if (result != CURLE_OK)
    {
        FILE* logFile = fopen("NullPorn-LOG.txt", "w");
        fprintf(logFile, "[ERR (%s) curl_easy_perform()]: %s\n", url, curl_easy_strerror(result));
        fclose(logFile);

        retValue = false;
    }

    curl_easy_cleanup(curl);
    return retValue;
}