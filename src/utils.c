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

KHASH_MAP_INIT_STR(map_str, char *)
extern khash_t(map_str) *h;

void utils_parse_args(int argc, char* argv[])
{
    int ret;
    unsigned k;

    for (int i = 0; i < argc; i++) {
        if (argv[i][0] == '-') {
            k = kh_put(map_str, h, argv[i], &ret);
            kh_val(h, k) = (i + 1 < argc) ? argv[i + 1] : NULL;
        }
    }
}

const char *utils_get_str(const char name[], const char def_value[])
{
    unsigned k = kh_get(map_str, h, name);
    if (k != kh_end(h)) {
        return kh_val(h, k);
    }
    return def_value;
}

int utils_get_int(const char name[], int def_value)
{
    unsigned k = kh_get(map_str, h, name);
    if (k != kh_end(h)) {
        const char* value = kh_val(h, k);
        return atoi(value);
    }
    return def_value;
}

int utils_get_time(char buffer[], int buffer_size)
{
    time_t t = time(NULL);
    struct tm *tm = localtime(&t);
    return strftime(buffer, buffer_size, "%a, %d %b %Y %H:%M:%S %z", tm);
}


int utils_get_uuid(char buffer[], int buffer_size)
{
    uuid_t binuuid;
    uuid_generate_random(binuuid);

    ASSERT_INT(buffer_size, <, 37, error);
    uuid_unparse_lower(binuuid, buffer);

    return 0;
error:
    return EAGAIN;
}

const char* file_get_tmp()
{
    const char *tmp = getenv(TMPVAR1);
    if (tmp == NULL)
        if ((tmp = getenv(TMPVAR2)) == NULL)
            tmp = getenv(TMPVAR3);

    return (tmp == NULL)? TMPDEF: tmp;
}

int file_read(const char path[], char buffer[], size_t buffer_size, size_t *read)
{
    FILE *fstream = fopen(path, "a+");
    ASSERT_PTR(fstream, ==, NULL, error)

    size_t t = fread(buffer, 1, buffer_size, fstream);
    if (t < buffer_size)
        buffer[t] = 0;
    else
        buffer[buffer_size - 1] = 0;

    if (read != NULL)
        *read = t;

    CALL(fclose(fstream), error);
    return 0;

error:
    return errno;
}

int file_write(const char path[], char buffer[], size_t buffer_size, size_t *written)
{
    FILE *fstream = fopen(path, "w");
    ASSERT_PTR(fstream, ==, NULL, error)

    size_t t = fwrite(buffer, 1, buffer_size, fstream);
    if (written != NULL)
        *written = t;

    CALL(fclose(fstream), error);
    return 0;

error:
    return errno;
}