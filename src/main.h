#ifndef main_h
#define main_h

#define APP_NAME "ip-nitificator\0"
#define HELP "--help"
#define VERBOSE "-d"
#define VERBOSE_DEF 1

#define MAX_STRING 256

#include <sysexits.h>// exit codes
#include <signal.h>  // SIGUSR1
#include <stdio.h>   // fprintf


typedef struct {
    char ip[MAX_STRING];
} app_state_t;

#endif // main_h