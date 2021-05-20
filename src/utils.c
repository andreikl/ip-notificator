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

void utils_parse_args(int argc, char** argv)
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

const char* get_tmp_dir()
{
    const char *tmp = getenv(TMPVAR1);
    if (tmp == NULL)
        if ((tmp = getenv(TMPVAR2)) == NULL)
            tmp = getenv(TMPVAR3);

    return (tmp == NULL)? TMPDEF: tmp;
}

int read_file(const char *path, char *buffer, int buffer_size, size_t *read)
{
    FILE *fstream = fopen(path, "a+");
    ASSERT_POINTER(fstream, ==, NULL, error)

    size_t read_size = fread(buffer, 1, buffer_size, fstream);
    if (read_size < buffer_size)
        buffer[read_size] = 0;
    else
        buffer[buffer_size - 1] = 0;

    if (read != NULL)
        *read = read_size;

    CALL(fclose(fstream), error);
    return 0;

error:
    return errno;

}
