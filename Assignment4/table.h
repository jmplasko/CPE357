#ifndef TABLE_H_INCLUDED
#define TABLE_H_INCLUDED
#include "create.h"

void print_table(int tarfile, int s);
void print_table_find(int tarfile, char list_of_files[][256], int arr, int s);
void print_table_v(int tarfile, int amount_v, int s);
void print_table_find_v(int tarfile, int amount_v,
  char list_of_files[][256], int arr, int s);

#endif
