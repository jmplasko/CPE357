#include "create.h"

int findtype(struct stat file){
  int type = 0;
  
  if S_ISREG(file.st_mode)
    type = 1;
  if S_ISLNK(file.st_mode)
    type = 2;
  if S_ISDIR(file.st_mode)
    type = 3;
  
  return type;
}

void create_header(char *pathnamefk, int tarfile, int offset, int f, int s){
  struct stat current;
  struct passwd *user;
  struct group *usergroup;
  
  char pathname[100000] = {0};
  char name[100] = {0};
  char mode[8] = {0};
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
  char block[512] = {0};
  char count[512] = {0};
  char holder[255] = {0};
  char big[2000] = {0};
  char buffer[1] = {0}; 
 
  int updated_mode, type;
  int i = 0;
  int startname = 0;
  unsigned int total = 0;
  int fl;  
  int printnulls = 0;
  int gg = 0;
  int first = 0;

  DIR *dir;
  struct dirent *dst;

  strcpy(pathname, pathnamefk);

  lseek(tarfile, offset, SEEK_CUR);
  write(tarfile, block, 512);
  lseek(tarfile, -512, SEEK_CUR);
  /* lstat file and check for error */
  if ( -1 == lstat(pathname, &current) ){
    printf("lstat failure \n");
    return;
  }
  
  /* Write the 'name' */
  if (strlen(pathname) > 255){
    printf("NO fixing");
    return;
  }

  type = findtype(current);
  
  if(type == 3){
     gg = strlen(pathname);
     if(pathname[gg-1] != 47){
        strcat(pathname, "/");
     }
  }


  //printf("%s ", pathname);
  if(strlen(pathname) > 100){
    while(strlen(pathname) > i){
      if(strlen(holder) ==  155){
        printf("broke");
        return;
      }
      holder[i] = pathname[i];
      
      if(pathname[i] == 47){
        if((strlen(pathname)-i) < 101){
          holder[i] = 0;
          strcpy(prefix, holder);
          startname = i+1;
          i = 256;
        }
      }
      i++;
    }
    i = 0;
    while(startname < strlen(pathname)){
      name[i] = pathname[startname];
      startname++;
      i++;
    }
    i = 0;
  }else{
    strcpy(name, pathname);
  }
  
  if(f != 0){
     printf("%s\n", pathname);
  }
  
  write(tarfile, name, 100);
  
  /* Find out if our file is a regular file, a symlink, or a directory */
  
  /* Write out the 'mode' */
  if ( 1 == type ){
    updated_mode = current.st_mode - 32768;
  }
  if ( 2 == type ){
    updated_mode = current.st_mode - 40960;
  }
  if ( 3 == type ){
    updated_mode = current.st_mode - 16384;
  }  

  //printf("dhghdhgjf");

  sprintf(mode, "%07o\0", updated_mode);
  write(tarfile, mode, 8);
  
  
  if(s == 0){
     /* Write out the 'uid' */
     if ( 2097151 < current.st_uid )
       insert_special_int(uid, 8, current.st_uid);
     else
       sprintf(uid, "%07o\0", current.st_uid);
  }else{
     /* Write out the 'uid' */
     if ( 2097151 >= current.st_uid )
       sprintf(uid, "%07o\0", current.st_uid);
     else
        printf("%s: Unable to create conforming header.  Skipping.\n", name);
  }

  write(tarfile, uid, 8);
  
  /* Write out the 'gid' */
  sprintf(gid, "%07o\0", current.st_gid);
  write(tarfile, gid, 8);
  
  /* Write out the 'size' */
  if ( 2 == type || 3 == type )
    write(tarfile, "00000000000", 12); /* Our file is a symlink or directory */
  else{
    /* Our file is a normal file */
    sprintf(size, "%011o\0", current.st_size);
    write(tarfile, size, 12);
  }
  
  /* Write out the 'mtime' */
  sprintf(mtime, "%o\0", current.st_mtime);
  write(tarfile, mtime, 12);
  
  /* Write in spaces for chksum */
  write(tarfile, "        ", 8);
  
  /* Write out the 'typeflag' */
  /* add case for regular file (alternate) */
  if ( 1 == type )
    write(tarfile, "0", 1);
  if ( 2 == type )
    write(tarfile, "2", 1);
  if ( 3 == type )
    write(tarfile, "5", 1);
  
  /* Write out the 'linkname' */
  if ( 2 == type )
    readlink(pathname, linkname, 100); /* Our file is a symlink */
    
  write(tarfile, linkname, 100);
  
  /* Write out the 'magic' */
  write(tarfile, "ustar", 6);
  
  /* Write out the 'version' */
  write(tarfile, "00", 2);
  
  /* Acquire user passwd and group structs */
  user = getpwuid(current.st_uid);
  usergroup = getgrgid(current.st_gid);
  
  /* Error handling */
  if ( NULL == user ){
    printf("error retrieving passwd struct/n");
    exit(5);
  }
  
  if ( NULL == usergroup ){
    printf("error retrieving group struct/n");
    exit(6);
  }
  /* Write out the 'uname' */
  strncpy(uname, user->pw_name, 31);
  write(tarfile, uname, 32);
  
  /* Write out the 'gname' */
  strncpy(gname, usergroup->gr_name, 31);
  write(tarfile, gname, 32);
  
  /* Write out the 'devmajor' and 'devminor' */
  /* 16 null characters??? */
  write(tarfile, devs, 16); /*TO DO: ASK ABOUT 16 NULLS FOR devmajor and devminor*/
  
  /* Write out the 'prefix' */
  write(tarfile, prefix, 155); /* TO DO: FIGURE OUT HOW THE PREFIX WORKS */
  
  lseek(tarfile, -500, SEEK_CUR);
  
  read(tarfile, count, 512);
  
  while( i < 512){
    total += (unsigned char)count[i];
    i++;
  }
 
  lseek(tarfile, -364, SEEK_CUR);
  sprintf(chksum, "%07o\0", total);
  write(tarfile, chksum, 8);
  
  lseek(tarfile, 356, SEEK_CUR);

  offset += 512;

  if(type == 1){
     fl = open(pathname, O_RDONLY);
     first = read(fl, buffer, 1);
     if(first != 0){
        write(tarfile, buffer, 1);
        offset++;
        while(read(fl, buffer, 1) != 0) {
           write(tarfile, buffer, 1);
           offset++;
        }
        printnulls = 512 - (offset%512);
        while(printnulls > 0){
           write(tarfile, "\0", 1);
           offset ++;
           printnulls --;
        }
     }
     close(fl);
  }
  /*if(type == 2){
     fl = open(pathname, O_RDONLY);
     first = read(fl, buffer, 1);
     if(first != 0){
        write(tarfile, buffer, 1);
        offset++;
        while(read(fl, buffer, 1) != 0) {
           write(tarfile, buffer, 1);
           offset++;
        }
        printnulls = 512 - (offset%512);
        while(printnulls > 0){
           write(tarfile, "\0", 1);
           offset ++;
           printnulls --;
        }
     }
     close(fl);
  }*/
  if(type == 3){
     dir = opendir(pathname);
     if(dir == NULL){
        return;
     }
     while((dst = readdir(dir)) != NULL){
        if((strcmp(dst->d_name, ".") != 0)  &&  (strcmp(dst->d_name, "..") != 0)){
           strcpy(big, pathname);
           //strcat(big, "/");
           strcat(big, dst->d_name);
           create_header(big, tarfile, 0, f, s);
           memset(big, 0, sizeof(big));
        }
     }
     closedir(dir);
     return;
  }

  //close(tarfile);
  return;
}
