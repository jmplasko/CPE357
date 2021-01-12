#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>

int main(){
	pid_t child;
	int status;
	
	printf("Hello, world!\n");
	
	if((child = fork())){
		/*parent*/
		if(child == -1){
			perror("fork");
			exit(EXIT_FAILURE);
		}
		/*make sure that child has finshed*/
		wait(&status);
		
		printf("This is the parent, pid %d.\n", getpid());
		
		if(WIFEXITED(status) && (WEXITSTATUS(status) == 0)){
			printf("This is the parent, pid %d, signing off.\n", getpid());
			exit(0);
		}else{
			exit(3);
		}
	}else{
		/*child*/
		printf("This is the child, pid %d.\n", getpid());
		exit(0);
	}

	return 1; /*should never get here*/
}