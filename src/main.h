#ifndef main_h
#define main_h

#define APP_NAME "ip-nitificator"
#define HELP "--help"
#define VERBOSE "-d"
#define VERBOSE_DEF 0
#define EMAIL_USER "-u"
#define EMAIL_USER_DEF "user"
#define EMAIL_SECRET "-p"
#define EMAIL_SECRET_DEF "password"
#define EMAIL_SMTP "-s"
#define EMAIL_SMTP_DEF "smtps://smtp.gmail.com"
#define EMAIL_TO "-t"
#define EMAIL_TO_DEF ""

#define MAX_STRING 256
#define MAX_DATA 1024

#define TMP_FILE "/ip-notificator.txt"
#define TMPVAR1 "TMPDIR"
#define TMPVAR2 "TEMP"
#define TMPVAR3 "TMP"
#define TMPDEF "/var/tmp"

#include <sysexits.h>  // exit codes
#include <signal.h>    // SIGUSR1
#include <stdio.h>     // fprintf
#include <errno.h>     // error codes
#include <string.h>    // memcpy
#include <time.h>      // time_t
#include <uuid/uuid.h> // uuid


#define GET_3RD_ARG(arg1, arg2, arg3, ...) arg3

#define ASSERT_INT(value, condition, expectation, error) \
{ \
    if (value condition expectation) { \
        fprintf(stderr, "ERROR: assert "#value"(%d) "#condition" "#expectation"(%d)\n%s:%d - %s\n", \
            value, expectation, __FILE__, __LINE__, __FUNCTION__); \
        goto error; \
    } \
}

#define ASSERT_LNG(value, condition, expectation, error) \
{ \
    if (value condition expectation) { \
        fprintf(stderr, "ERROR: assert "#value"(%ld) "#condition" "#expectation"(%ld)\n%s:%d - %s\n", \
            value, (long int)expectation, __FILE__, __LINE__, __FUNCTION__); \
        goto error; \
    } \
}

#define ASSERT_PTR(value, condition, expectation, error) \
{ \
    if (value condition expectation) { \
        fprintf(stderr, "ERROR: assert "#value"(%p) "#condition" "#expectation"(%p)\n%s:%d - %s\n", \
            value, expectation, __FILE__, __LINE__, __FUNCTION__); \
        goto error; \
    } \
}

#define DEBUG_INT(text, value) \
{ \
    fprintf(stderr, "INFO: %s, "#text": %d\n", \
        __FUNCTION__, \
        value); \
}

#define DEBUG_POINTER(text, value) \
{ \
    fprintf(stderr, "INFO: %s, "#text": %p\n", \
        __FUNCTION__, \
        value); \
}

#define DEBUG_STR(text, value) \
{ \
    fprintf(stderr, "INFO: %s, "#text": %s\n", \
        __FUNCTION__, \
        value); \
}

#define CALL_MESSAGE(call, res) \
{ \
    fprintf(stderr, "ERROR: "#call" returned error: %s (%d)\n%s:%d - %s\n", \
        strerror(res), res, __FILE__, __LINE__, __FUNCTION__); \
}

#define CALL_2(call, error) \
{ \
    int res = call; \
    if (res) { \
        CALL_MESSAGE(call, res); \
        goto error; \
    } \
}

#define CALL_1(call) \
{ \
    int res = call; \
    if (res) { \
        CALL_MESSAGE(call, res); \
    } \
}

#define CALL_X(...) GET_3RD_ARG(__VA_ARGS__, CALL_2, CALL_1, )

#define CALL(...) CALL_X(__VA_ARGS__)(__VA_ARGS__)


struct app_state_t {
    int verbose;

    char ip[MAX_STRING];
    char file_path[MAX_STRING];
    char file_data[MAX_STRING];
    const char *email_user;
    const char *email_secret;
    const char *email_smtp;
    const char *email_to;
    size_t email_read;
    char buffer[MAX_STRING];
    char email_data[MAX_DATA];
};

#endif // main_h