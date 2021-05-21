
#ifndef utils_h
#define utils_h

void utils_parse_args(int argc, char* argv[]);
const char *utils_get_str(const char name[], const char def_value[]);
int utils_get_int(const char name[], int def_value);

int utils_get_time(char buffer[], int buffer_size);
int utils_get_uuid(char buffer[], int buffer_size);

const char *file_get_tmp();
int file_read(const char path[], char buffer[], int buffer_size, int *read);
int file_write(const char path[], char buffer[], int buffer_size, int *written);

#endif //utils_h
