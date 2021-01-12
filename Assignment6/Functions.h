#ifndef FUNCTIONS_H_INCLUDED
#define FUNCTIONS_H_INCLUDED
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <errno.h>
#include <sys/stat.h>
#include <fcntl.h>
#define LINEMAX 512
#define ARGUMENTSMAX 10
#define PIPELINEMAX 10

void print_stage_one_item(char* line);
void print_single_stage(char arguments[256][LINEMAX+1], char* line, int items);
void print_stage(int stage, char *current_argv_line, char *stage_input, 
  char *stage_output, int argvc, char current_argv[256][LINEMAX+1]);
int tokenize(char arguments[256][LINEMAX+1], char *input);
void line_parser(char arguments[256][LINEMAX+1], int items);
int command_checker(char arguments[256][256], int items);
int tryit(char arguments[256][256], int items);
int prompt_user();
void run_mush();

#endif
