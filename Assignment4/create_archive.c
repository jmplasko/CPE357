#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <limits.h>
#include <stdint.h>
#include <fcntl.h>
#include <libgen.h>
#include <pwd.h>
#include <grp.h>


int main(int argc, char *argv[]){
  FILE* file;
  int outfile;
  struct stat current;
  struct passwd *user;
  struct group *usergroup;
  
  char name[100] = {0};
  char byte[8] = {0}; /* aka mode */
  char uid[8] = {0};
  char gid[8] = {0};
  char size[12] = {0};
  char mtime[12] = {0};
  char chksum[8] = {0};
  char linkname[100] = {0};
  char uname[32] = {0};
  char gname[32] = {0};
  char devs[16] = {0};
  char prefix[155] = {0};
  char lol[500] = {0};
  int mode;
  int i = 0;
  int total = 0;
  
  file = fopen(argv[1], "r");
  outfile = open(argv[2], O_RDWR | O_CREAT | O_TRUNC, S_ISUID | S_ISGID | S_ISVTX | S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH );
  lstat(argv[1], &current);
  
  if ( strlen(argv[1]) > 100 ){
    printf("yo your path is too long\n");
    exit(2);
  }
  
  strcpy(name, argv[1]);
  write(outfile, name, 100);
  
  //printf("%o\n", current.st_mode);
  
  if S_ISREG(current.st_mode)
    mode = current.st_mode - 32768;
  if S_ISLNK(current.st_mode)
    mode = current.st_mode - 40960;
  if S_ISDIR(current.st_mode)
    mode = current.st_mode - 16384;
  
  sprintf(byte, "%07o\0", mode);
  write(outfile, byte, 8);
  
  /*figure this shit out */
  write(outfile, "9999999\0", 8); /* user id*/
  
  sprintf(gid, "%07o\0", current.st_gid);
  write(outfile, gid, 8); /*gid*/
  
  if ( S_ISLNK(current.st_mode) || S_ISDIR(current.st_mode) )
    write(outfile, "00000000000", 12);
  else{
    sprintf(size, "%011o\0", current.st_size);
    write(outfile, size, 12); /* size */
  }
  
  sprintf(mtime, "%o\0", current.st_mtime);
  write(outfile, mtime, 12); /*mtime*/
  
  /*spaces for chksum at the moment, go back seek_set, offset of 148, write in sum */
  write(outfile, "        ", 8);
  
  /* add case for regular file (alternate) */
  if S_ISREG(current.st_mode)
    write(outfile, "0", 1);
  if S_ISLNK(current.st_mode)
    write(outfile, "2", 1);
  if S_ISDIR(current.st_mode)
    write(outfile, "5", 1);
  
  
  if ( S_ISLNK(current.st_mode) )
    readlink(argv[1], linkname, 100);
    
  write(outfile, linkname, 100);
  
  write(outfile, "ustar", 6);/*magic*/
  
  write(outfile, "00", 2);/*version*/
  
  user = getpwuid(current.st_uid);
  usergroup = getgrgid(current.st_gid);
  
  strncpy(uname, user->pw_name, 31);
  write(outfile, uname, 32); /*uname*/
  
  strncpy(gname, usergroup->gr_name, 31);
  write(outfile, gname, 32); /*gname*/
  
  /* write in 16 null characters!!!! */
  write(outfile, devs, 16); /*devmajor and devminor*/
  
  /*figure out prefix*/
  write(outfile, prefix, 155);
  
  lseek(outfile, -500, SEEK_CUR);
  
  
  read(outfile, lol, 500);
  

  while(i < 500){
	  printf("%c", lol[i]);
	  total += lol[i];
	  i++;
  }
  
  printf("\n%o\n", total);
  
  
  fclose(file);
  close(outfile);
  
  
  
  
  
  return 0;
}