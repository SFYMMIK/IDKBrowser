#include "NetworkManager.h"
#include <curl/curl.h>
#include <stdlib.h>
#include <string.h>

static CURL *curl;

void initialize_network_manager() {
    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();
}

void cleanup_network_manager() {
    if (curl) {
        curl_easy_cleanup(curl);
    }
    curl_global_cleanup();
}

static size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    size_t totalSize = size * nmemb;
    char **response = (char **)userp;

    *response = realloc(*response, strlen(*response) + totalSize + 1);
    strncat(*response, (char *)contents, totalSize);

    return totalSize;
}

char* fetch_page(const char* url) {
    if (!curl) {
        return NULL;
    }

    char *response = malloc(1);
    response[0] = '\0';

    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

    CURLcode res = curl_easy_perform(curl);
    if (res != CURLE_OK) {
        free(response);
        return NULL;
    }

    // Additionally fetch JavaScript files if needed
    // Logic for fetching linked JavaScript files can be added here

    return response;
}