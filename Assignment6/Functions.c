#include "Functions.h"

void sigintHandler(int sig_num){
  signal(SIGINT, sigintHandler);
  printf("\n");
  fflush(stdout);
} 
/* Prints out the stage for one argument */
void print_stage_one_item(char* line){
  printf("\n--------\n");
  printf("Stage 0: ");
  printf("\"%s\"\n", line);
  printf("--------\n");
  printf("     input: original stdin\n    output: original stdout\n");
  printf("      argc: 1\n");
  printf("      argv: \"%s\"\n", line);
}
/* If we only have Stage: 0, this is used to print out */
void print_single_stage(char arguments[256][LINEMAX+1], char* line, int items){
  int i;
  
  printf("\n--------\n");
  printf("Stage 0: ");
  printf("\"%s\"\n", line);
  printf("--------\n");
  printf("     input: original stdin\n    output: original stdout\n");
  printf("      argc: %d\n", items);
  printf("      argv: ");
  
  for (i = 0; i < items - 1; i++){
    printf("\"%s\",", arguments[i]);
  }
  printf("\"%s\"\n", arguments[items-1]);
}
/* Used to print out the stages, if multiple are present */
void print_stage(int stage, char *current_argv_line, char *stage_input, 
  char *stage_output, int argvc, char current_argv[256][LINEMAX+1]){
  int i;
  
  printf("\n--------\n");
  printf("Stage %d: ", stage);
  printf("\"%s\"\n", current_argv_line);
  printf("--------\n");
  printf("     input: %s\n    output: %s\n", stage_input, stage_output);
  printf("      argc: %d\n", argvc);
  printf("      argv: ");
  
  for (i = 0; i < argvc - 1; i++){
    printf("\"%s\",", current_argv[i]);
  }
  printf("\"%s\"\n", current_argv[argvc-1]);
}

/* Slices our input string into tokens, stores the tokens in an array
  and returns an int representing the number of tokens found */
int tokenize(char arguments[256][LINEMAX+1], char *input){
  /* input/output flags to indicate the next token is an input/output */
  int next_is_input = 0, next_is_output = 0;
  /* flag to indicate the previous token was a pipe */
  int previous_is_pipe = 0;
  /* Flags to indicate an input and output was set or
  if we're in a pipe, used for error handling */
  int input_f = 0, output_f = 0, pipe_f = 0;
  /* num of tokens, args in command, and args in pipe; respectively */
  int items = 0, arginc = 0, argcpipe = 1;
  /* flag used to output correct command to stderr */
  int cmd_f = 0;
  /* Stores our input string, to be tokenized*/
  char line[LINEMAX+1];
  /* Token */
  char *token;
  /* Stores the command string displayed in error messages*/
  char cmd[LINEMAX+1];
  
  /* Tokenize our input */
  strcpy(line, input); /* strtok is destructive so we make a copy */
  token = strtok(line, " "); /* Split by spaces */
  
  /* While we still have tokens */
  while ( token != NULL ){
    /* Add our token to our array */
    strcpy(arguments[items++], token);
    
    /* Sets the cmd string for error handling purposes for stage 0*/
    if ( arginc == 0 && cmd_f == 0)
      strcpy(cmd, token);
    
    /* If this token is going to be processed as an input */
    if ( next_is_input == 1 ){
      /* Error handling */
      if ( ( token[0] == '<' )  || ( token[0] == '>' )
        || ( token[0] == '|' ) || ( input_f == 1) ){
        fprintf(stderr, "%s: bad input redirecion\n", cmd);
        return 0;
      }
      else{
        /* Correctly processed, so we... */
        next_is_input = 0; /* Indicate that the next token isn't an input */
        input_f = 1; /* Set our input flag */
        arginc--; /* Decrement arginc as we do not count this as an argument */
      }
    }
    /* If this token is going to be processed as an output */
    if ( next_is_output == 1 ){
      /* Error Handling */
      if ( token[0] == '<' ){
        fprintf(stderr, "%s: bad input redirecion\n", cmd);
        return 0;
      }
      else if ( ( token[0] == '>' ) || ( token[0] == '|' ) || output_f == 1){
        fprintf(stderr, "%s: bad output redirecion\n", cmd);
        return 0;
      }
      else{
        /* Correctly processed so we... */
        next_is_output = 0; /* Indicate that the next token isn't an output */
        output_f = 1; /* Set our output flag */
        arginc--; /* Decrement arginc as we do not count this as an argument */
      }
    }
    
    /* Our previous token was a pipe! */
    if ( previous_is_pipe == 1 ){
      /* Error handling */
      strcpy(cmd, token); /* Reset our cmd string for error handling */
      if ( token[0] == '<' ){
        fprintf(stderr, "%s: bad input redirecion\n", cmd);
        return 0;
      }
      else if ( token[0] == '>' ){
        fprintf(stderr, "%s: bad output redirecion\n", cmd);
        return 0;
      }
      else if ( token[0] == '|' ){
        fprintf(stderr, "invalid null command\n");
        return 0;
      }
      
      else{
        /* Correctly processed so we... */
        previous_is_pipe = 0; /* Indicate that current token isn't an pipe */
        pipe_f = 1; /* Indicate that we are now in a pipe */
        arginc = 0; /* Reset our arguments in command counter */
      }
    }
    /* If our current token is a < */
    if ( token[0] == '<' ){
      /*Error handling */
      if ( pipe_f == 1 ){
        fprintf(stderr, "%s: ambiguous input\n", cmd);
        exit(7);
      }
      next_is_input = 1; /* Indicate the next token is an input */
      cmd_f = 1; /* Set cmd_f for error handling purposes */
    }
    /* If our current token is a > */
    else if ( token[0] == '>' ){
      next_is_output = 1; /* Indicate the next token is an output */
      cmd_f = 1; /* Set cmd_f for error handling purposes */
    }
    /* If our current token is a | */
    else if ( token[0] == '|' ){
      /* Error handling */
      if ( output_f == 1 ){
        fprintf(stderr, "%s: ambiguous output\n", cmd);
        return 0;
      }
      argcpipe++; /*Increment arguments in pipe counter */
      previous_is_pipe = 1; /*Indicate that our previous token was a pipe */
    }
    /* Our token is a string */
    else{
      arginc++; /*Increment arguments in command */
      cmd_f = 1; /* Set cmd_f for error handling purposes */
    }
    
    /* If arguments in command exceed the max, exit */
    if ( arginc > ARGUMENTSMAX ){
      fprintf(stderr, "%s: too many arguments\n", cmd);
      return 0;
    }
    
    /* If arguments in pipe exceed the max, exit */
    if ( argcpipe > PIPELINEMAX ){
      fprintf(stderr, "pipeline too deep\n");
      return 0;
    }
    /* Get our next token */
    token = strtok(NULL, " ");
  }
  
  /*At the end, all our flags should be 0, otherwise we have an error */
  if ( next_is_input == 1 ){
    fprintf(stderr, "%s: bad input redirecion\n", cmd);
    return 0;
  }
  
  else if ( next_is_output == 1 ){
    fprintf(stderr, "%s: bad output redirecion\n", cmd);
    return 0;
  }
  
  else if ( previous_is_pipe == 1 ){
    fprintf(stderr, "invalid null command\n");
    return 0;
  }
  
  /* Return tne number of tokens processed */
  return items;
}

void line_parser(char arguments[256][LINEMAX+1], int items){
  char stage_input[LINEMAX+1] = {0}; /* Used for printing purposes */
  char stage_output[LINEMAX+1] = {0}; /* Used for printing purposes */
  char current_argv[256][LINEMAX+1]; /* Used for printing purposes */
  char current_argv_line[LINEMAX+1] = {0}; /* Used for printing purposes */
  /* Flags to indicate when to change inputs/outputs and stage */
  int input_flag = 0, output_flag = 0, pipe_flag = 0;
  int stage = 0; /* Used for printing purposes */
  int argvc = 0; /* Used for printing purposes */
  int i; /* Used to iterate through our line input array */
  
  /* Set our strcpy to the default messages */
  strcpy(stage_input, "original stdin");
  strcpy(stage_output, "original stdout");
  /* Error handling for incorrect line inputs 
    was already done in tokenize function */
    
  /* Iterate through our line input array */
  for ( i = 0; i < items; i++){
    /* While we are still in the current stage */
    if (pipe_flag != 1){
      if ( input_flag == 1 ){
        /* Set our stage input */
        strcpy(stage_input, arguments[i]);
        /* Reset our input flag */
        input_flag = 0;
        /* Add our input to our current line */
        strcat(current_argv_line, arguments[i]);
        strcat(current_argv_line, " ");
        continue;
      }
      
      else if ( output_flag == 1 ){
        /* Set our stage output */
        strcpy(stage_output, arguments[i]);
        /* Reset our output flag */
        output_flag = 0;
        /* Add our output to our current line */
        strcat(current_argv_line, arguments[i]);
        strcat(current_argv_line, " ");
        continue;
      }
      
      if ( arguments[i][0] == '<' ){
        /* Set our input flag */
        input_flag = 1;
        /* Add the input character to our current line */
        strcat(current_argv_line, arguments[i]);
        strcat(current_argv_line, " ");
      }
      else if ( arguments[i][0] == '>' ){
        /* Set our output flag */
        output_flag = 1;
        /* Add the output character to our current line */
        strcat(current_argv_line, arguments[i]);
        strcat(current_argv_line, " ");
      }
      
      else if ( arguments[i][0] == '|' ){
        /* Set our pipe flag */
        pipe_flag = 1;
      }
      
      else{
        /* Add our command to the current line */
        strcpy(current_argv[argvc], arguments[i]);
        argvc++;
        if ( current_argv_line == NULL ){
          strcpy(current_argv_line, arguments[i]);
          strcat(current_argv_line, " ");
        }
        else{
          strcat(current_argv_line, arguments[i]);
          strcat(current_argv_line, " ");
        }
      }
    }
    /* The stage number has changed*/
    else{
      /*Print current stage*/
      memset(stage_output, '\0', LINEMAX+1);
      sprintf(stage_output, "pipe to stage %d", stage + 1);
      print_stage(stage, current_argv_line, stage_input, 
        stage_output, argvc, current_argv);
      memset(stage_input, '\0', LINEMAX+1);
      sprintf(stage_input, "pipe from stage %d", stage);
      strcpy(stage_output, "original stdout");
      stage++;
      pipe_flag = 0;
      /* Reset our current arguments array */
      while (argvc > 0){
        memset(current_argv[argvc--], '\0', LINEMAX+1);
      }
      /* Set our array to the current command values */
      strcpy(current_argv[argvc], arguments[i]);
      /* Increment our arguments in stage counter*/
      argvc++;
      /* Add our command to the current line */
      memset(current_argv_line, '\0', LINEMAX+1);
      strcpy(current_argv_line, arguments[i]);
      strcat(current_argv_line, " ");
      /* If we're not at the end, continue */
      if (i + 1 != items )
        continue;
    }
  }
  /* Print our last stage, or stage 0 depending on the conditions */
  print_stage(stage, current_argv_line, stage_input, 
    stage_output, argvc, current_argv);
}

int command_checker(char arguments[256][256], int items){
  char directorypath[LINEMAX+3] = {0};
  
  if (strcmp(arguments[0], "cd") == 0){
    if ( items != 2 ){
      if ( 2 < items )
        fprintf(stderr, "%s: too many arguments.\n", arguments[0]);
      if ( items == 1 )
        fprintf(stderr, "%s: missing argument.\n", arguments[0]);
    }
    
    else{
      strcpy(directorypath, "./");
      strcat(directorypath, arguments[1]);
      if (chdir(directorypath) != 0){
        perror(arguments[0]);
      }
      return 1;
    }
  }
  
  else if(strcmp(arguments[0], "exit") == 0){
    return 0;
  }
  
  return tryit(arguments, items);
}

/*int tryit(char arguments[256][256], int items){
  pid_t child;
  int status;
  int i;
  struct sigaction action;
  action.sa_handler = sigintHandler;
  action.sa_flags = SA_RESETHAND;
  
  
  if ( ( child = fork() ) ){
    if ( child == -1 ){
      perror("fork");
      exit(EXIT_FAILURE);
    }
    wait(&status);
    if ( WIFEXITED(status) && (WEXITSTATUS(status) == 0) && !WIFSIGNALED(status)){
      printf("Process %d succeeded.\n", child);
	  errno = 0;
      return 1;
    }
    else{
      printf("Process %d exited with an error value.\n", child);
	  errno = 0;
      return 1;
    }
  }
  else{
	if(sigaction(SIGINT, &action, NULL) == -1)
		perror("sigaction");
    if (execlp(arguments[0], arguments[0], arguments[1], NULL) == -1){
      perror(arguments[0]);
      
      exit(4);
    }
  }
  return 1;  
}*/

int tryit(char arguments[256][256], int items){
	pid_t chi[10] = {0};
	pid_t child = 0;
	int status =0; 
	int i = 0;
	int j = 0;
	int trackerarg = 0;
	int pp[10][2];
	int k = 0;
	int r = 0;
	int pipes = 0;
	int left = 0;
	int right = 0;
	int chit = 0;
	char holdargv[256][256];
	struct sigaction action;
	action.sa_handler = sigintHandler;
	action.sa_flags = SA_RESETHAND;
	
	char redirecting[256][256];
	char out_filename[512] = {0};
	char in_filename[512] = {0};
	int input_flag = 0, output_flag = 0;
	int input_stage = 0, output_stage = 0;
	int input_fd, output_fd;
	int redir = 0;
	
	for(i = 0; i < 256; i++){
	  for(j = 0; j < 256; j++){
		  holdargv[i][j] = '\0';
		  redirecting[i][j] = '\0';
	  }
	}
	
	i = 0;
	j = 0;
	
	/*for(i = 0; i < items; i++){
		if(strcmp(arguments[i], "<") == 0){
			if(strcmp(arguments[i+1], ">") == 0){
				gf = i+2;
				for(gf = 0; gf < items; gf++){
					if(strcmp(arguments[gf], '|') == 0){
							gf = items + 1;
					}else{
						holdargv[trackerarg] = malloc(strlen(arguments[i]));
						strcpy(holdargv[trackerarg], arguments[i]);
						tracker++;
					}
				}
			}else{
				arguement
			}
		}
	}
	
	for(i = 0; i < items; i++){
		if(strcmp(arguments[i], ">") == 0){
			if(strcmp(arguments[i+1], "<") == 0){
				
			}else{
				
			}
		}
	}*/
	
	/*for(i = 0; i < items; i++){
		if(strcmp(arguments[i], "|") == 0){
			pipes++;
		}
	}*/
	
	for(i = 0; i < items; i++){
		if (right == 1){
		  stpcpy(in_filename, arguments[i]);
		  input_flag = 1;
		  input_stage = pipes;
		  right = 0;
		}
		if (left == 1){
		  stpcpy(out_filename, arguments[i]);
		  output_flag = 1;
		  output_stage = pipes;
		  left = 0;
		}
		if(strcmp(arguments[i], "<") == 0)
				right = 1;
		else if(strcmp(arguments[i], ">") == 0)
				left = 1;
		else if(strcmp(arguments[i], "|") == 0)
		  pipes++;
		else{
		  strcpy(redirecting[i], arguments[i]);
		  redir++;
		}
	}

	
	/*if((pipes == 0) && (left == 0) && (right == 0)){
		if ( ( child = fork() ) ){
			if ( child == -1 ){
			  perror("fork");
			  exit(EXIT_FAILURE);
			}
			wait(&status);
			if ( WIFEXITED(status) && (WEXITSTATUS(status) == 0)){
			  printf("Process %d succeeded.\n", child);
			  for(i = 0; i < items; i++)
				  arguments[i][0] = '\0';
			  errno = 0;
			  return 1;
			}
			else{
			  printf("Process %d exited with an error value.\n", child);
			  for(i = 0; i < items; i++)
				  arguments[i][0] = '\0';
			  errno = 0;
			  return 1;
			}
		 }else{
			if(sigaction(SIGINT, &action, NULL) == -1)
				perror("sigaction");
			if(items == 1){
				if(execlp(arguments[0], arguments[0], NULL) == -1){
					perror(arguments[0]);
					exit(4);
				}
			}
			if(items == 2){
				if(execlp(arguments[0], arguments[0],
					arguments[1], NULL) == -1){
					perror(arguments[0]);
					exit(4);
				}
			}
			if(items == 3){
				if(execlp(arguments[0], arguments[0],
					arguments[1], arguments[2], NULL) == -1){
					perror(arguments[0]);
					exit(4);
									}
			}
			if(items == 4){
				if(execlp(arguments[0], arguments[0],
					arguments[1], arguments[2], arguments[3],
					NULL) == -1){
					perror(arguments[0]);
					exit(4);
									}
			}
			if(items == 5){
				if(execlp(arguments[0], arguments[0],
					arguments[1], arguments[2], arguments[3],
					arguments[4], NULL) == -1){
					perror(arguments[0]);
					exit(4);
									}
			}
			if(items == 6){
				if(execlp(arguments[0], arguments[0],
					arguments[1], arguments[2], arguments[3],
					arguments[4], arguments[5], NULL) == -1){
					perror(arguments[0]);
					exit(4);
									}
			}
			if(items == 7){
				if(execlp(arguments[0], arguments[0],
					arguments[1], arguments[2], arguments[3],
					arguments[4], arguments[5], arguments[6],
					NULL) == -1){
					perror(arguments[0]);
					exit(4);
									}
			}
			if(items == 8){
				if(execlp(arguments[0], arguments[0],
					arguments[1], arguments[2], arguments[3],
					arguments[4], arguments[5], arguments[6],
					arguments[7],NULL) == -1){
					perror(arguments[0]);
					exit(4);
									}
			}
			if(items == 9){
				if(execlp(arguments[0], arguments[0],
					arguments[1], arguments[2], arguments[3],
					arguments[4], arguments[5], arguments[6],
					arguments[7], arguments[8], NULL) == -1){
					perror(arguments[0]);
					exit(4);
									}
			}
			if(items == 10){
				if(execlp(arguments[0], arguments[0],
					arguments[1], arguments[2], arguments[3],
					arguments[4], arguments[5], arguments[6],
					arguments[7], arguments[8], arguments[9], 
					NULL) == -1){
					perror(arguments[0]);
					exit(4);
									}
			}
		}
		return 1;
	}*/
	
	if((pipes == 0) && (left == 0) && (right == 0)){
		if ( ( child = fork() ) ){
			if ( child == -1 ){
			  perror("fork");
			  exit(EXIT_FAILURE);
			}
       /*wait(&status);*/
      if ( (-1) == waitpid(child, &status, 0) ) {  /*wait for one child*/
        /*kill(child, SIGINT);*/
        errno = 1;
		exit(0);
      }
	  
	  
	  
	  for(i = 0; i <= redir; i++){
        redirecting[i][0] = '\0';
      }
      redir = 0;
      errno = 0;
	  
      if(output_flag == 1){
        close(output_fd);
		for(i = 0; i < items; i++)
				  out_filename[i] = '\0';
      }
      if (input_flag == 1){
        close(input_fd);
        for(i = 0; i < items; i++)
				  out_filename[i] = '\0';
      }
	 
		if ( WIFEXITED(status) && (WEXITSTATUS(status) == 0) && !WIFSIGNALED(status)){
			  /*printf("Process %d succeeded.\n", child);*/
			  
			for (i = 0; i < items; i++){
				arguments[i][0] = '\0';
			}
			return 1;
		}
		else{
			
			
			for (i = 0; i < items; i++){
				arguments[i][0] = '\0';
			}
			printf("Process %d exited with an error value.\n", child);
			fflush(stdout);
			return 1;
		}
		
		}else{
      if (sigaction(SIGINT, &action, NULL) == -1){
        perror("Failed to install signal handler for SIGINT");
      }
      
      if (output_flag == 1){
        if ((output_fd = open(out_filename, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR))<0){
          perror(out_filename);
          exit(3);
        }
        dup2(output_fd, STDOUT_FILENO);
      }
      if (input_flag == 1){
        if ((input_fd = open(in_filename, O_RDONLY, S_IRUSR | S_IROTH)) < 0){
          perror(in_filename);
          exit(3);
        }
        dup2(input_fd, STDIN_FILENO);
      }
      
      
			
			if(redir == 1){
				if(execlp(redirecting[0], redirecting[0], NULL) == -1){
					perror(redirecting[0]);
					exit(4);
				}
			}
			if(redir == 2){
				if(execlp(redirecting[0], redirecting[0],
					redirecting[1], NULL) == -1){
					perror(redirecting[0]);
					exit(4);
				}
			}
			if(redir == 3){
				if(execlp(redirecting[0], redirecting[0],
					redirecting[1], redirecting[2], NULL) == -1){
					perror(redirecting[0]);
					exit(4);
									}
			}
			if(redir == 4){
				if(execlp(redirecting[0], redirecting[0],
					redirecting[1], redirecting[2], redirecting[3],
					NULL) == -1){
					perror(redirecting[0]);
					exit(4);
									}
			}
			if(redir == 5){
				if(execlp(redirecting[0], redirecting[0],
					redirecting[1], redirecting[2], redirecting[3],
					redirecting[4], NULL) == -1){
					perror(redirecting[0]);
					exit(4);
									}
			}
			if(redir == 6){
				if(execlp(redirecting[0], redirecting[0],
					redirecting[1], redirecting[2], redirecting[3],
					redirecting[4], redirecting[5], NULL) == -1){
					perror(redirecting[0]);
					exit(4);
									}
			}
			if(redir == 7){
				if(execlp(redirecting[0], redirecting[0],
					redirecting[1], redirecting[2], redirecting[3],
					redirecting[4], redirecting[5], redirecting[6],
					NULL) == -1){
					perror(redirecting[0]);
					exit(4);
									}
			}
			if(redir == 8){
				if(execlp(redirecting[0], redirecting[0],
					redirecting[1], redirecting[2], redirecting[3],
					redirecting[4], redirecting[5], redirecting[6],
					redirecting[7],NULL) == -1){
					perror(redirecting[0]);
					exit(4);
									}
			}
			if(redir == 9){
				if(execlp(redirecting[0], redirecting[0],
					redirecting[1], redirecting[2], redirecting[3],
					redirecting[4], redirecting[5], redirecting[6],
					redirecting[7], redirecting[8], NULL) == -1){
					perror(redirecting[0]);
					exit(4);
									}
			}
			if(redir == 10){
				if(execlp(redirecting[0], redirecting[0],
					redirecting[1], redirecting[2], redirecting[3],
					redirecting[4], redirecting[5], redirecting[6],
					redirecting[7], redirecting[8], redirecting[9], 
					NULL) == -1){
					perror(redirecting[0]);
					exit(4);
									}
			}
		
		}
		return 1;
	}

	for(i = 0; i < pipes; i++){
		if(pipe(pp[i])){
			perror("pipe");
			exit(3);
		}
	}
	
	for(i = 0; i < (items+1); i++){
		if((strcmp(arguments[i], "|") == 0) || arguments[i][0] == '\0'){
			if((chi[chit] = fork()) == 0){
				/*child*/
				if(r == 0){
					dup2(pp[chit][1], STDOUT_FILENO);
				}else if (r == pipes){
					dup2(pp[chit-1][0], STDIN_FILENO);
				}else{
					dup2(pp[chit-1][0], STDIN_FILENO);
					dup2(pp[chit][1], STDOUT_FILENO);
				}
		
				for(j = 0; j < pipes; j++){
					close(pp[j][0]);
					close(pp[j][1]);
				}

				/*if(trackerarg == 1){
					if(sigaction(SIGINT, &action, NULL) == -1)
					perror("sigaction");
					if(execlp(holdargv[0],holdargv[0], NULL) == -1){
						perror(holdargv[0]);
						exit(3);
					}
				}else{
					if(sigaction(SIGINT, &action, NULL) == -1)
					perror("sigaction");
					if(execvp(holdargv[0],holdargv) == -1){
						perror(holdargv[0]);
						exit(3);
					}
				}*/
					if(sigaction(SIGINT, &action, NULL) == -1)
						perror("sigaction");
					if(trackerarg == 1){
						if(execlp(holdargv[0], holdargv[0], NULL) == -1){
							perror(holdargv[0]);
							exit(4);
						}
					}
					if(trackerarg  == 2){
						if(execlp(holdargv[0], holdargv[0],
							holdargv[1], NULL) == -1){
							perror(holdargv[0]);
							exit(4);
						}
					}
					if(trackerarg  == 3){
						if(execlp(holdargv[0], holdargv[0],
							holdargv[1], holdargv[2], NULL) == -1){
							perror(holdargv[0]);
							exit(4);
											}
					}
					if(trackerarg  == 4){
						if(execlp(holdargv[0], holdargv[0],
							holdargv[1], holdargv[2], holdargv[3],
							NULL) == -1){
							perror(holdargv[0]);
							exit(4);
											}
					}
					if(trackerarg  == 5){
						if(execlp(holdargv[0], holdargv[0],
							holdargv[1], holdargv[2], holdargv[3],
							holdargv[4], NULL) == -1){
							perror(holdargv[0]);
							exit(4);
											}
					}
					if(trackerarg  == 6){
						if(execlp(holdargv[0], holdargv[0],
							holdargv[1], holdargv[2], holdargv[3],
							holdargv[4], holdargv[5], NULL) == -1){
							perror(holdargv[0]);
							exit(4);
						}
					}
					if(trackerarg  == 7){
						if(execlp(holdargv[0], holdargv[0],
							holdargv[1], holdargv[2], holdargv[3],
							holdargv[4], holdargv[5], holdargv[6],
							NULL) == -1){
							perror(holdargv[0]);
							exit(4);
						}
					}
					if(trackerarg  == 8){
						if(execlp(holdargv[0], holdargv[0],
							holdargv[1], holdargv[2], holdargv[3],
							holdargv[4], holdargv[5], holdargv[6],
							holdargv[7],NULL) == -1){
							perror(holdargv[0]);
							exit(4);
						}
					}
					if(trackerarg  == 9){
						if(execlp(holdargv[0], holdargv[0],
							holdargv[1], holdargv[2], holdargv[3],
							holdargv[4], holdargv[5], holdargv[6],
							holdargv[7], holdargv[8], NULL) == -1){
							perror(holdargv[0]);
							exit(4);
						}
					}
					if(trackerarg  == 10){
						if(execlp(holdargv[0], holdargv[0],
							holdargv[1], holdargv[2], holdargv[3],
							holdargv[4], holdargv[5], holdargv[6],
							holdargv[7], holdargv[8], holdargv[9], 
							NULL) == -1){
							perror(holdargv[0]);
							exit(4);
						}
					}
			}
			
			r++;
			chit++;
			
			for(k = 0; k < 256; k++){
				for(j = 0; j < 256; j++){
					holdargv[k][j] = '\0';
				}
			}
	
			k = 0;
			j = 0;
			
			trackerarg = 0;
			
		}else{
			if(chi[chit] == (-1)){
				perror("fork");
				exit(EXIT_FAILURE);
			}
			
			/*holdargv[trackerarg] = malloc(strlen(arguments[i]) + 1);*/
			strcpy(holdargv[trackerarg], arguments[i]);
			trackerarg++;
		}
	}
	
	/*parent*/
	for(i = 0; i < pipes; i++){
		close(pp[i][0]);
		close(pp[i][1]);
	}
	
	for(i = 0; i < (pipes+1); i++){
		if ( (-1) == waitpid(chi[i], &status, 0) ) {  /*wait for one child*/
			perror("wait");
		}
	}
	
	for(i = 0; i < (pipes+1); i++){
		if(!WIFEXITED(status) && (WEXITSTATUS(status) == 0)){
			printf("Process %d exited with an error value.\n", chi[i]);
			
			errno = 0;
			
		}else{
			printf("Process %d succeeded.\n", chi[i]);
			
			errno = 0;
			
		}
	} 
	
	return 1; /*should never get here*/
}


int prompt_user(){
  /* Array to hold our input line
  3 larger to account for newline character,
  null character from fgets, and error handling purposes */
  char input[LINEMAX+3] = {0};
  /* Will eventually hold our tokens */
  char arguments[256][LINEMAX+1];
  /* Our number of tokens and the index of the newline character */
  int newline_index;
  int items = 0;
  
  int i, j;
  
  /* Get our input from the user */
  printf("8-P ");
  fgets(input, 515, stdin);
  
  /* If our string is over LINEMAX, EXIT*/
  if ( input[LINEMAX+1] != '\0' ){
    fprintf(stderr, "command too long\n");
    exit(1);
  }
  
  /* Removes newline character from our input */
  newline_index = (int) strlen(input) - 1;
  input[newline_index] = '\0';
  
  /*Tokenize our input and acquire the number of items inputted*/
  items = tokenize(arguments, input); 
  
  /* No Input Case */
  if (items == 0){
    return 1; /*Empty Input */
  }
  /* Execute our input */
  
  char* passed_in[256] = { NULL };
  char paassed_in[256][256];
  for(i = 0; i < 256; i++){
	  for(j = 0; j < 256; j++){
		  paassed_in[i][j] = '\0';
	  }
  }
  
  for ( i = 0; i < items; i++){
    /*passed_in[i] = malloc(strlen(arguments[i]) + 1);*/
	strcpy(paassed_in[i], arguments[i]);
    /*strcpy(passed_in[i], arguments[i]);*/
  }
  
  
  
  return command_checker(paassed_in, items);
}

void run_mush(){
  int status;
  do{
	signal(SIGINT, sigintHandler);
    status = prompt_user();
  }while(status);
}