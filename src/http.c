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

static size_t write_data(void *ptr, size_t size, size_t nmemb, char* data) {
    ASSERT_LNG(size * nmemb, >, MAX_STRING, exit);
    //DEBUG_LNG("write_data nmemb", nmemb);
    memcpy(data, ptr, size * nmemb);
    return size * nmemb;
exit:
    return -1;
}

int http_get_ip(struct app_state_t *app)
{
    CURL *curl = curl_easy_init();
    ASSERT_PTR(curl, ==, NULL, cleanup);

    curl_easy_setopt(curl, CURLOPT_URL, EXTERNAL_IP_PROVIDER);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, app->ip);
    //char header[MAX_STRING];
    //curl_easy_setopt(curl, CURLOPT_WRITEHEADER, header);

    CURL_CALL(curl_easy_perform(curl), cleanup);

    long response_code;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);
    ASSERT_LNG(response_code, !=, 200, cleanup);

    curl_easy_cleanup(curl);
    return 0;

cleanup:
    if (curl) {
        curl_easy_cleanup(curl);
    }

    return EAGAIN;
}

static int read_data(char *ptr, int size, int nmemb, void *userp)
{
    struct app_state_t *app = (struct app_state_t *)userp;

    int len = strlen(app->email_data);
    if (app->email_read >= len) { //all data has been copied
        return 0;
    }
    ASSERT_INT(size * nmemb, <, len, exit);
    if (app->verbose) {
        DEBUG_STR("email_data", app->email_data);
    }
    memcpy(ptr, app->email_data, len);

    app->email_read = len;
    return len;
 
exit:
    return -1;
}

int http_send_email(struct app_state_t *app)
{
    int res = EAGAIN;
    struct curl_slist *recipients = NULL;

    CURL *curl = curl_easy_init();
    ASSERT_PTR(curl, ==, NULL, cleanup);

    strcpy(app->email_data, "Date: ");
    ASSERT_INT(utils_get_time(app->buffer, sizeof(app->buffer)), ==, 0, cleanup);
    strncat(app->email_data, app->buffer, sizeof(app->email_data));
    strcat(app->email_data, "\r\n");
    strcat(app->email_data, "To: <");
    strcat(app->email_data, app->email_to);
    strcat(app->email_data, ">\r\n");
    strcat(app->email_data, "From: <");
    strcat(app->email_data, app->email_user);
    strcat(app->email_data, "> (ip-notificator)\r\n");
    strcat(app->email_data, "Message-ID: <");
    CALL(utils_get_uuid(app->buffer, sizeof(app->buffer)), cleanup);
    strncat(app->email_data, app->buffer, sizeof(app->email_data));
    strcat(app->email_data, "@ip-notificator>\r\n");
    strcat(app->email_data, "Subject: Public IP has been changed!!!\r\n");
    strcat(app->email_data, "\r\n"); //empty line to divide headers from body, see RFC5322
    strcat(app->email_data, "New IP is: ");
    strcat(app->email_data, app->ip);
    strcat(app->email_data, "\r\n");

    curl_easy_setopt(curl, CURLOPT_USERNAME, app->email_user);
    curl_easy_setopt(curl, CURLOPT_PASSWORD, app->email_secret);
    curl_easy_setopt(curl, CURLOPT_URL, app->email_smtp);

    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);

    curl_easy_setopt(curl, CURLOPT_MAIL_FROM, app->email_user);
    recipients = curl_slist_append(recipients, app->email_to);
    curl_easy_setopt(curl, CURLOPT_MAIL_RCPT, recipients);

    curl_easy_setopt(curl, CURLOPT_READFUNCTION, read_data);
    curl_easy_setopt(curl, CURLOPT_READDATA, app);
    curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);

    if (app->verbose)
        curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);

    CURL_CALL(curl_easy_perform(curl), cleanup);

    unsigned response_code;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);
    ASSERT_INT(response_code, <, 200, cleanup);
    ASSERT_INT(response_code, >=, 300, cleanup);
    res = 0;

cleanup:
    if (recipients)
        curl_slist_free_all(recipients);

    if (curl)
        curl_easy_cleanup(curl);

    return res;
}
