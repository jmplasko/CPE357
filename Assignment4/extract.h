#ifndef EXTRACT_H_INCLUDED
#define EXTRACT_H_INCLUDED
#include "table.h"

void extract(int tarfile, int v_flags, int s_flag);
void extract_find(int tarfile, int v_flags,
  char list_of_files[][256], int len, int s_flag);

#endif