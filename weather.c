#include "weather.h"
#include "curl/curl.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

typedef struct {
    char *data;
    size_t size;
} response_t;

void parse_response(const response_t *response)
{
    printf("%s", response->data);
}

size_t cb(void *data, size_t size, size_t nmemb, void *clientp)
{
    size_t realsize = size * nmemb;
    response_t *chunk = (response_t*)clientp;

    char *ptr = realloc(chunk->data, chunk->size + realsize + 1);
    if (!ptr) {
        fprintf(stderr, "make response chank failed: insufficient memory!\n");
        return 0;
    }

    chunk->data = ptr;
    memcpy(&(chunk->data[chunk->size]), data, realsize);
    chunk->size += realsize;
    chunk->data[chunk->size] = 0;

    return realsize;
}

CURLcode request(const char *url)
{
    CURLcode res;
    curl_global_init(CURL_GLOBAL_DEFAULT);
 
    response_t response = {NULL, 0};

    CURL *curl = curl_easy_init();
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
    
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, cb);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

        res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            fprintf(stderr, "request error: %s\n", curl_easy_strerror(res));
        } 

        parse_response(&response);
        free(response.data);
        curl_easy_cleanup(curl);
    }
    
    curl_global_cleanup();
    return res;
}

const char *make_url(const char *area_name) 
{
    const char *str_url  = "https://wttr.in/";
    const char *str_frmt = "?format=j1";
    size_t len_url = strlen(str_url);
    size_t len_frmt = strlen(str_frmt);
    size_t len_area_name = strlen(area_name);

    char *res = malloc(len_url + len_area_name + len_frmt + 1);
    if (!res) {
        fprintf(stderr, "make url failed: insufficient memory!\n");
        return NULL;
    }
    
    memcpy(res, str_url, len_url);
    memcpy(res + len_url, area_name, len_area_name);
    memcpy(res + len_url + len_area_name, str_frmt, len_frmt + 1);
    return res;
}

void show_weather(const char *area_name)
{
    const char *url = make_url(area_name);
    request(url);
    free((void*)url);
}
