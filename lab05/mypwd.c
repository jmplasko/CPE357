#include <limits.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <unistd.h>
#include <libgen.h>
#include <fcntl.h>
#include <stdint.h>
#include <inttypes.h>

int main(int argc, char* arv[]){
	
	char pp[1254] = {0}; 
		/*if we assume that all 
	      directories are only 
		  one letter then we know 
		  that therecould never 
		  be string of /../../.. etc
		  that could be bigger than 
		  1024 to get back to root
		  form the startng directory*/
		  
	char *ff = "..";
	char *gg = "/..";
	char holder[2048]; /*strings of length 2048*/
	char this[2048];
	char new[2048];
	DIR *dir1, *dir2;
	struct stat sb;
	struct stat comp;
	long impdev = NULL;
	long impino = NULL;
	struct dirent *entry;
	int total = 0;
	int j;
	int i = 0;
	int len;
	
	strcpy(this,"");
	strcpy(pp,ff);
	
	dir1 = opendir(".");
	/*checking to see if it opened*/
	if(dir1 == 0){
		perror("mypwd");
		return 0;
	}
	/*finding the stats of the child direcotry*/
	i = stat(".", &sb);
	if(i == -1){
		closedir(dir1);
		perror("mypwd");
		return 0;
	}
	
	/*saves the child dev and 
	  ino to keep track of it*/
	impdev = sb.st_dev;
	impino = sb.st_ino;
	
	dir2 = opendir("..");
	/*checking to see if it opened*/
	if(dir2 == 0){
		closedir(dir1);
		perror("mypwd");
		return 0;
	}
	/*finding the stats of the parent direcotry*/
	i = stat("..", &comp);
	if(i == -1){
		closedir(dir1);
		closedir(dir2);
		perror("mypwd");
		return 0;
	}
	
	/*This loop starts at the directory it was in and runs until
	  it reaches root or something goes wrong*/
	while((sb.st_dev != comp.st_dev) || (sb.st_ino != comp.st_ino)){
		/*no mem leaks today*/
		closedir(dir1);
		closedir(dir2);
		
		dir1 = opendir(pp);
		if(dir1 == 0){
			perror("mypwd");
			return 0;
		}
		
		errno = 0;
		entry = readdir(dir1);
		
		if(errno != 0){
			closedir(dir1);
			perror("mypwd");
			return 0;
		}
		
		j = 1;
		/*This while loop finds the name of the child directory
		  but more spefically goes through all names in parent dir1*/
		while(entry != NULL){
			
			/*creates string that allows you to go
			  back to all the child directories*/
			strcpy(holder, pp);
			strcat(holder, "/");
			strcat(holder, entry->d_name);
			strcat(holder, "/.");
			
			/*this allows us to compare dev and ino to saved values*/
			i = lstat(holder, &comp);
		
			/*compares the saved dev and ino of child to 
			  dev and ino of the curretn name from directory*/
			if((impdev == comp.st_dev) && (impino == comp.st_ino)){
				total += strlen(entry->d_name);
				total ++;
				if(total > 2048){
					closedir(dir1);
					printf("path too long");
					return 0;
				}
				strcpy(new, entry->d_name);
				strcat(new, "/");
				strcat(new, this);
				strcpy(this, new);
				entry = NULL;
				j = 0;
			}else{
				errno = 0;
				entry = readdir(dir1);
				if(errno != 0){
					closedir(dir1);
					perror("mypwd");
					return 0;
				}
			}
		}
		
		/*this if statment can tell if its unlinked*/
		if(j == 1){
			closedir(dir1);
			printf("cannot get into current directory");
			return 0;
		}
		
		/*rest of this is as same as first part before while loop*/
		i = stat(pp, &sb);
		if(i == -1){
			closedir(dir1);
			perror("mypwd");
			return 0;
		}
		
		impdev = sb.st_dev;
		impino = sb.st_ino;
		
		strcat(pp, gg);
	
		dir2 = opendir(pp);
		if(dir2 == 0){
			closedir(dir1);
			perror("mypwd");
			return 0;
		}
		
		i = stat(pp, &comp);
		if(i == -1){
			closedir(dir1);
			closedir(dir2);
			perror("mypwd");
			return 0;
		}
		
    }
	
	closedir(dir1);
	closedir(dir2);
	
	/*just another check for length of string
	  and jusr does final edits to the string*/
	len = strlen(this);
	if(len > 1){
		this[len-1] = 0;
		total --;
	}
	
	total ++;
	if(total <= 2048){
		strcpy(new, "/");
		strcat(new, this);
		strcpy(this, new);
	}else{
		printf("path too long");
		return 0;
	}
	
	/*final strig to be path to be printed out*/
	printf("%s\n", this);
	
	return 0;
}