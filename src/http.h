#ifndef http_h
#define http_h

#define CURL_CALL_2(call, error) \
{ \
    int res = call; \
    if (res != CURLE_OK) { \
        fprintf(stderr, "ERROR: "#call" returned error: %s(%d)\n%s:%d - %s\n", \
            curl_easy_strerror(res), res, __FILE__, __LINE__, __FUNCTION__); \
        goto error; \
    } \
}

#define CURL_CALL_1(call) \
{ \
    int res = call; \
    if (res != CURLE_OK) { \
        fprintf(stderr, "ERROR: "#call" returned error: %s(%d)\n%s:%d - %s\n", \
            curl_easy_strerror(res), res, __FILE__, __LINE__, __FUNCTION__); \
    } \
}

#define CURL_CALL_X(...) GET_3RD_ARG(__VA_ARGS__, CURL_CALL_2, CURL_CALL_1, )

#define CURL_CALL(...) CURL_CALL_X(__VA_ARGS__)(__VA_ARGS__)


int http_init();
void http_cleanup();

int http_get_ip(struct app_state_t *app);
int http_send_email(struct app_state_t *app);

#endif // http_h