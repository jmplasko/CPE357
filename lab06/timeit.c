#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <sys/time.h>

void handler(int signum){
	/*prints correct statement*/
	static int n = 0;
	if(n++%2){
		printf("Tock\n");
		fflush(stdout);
	}else{
		printf("Tick...");
		fflush(stdout);
	}
}


int main(int argc, char *argv[]){
	long second = 0;
    int i, j;
	int flag = 0;
	int flagn = 0;
	char *ptr;
	struct sigaction sa;
	struct itimerval alr, old;

	/*sets the SIGALRM everyhalf second*/
	alr.it_value.tv_sec = 0;
	alr.it_value.tv_usec = 500000;
	alr.it_interval.tv_sec = 0;
	alr.it_interval.tv_usec = 500000;
	
	/*checks for right amount of arguments*/
	if((argc < 2) || (argc > 2)){
		printf("usage: ./timeit <seconds>\n");
		exit(2);
	}
	
	if(argv[1][0] == '-'){
		flagn = 1;
	}
	
	/*checks the argument to make sure it's a number*/
	char time_s[strlen(argv[1])];
	for(i = 0; i < strlen(argv[1]); i++){ 
		for(j = 0; j < 10; j++){
			if(argv[1][i] == 48+j){
				flag = 1;
			}
		}
		j = 0;
		if(flag == 1 || ((flagn == 1) && (i == 0))){
			time_s[i] = argv[1][i];
			flag = 0;
		}else{
			printf("%s: malformed time.\n", argv[1]);
			printf("usage: ./timeit <seconds>\n");
			exit(3);
		}
	}
	
	second = 2*strtol(time_s, &ptr ,10);
	
	/*deals with errors*/
	if(flagn == 0){
		if(second > (999999999*2)){
			printf("%s: time out of range.\n", argv[1]);
			printf("usage: ./timeit <seconds>\n");
			exit(4);
		}
	}else if(second != 0){
		if(strlen(argv[1]) > 9){
			printf("%s: time out of range.\n", argv[1]);
			printf("usage: ./timeit <seconds>\n");
			exit(5);
		}else if(flagn == 1){
			printf("Invalid time(%s). Must be >= 0.\n", argv[1]);
			printf("usage: ./timeit <seconds>\n");
			exit(6);
			
		}
	}
	
	/*since while loop below runs two times 
	  every second the argument needs to be 
	  doubled also*/
	second = 2*strtol(time_s, &ptr ,10);

	memset(&sa, 0, sizeof(sa));
    
	/*sets sa to run handler on SIGALRM*/
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	sa.sa_handler = handler;
	sigaction(SIGALRM, &sa, NULL);
	
    /*starts timer*/
	setitimer(ITIMER_REAL, &alr, &old);
	
	while(second--){
		/*waits for SIGALRM*/
		pause();	
	}
	
	printf("Time's up!\n");
	
	return 0;
}
