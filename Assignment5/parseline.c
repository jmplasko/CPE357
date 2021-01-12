#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#define LINEMAX 512

void print_stage_one_item(char* line, char *input, char *output){
  printf("\n--------\n");
  printf("Stage 0: ");
  printf("\"%s\"\n", line);
  printf("--------\n");
  printf("     input: %s\n    output: %s\n", input, output);
  printf("      argc: 1\n");
  printf("      argv: \"%s\"\n", line);
}

int main(){
  char input[515] = {0};
  char arguments[256][LINEMAX+1];
  char line[LINEMAX+1]; /*Accounting for our NULL */
  char *token;
  
  int items = 0;
  int i, newline_index, j;
  
  /* VARIABLES FOR OUR LOOP */
  char stage_input[LINEMAX+1] = {0};
  char stage_output[LINEMAX+1] = {0};
  int input_flag = 0, output_flag = 0, pipe_flag = 0;
  int start_index = 0, end_index = 0;
  int stage = 0;
  int argc = 0;
  char arg = 0;
  
  
  printf("line: ");
  fgets(input, 515, stdin);
  
  /* If our string is over LINEMAX, EXIT*/
  if ( input[LINEMAX+1] != '\0' ){
    printf("broke\n");
    exit(1);
  }
  
  /* Removes newline character from our input */
  newline_index = (int) strlen(input) - 1;
  input[newline_index] = '\0';
  
  /*strtoken messes with the input char pointer so we copy it over */
  strcpy(line, input);
  token = strtok(line, " ");
  
  /*
  Tokenize our input*/
  while (token != NULL){
    /* Copy into our array of tokens */
    strcpy(arguments[items++], token);
    token = strtok(NULL, " ");
  }
  
  strcpy(stage_input, "original stdin");
  strcpy(stage_output, "original stdout");
  
  
  if (items == 1){
    print_stage_one_item(arguments[0], stage_input, stage_output);
    return 0;
  }
  
  /* Our BULLSHIT*/ 
  
  for( i = 0; i < items; i++){
    //printf("%s\n", arguments[i]);

	if(strlen(arguments[i]) == 1){
		arg = arguments[i][0];
	}else{
		arg = 0;
	}

	switch(arg) {
		case '<' :
			if((arguments[end_index + 1][0] == '\0' || (end_index == 0))){
				fprintf(stderr, "invalid null command\n");
				exit(3);
			}else if(output_flag == 1){
				fprintf(stderr, "%s: ambigous output\n", arguments[start_index]);
				exit(3);
			}else if(arguments[end_index + 1][0] == '|'){
				fprintf(stderr, "%s: invalid null command\n", arguments[start_index]);
				exit(3);
			}else if((arguments[end_index + 1][0] == '<') || (arguments[end_index + 1][0] == '>')){
				fprintf(stderr, "%s: bad input redirection\n", arguments[start_index]);
				exit(3);
			}else{
				for(j = 1; j < 10; j++){
					if(arguments[end_index + j][0] == '<'){
						fprintf(stderr, "%s: invalid null command\n", arguments[end_index + 1]);
						exit(3);
					}
				}
			}
			end_index++;
			start_index = end_index;
			break;
		case '>' :
			if((arguments[end_index + 1][0] == '\0') || (end_index == 0)){
				fprintf(stderr, "invalid null command\n");
				exit(3);
			}else if(output_flag == 1){
				fprintf(stderr, "%s: ambigous output\n", arguments[start_index]);
				exit(3);
			}else if(arguments[end_index + 1][0] == '|'){
				fprintf(stderr, "%s: invalid null command\n", arguments[start_index]);
				exit(3);
			}else if((arguments[end_index + 1][0] == '<') || (arguments[end_index + 1][0] == '>')){
				fprintf(stderr, "%s: bad input redirection\n", arguments[start_index]);
				exit(3);
			}else{
				for(j = 1; j < 10; j++){
					if(arguments[end_index + j][0] == '<'){
						fprintf(stderr, "%s: invalid null command\n", arguments[end_index + 1]);
						exit(3);
					}
				}
			}
			end_index++;
			start_index = end_index;
			break;
		case '|' :
			if((arguments[end_index + 1][0] == '\0') || (end_index == 0)){
				fprintf(stderr, "invalid null command\n");
				exit(3);
			}else if(output_flag == 1){
				fprintf(stderr, "%s: ambigous output\n", arguments[start_index]);
				exit(3);
			}else if(arguments[end_index + 1][0] == '|'){
				fprintf(stderr, "%s: invalid null command\n", arguments[start_index]);
				exit(3);
			}else if((arguments[end_index + 1][0] == '<') || (arguments[end_index + 1][0] == '>')){
				fprintf(stderr, "%s: bad input redirection\n", arguments[start_index]);
				exit(3);
			}else{
				for(j = 1; j < 10; j++){
					if(arguments[end_index + j][0] == '<'){
						fprintf(stderr, "%s: invalid null command\n", arguments[end_index + 1]);
						exit(3);
					}
				}
			}
			end_index++;
			start_index = end_index;
			break;
		default :
			end_index = i + 1;
	}
  }
  
  return 0;
}