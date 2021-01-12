#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <dirent.h>
#include <errno.h>

int main(int argc, char* argv []){

	pid_t child;
	int status;

	if(argc != 2){
		printf("usage: tryit command\n");
		exit(3);
	}
	
	if((child = fork())){
		/*parent*/
		if(child == -1){
			perror("fork");
			exit(EXIT_FAILURE);
		}
		if(-1 == wait(&status)){
			perror("wait");
			exit(3);
		}
		if(!WIFEXITED(status) || WEXITSTATUS(status)){
			printf("Process %d exited with an error value.\n", child);
			exit(3);
		}else{
			printf("Process %d succeeded.\n", child);
			exit(0);
		}
	}else{
		/*child*/
		execl(argv[1], argv[1], (char*)NULL);
		perror(argv[1]);
		exit(3);
	}
	return 1; /*should never get here*/
}