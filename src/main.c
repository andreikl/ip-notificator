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

#include "main.h"
#include "utils.h"

#include "khash.h"

KHASH_MAP_INIT_STR(map_str, char*)
khash_t(map_str) *h;

int is_abort = 0;

static void signal_handler(int signal_number)
{
    if (signal_number == SIGUSR1) {
        fprintf(stderr, "INFO: SIGUSR1\n"); 
    } else {
        fprintf(stderr, "INFO: Other signal %d\n", signal_number);
    }
    is_abort = 1;
}

static void print_help()
{
    printf("%s [options]\n", APP_NAME);
    printf("options:\n");
    printf("\n");
    printf("%s: help\n", HELP);
    printf("%s: verbose, default: %d\n", VERBOSE, VERBOSE_DEF);
    printf("%s: email user, default: %s\n", EMAIL_USER, EMAIL_USER_DEF);
    printf("%s: email secret, default: %s\n", EMAIL_SECRET, EMAIL_SECRET_DEF);
    printf("%s: smtp url, default: %s\n", EMAIL_SMTP, EMAIL_SMTP_DEF);
    printf("%s: email to, default: %s\n", EMAIL_TO, EMAIL_TO_DEF);
    exit(0);
}

static int main_function()
{
    struct app_state_t app;
    memset(&app, 0, sizeof(struct app_state_t));
    app.verbose = utils_get_int(VERBOSE, VERBOSE_DEF);
    app.email_user = utils_get_str(EMAIL_USER, EMAIL_USER_DEF);
    app.email_secret = utils_get_str(EMAIL_SECRET, EMAIL_SECRET_DEF);
    app.email_smtp = utils_get_str(EMAIL_SMTP, EMAIL_SMTP_DEF);
    app.email_to = utils_get_str(EMAIL_TO, EMAIL_TO_DEF);

    strcpy(app.file_path, file_get_tmp());
    strcat(app.file_path, TMP_FILE);
    CALL(http_init(), exit);
    CALL(file_read(app.file_path, app.file_data, sizeof(app.file_data), NULL), cleanup);
    CALL(http_get_ip(&app), cleanup);
    if (strcmp(app.file_data, app.ip)) {
        CALL(http_send_email(&app), cleanup);
        CALL(file_write(app.file_path, app.ip, strlen(app.ip), NULL), cleanup);
    }

    DEBUG_STR("app.ip", app.ip);
    DEBUG_STR("app.file_path", app.file_path);
    DEBUG_STR("app.file_data", app.file_data);

cleanup:
    http_cleanup();

    return EX_OK;

exit:
    return EX_SOFTWARE;
}

int main(int argc, char** argv)
{
    int exit_code = EX_SOFTWARE;
    signal(SIGINT, signal_handler);

    h = kh_init(map_str);
    utils_parse_args(argc, argv);

    unsigned k = kh_get(map_str, h, HELP);
    if (k != kh_end(h)) {
        print_help();
    }
    else {
        exit_code = main_function();
    }

    kh_destroy(map_str, h);
    return exit_code;
}
