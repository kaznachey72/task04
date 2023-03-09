#include "weather.h"
#include "curl/curl.h"
#include "cjson/cJSON.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

typedef struct {
    char *data;
    size_t size;
} response_t;

const char *get_value_str(const cJSON *json, const char *name)
{
    cJSON *json_str = cJSON_GetObjectItemCaseSensitive(json, name);
    bool is_valid = name && json && cJSON_IsString(json_str) && json_str->valuestring;
    return (is_valid ? json_str->valuestring : "?");
}

void parse_response(const response_t *response)
{
    cJSON* json_root = cJSON_Parse(response->data);
    if (!json_root) {
        const char *error = cJSON_GetErrorPtr();
        fprintf(stderr, "Error: %s\n", (error ? error : "parse fail"));
        return;
    }   

    cJSON *json_area = cJSON_GetObjectItem(json_root, "nearest_area");
    cJSON *json_area_0 = cJSON_GetArrayItem(json_area, 0);
    cJSON *json_area_name = cJSON_GetObjectItem(json_area_0, "areaName");
    cJSON *json_area_name_0 = cJSON_GetArrayItem(json_area_name, 0);
    printf("area name:\t%s\n", get_value_str(json_area_name_0, "value"));

    cJSON *json_currcond = cJSON_GetObjectItem(json_root, "current_condition");
    cJSON *json_currcond_0 = cJSON_GetArrayItem(json_currcond, 0); 

    cJSON *json_desc = cJSON_GetObjectItem(json_currcond_0, "weatherDesc");
    cJSON *json_desc_0 = cJSON_GetArrayItem(json_desc, 0); 

    printf("weather desc:\t%s\n", get_value_str(json_desc_0, "value"));
    printf("wind dir:\t%s\n", get_value_str(json_currcond_0, "winddir16Point"));
    printf("wind speed:\t%s km/h\n", get_value_str(json_currcond_0, "windspeedKmph"));

    cJSON *json_weather = cJSON_GetObjectItem(json_root, "weather");
    cJSON *json_weather_0 = cJSON_GetArrayItem(json_weather, 0); 
    printf("temp range:\t%s..%s C\n", get_value_str(json_weather_0, "mintempC"), 
                                      get_value_str(json_weather_0, "maxtempC"));

    cJSON_Delete(json_root);
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
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10L);

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
