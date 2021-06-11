// IP Notificator
//
// Copyright (C) 2021 Andrei Klimchuk <andrew.klimchuk@gmail.com>
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 3 of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program; if not, write to the Free Software Foundation,
// Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

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