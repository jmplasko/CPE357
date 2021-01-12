#ifndef FUNCTIONS_H_INCLUDED
#define FUNCTIONS_H_INCLUDED
#include <stdio.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <limits.h>
#include <stdint.h>
#include <fcntl.h>
#include <libgen.h>
#include <pwd.h>
#include <grp.h>
#include <ctype.h>
#include <time.h>
#include <utime.h>

int insert_special_int(char *where, size_t size, int32_t val);
uint32_t extract_special_int(char *where, int len);
int make_full_path(char *path);

#endif