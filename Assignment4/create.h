#ifndef CREATE_H_INCLUDED
#define CREATE_H_INCLUDED
#include "Functions.h"

int findtype(struct stat file);
void create_header(char *pathname, int tarfile, int offset, int v, int s);

#endif
