#ifndef NCODE_H_INCLUDED
#define NCODE_H_INCLUDED
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdint.h>
#include <inttypes.h>

typedef struct parentToHuff{
	unsigned int total_of_children;
	unsigned int amount_char;
	struct parentToHuff *next;
	struct parentToHuff *leftchild;
	struct parentToHuff *rightchild;
	unsigned char hold_ascii;
} parentToHuff;

void *safe_malloc(size_t size);
int cmpfunc(const void* a, const void* b);
parentToHuff* sorting_alg(parentToHuff* new_huff, parentToHuff* old_list);
parentToHuff* create_tree(parentToHuff* arr);
unsigned int find_code
(unsigned char findthis, parentToHuff* tree, char string[]);
char *strrev(char *str);
int cmpnew(const void* a, const void* b);

#endif

