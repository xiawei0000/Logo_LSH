#ifndef BASIC_H
#define  BASIC_H

#include <windows.h>
#include <time.h>
#include <string>

int gettimeofday(struct timeval *tp, void *tzp);
long long   atoll(const char *numstr);
void usage(char *programName);
#endif

