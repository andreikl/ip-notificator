// MIT License

// Copyright (c) 2021 Andrei Klimchuk <andrew.klimhcuk@gmail.com>

// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include <curl/curl.h>

#include "main.h"
#include "utils.h"
#include "http.h"

#define EXTERNAL_IP_PROVIDER "https://myexternalip.com/raw\0"

int http_init()
{
    CURL_CALL(curl_global_init(CURL_GLOBAL_ALL), exit);
    return 0;

exit:
    return EAGAIN;
}

void http_cleanup()
{
    curl_global_cleanup();
}

int write_data(void *ptr, unsigned size, unsigned nmemb, char* data) {
    ASSERT_INT(size * nmemb, >, MAX_STRING, exit);
    //DEBUG_INT("write_data nmemb", nmemb);
    memcpy(data, ptr, size * nmemb);
    return size * nmemb;
exit:
    return -1;
}

int http_get_ip(struct app_state_t *app)
{
    CURL *curl = curl_easy_init();
    ASSERT_POINTER(curl, ==, NULL, cleanup);

    curl_easy_setopt(curl, CURLOPT_URL, EXTERNAL_IP_PROVIDER);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);

    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, app->ip);
    //char header[MAX_STRING];
    //curl_easy_setopt(curl, CURLOPT_WRITEHEADER, header);

    CURL_CALL(curl_easy_perform(curl), cleanup);

    unsigned response_code;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);
    ASSERT_INT(response_code, !=, 200, cleanup);

    curl_easy_cleanup(curl);
    return 0;

cleanup:
    if (curl) {
        curl_easy_cleanup(curl);
    }

    return EAGAIN;
}
