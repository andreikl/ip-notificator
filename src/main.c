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

static int main_function()
{
    return EX_OK;
}

int main(int argc, char** argv)
{
    int exit_code = EX_SOFTWARE;
    signal(SIGINT, signal_handler);

    h = kh_init(map_str);
    utils_parse_args(argc, argv);

    unsigned k = kh_get(map_str, h, HELP);
    if (k != kh_end(h)) {
        utils_print_help();
    }
    else {
        exit_code = main_function();
    }

    kh_destroy(map_str, h);
    return exit_code;
}
