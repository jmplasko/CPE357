#include "table.h"

void print_table(int tarfile, int s){
  char name[100] = {0};
  char prefix[155] = {0};
  char size_octal[12] = {0};
  char combined[255] = {0};
  char uid[8] = {0};
  char count[512] = {0};
  char chksum[8] = {0};
  unsigned long cnt = 0;
  long size;
  int portion_filled, skip_over;
  char *ptr, *t;
  int done = 0;
  int i = 0;
  long chk = 0;
  uint32_t user_converted;
  
  
  while (done == 0){
    /*Read name*/
    read(tarfile, name, 100);
    /*Check to see if we are done*/
    if(name[0] == '\0'){
      done = 1;
      break;
    }
    
    lseek(tarfile, 8, SEEK_CUR);
    read(tarfile, uid, 8);

    /*Seek to size and read size*/
    lseek(tarfile, 8, SEEK_CUR);
    read(tarfile, size_octal, 12);
    size = strtol(size_octal, &ptr, 8);

    lseek(tarfile, 12, SEEK_CUR);
    read(tarfile, chksum, 8);


    user_converted = extract_special_int(uid, 8);
    if(s != 0){
        /* Write out the 'uid' */
        if ( 2097151 < user_converted ){
          printf("Malformed header found.  Bailing.\n");
          return;
        }
      }  
       chk = strtol(chksum, &t, 8);
       
       lseek(tarfile, -156, SEEK_CUR);

       read(tarfile, count, 512);

       while( i < 512){
          cnt += (unsigned char)count[i];
          if(i == 147)
             i += 8;
          i++;
       }
       i = 0;

       memset(count, '\0', sizeof(count));

       cnt += 256;

       if(cnt != chk){
          printf("Malformed header found.  Bailing.\n");
          return;
       }
       cnt = 0;
       chk = 0;        
       lseek(tarfile, -512, SEEK_CUR);
       lseek(tarfile, 156, SEEK_CUR);
    
  
    /* Seek to prefix and read */
    lseek(tarfile, 189, SEEK_CUR);
    read(tarfile, prefix, 155);
    if ( prefix[0] != '\0' ){
      /* Combine name and prefix */
      strcpy(combined, prefix);
      strcat(combined, "/");
      strcat(combined, name);
      printf("%s\n", combined);
      memset(combined, '\0', 255);
    }
    else{
      printf("%s\n", name);
    }
    
    lseek(tarfile, 12, SEEK_CUR);
  
    lseek(tarfile, size, SEEK_CUR);
    portion_filled = size % 512;
    
    if ( portion_filled != 0 ){
      skip_over = 512 - portion_filled;
      lseek(tarfile, skip_over, SEEK_CUR);
    }
    memset(name, '\0', 100);
    memset(prefix, '\0', 155);
    memset(size_octal, '\0', 12);
    
  }
}

void print_table_find(int tarfile, char list_of_files[][256], int arr, int s){
  char name[101] = {0};
  char prefix[155] = {0};
  char size_octal[12] = {0};
  char combined[257] = {0};
  long size;
  char type;
  int portion_filled, skip_over;
  char *ptr, *t;
  int done = 0;
  int j = 0;
  int i =0;
  char holdpath[257] = {0};
  uint32_t user_converted;
  long chk = 0;
  int k = 0; 
  unsigned long cnt = 0;
  char count[512] = {0};
  char uid[8] = {0};
  char chksum[8] = {0};
  
  while (done == 0){
    /*Read name*/
    read(tarfile, name, 100);
    /*Check to see if we are done*/
    if(name[0] == '\0'){
      done = 1;
      break;
    }
    
 
    lseek(tarfile, 24, SEEK_CUR);   
    read(tarfile, size_octal, 12);
    size = strtol(size_octal, &ptr, 8);

    lseek(tarfile, 20, SEEK_CUR);
    read(tarfile, &type, 1);

    

    /* Seek to prefix and read */
    lseek(tarfile, 188, SEEK_CUR);
    read(tarfile, prefix, 155);
    if ( prefix[0] != '\0' ){
      /* Combine name and prefix */
      strcpy(combined, prefix);
      strcat(combined, "/");
      strcat(combined, name);
    for(j = 0; j < arr; j++){   
        if(strlen(combined) >= strlen(list_of_files[j])){
        for(i = 0; i <=strlen(combined); i++){
          if(((combined[i] == '/') 
            && (strlen(holdpath)>= (strlen(list_of_files[j])-1))) 
            || ((!strcmp(holdpath, list_of_files[j])) 
            && (combined[i] == '\0'))){
            
                   if(strstr(list_of_files[j], holdpath)){
                          printf("%s\n", combined);
              i = 258;
              j = arr+1;
              
                   }else{
              holdpath[i] = combined[i];
             }
          }else{
             holdpath[i] = combined[i];
           }
                }
        memset(holdpath, '\0', 257);
      }
      }
    j = 0;
      memset(combined, '\0', 257);
    }
    else{
     for(j = 0; j < arr; j++){    
        if(strlen(name) >= strlen(list_of_files[j])){
        for(i = 0; i <= strlen(name); i++){
          if(((name[i] == '/') 
            && (strlen(holdpath)>= (strlen(list_of_files[j])-1)))
            || ((!strcmp(holdpath, list_of_files[j])) 
            && (name[i] == '\0'))) {
            
                   if(strstr(list_of_files[j], holdpath)){
                          printf("%s\n", name);
              i = 258;
              j = arr+1;
              //memset(holdpath, '\0', 255);
                   }else{
              holdpath[i] = name[i];
             }
           }else{
             holdpath[i] = name[i];
           }
                }
        memset(holdpath, '\0', 257);
      }
      }
    j = 0;
    }
    
    lseek(tarfile, 12, SEEK_CUR);
  
    lseek(tarfile, size, SEEK_CUR);
    portion_filled = size % 512;
    
    if ( portion_filled != 0 ){
      skip_over = 512 - portion_filled;
      lseek(tarfile, skip_over, SEEK_CUR);
    }
    memset(name, '\0', 101);
    memset(prefix, '\0', 155);
    memset(size_octal, '\0', 12);
    
  }
}

void print_table_v(int tarfile, int amount_v, int s){
  char name[100] = {0};
  char prefix[155] = {0};
  char size_octal[12] = {0};
  char combined[255] = {0};
  char perm[10] = {0};
  char uname[32] = {0};
  char ugroup[32] = {0};
  char timew[16] = {0};
  char uid[8] = {0};
  char gid[8] = {0};
  char chksum[8] = {0};
  char linkname[100] = {0};
  char mode[8] = {0};
  char type;
  long size;
  int portion_filled, skip_over;
  char *ptr;
  int done = 0;
  struct tm date;
  struct tm spdate;
  char dates[50] = {0};
  char spdates[50] = {0};


  while (done == 0){
    /*Read name*/
    read(tarfile, name, 100);
    /*Check to see if we are done*/
    if(name[0] == '\0'){
      done = 1;
      break;
    }
    read(tarfile, perm, 8);
    lseek(tarfile, -6, SEEK_CUR);
    read(tarfile, mode, 6);
    read(tarfile, uid, 8);
    read(tarfile, gid, 8);

    /*Seek to size and read size*/
    //lseek(tarfile, 16, SEEK_CUR);
    read(tarfile, size_octal, 12);
    size = strtol(size_octal, &ptr, 8);

    read(tarfile, timew, 12);
    read(tarfile, chksum, 8);
    //lseek(tarfile, 8, SEEK_CUR);
    read(tarfile, &type, 1);
    read(tarfile, linkname, 100);
    lseek(tarfile, 8, SEEK_CUR);

    read(tarfile, uname, 32);
    read(tarfile, ugroup, 32);

    /* Seek to prefix and read */
    lseek(tarfile, 16, SEEK_CUR);
    read(tarfile, prefix, 155);
    
    if(amount_v > 1){

      int hip = strtol(timew, NULL, 8);
      time_t winner = hip;
      uint32_t user_converted;
      spdate = *localtime(&winner);
      strftime(spdates, sizeof(spdates), "%a %b %d %H:%M:%S %Y", &spdate);
      
      printf("reading header:\n");
      printf("  char      name[]: %s\n", name);
      printf("  mode_t      mode: %s\n", mode);
      user_converted = extract_special_int(uid, 8);
      if ( 2097151 < user_converted )
        printf("  uid_t        uid: %d\n", user_converted);
      else
        printf("  uid_t        uid: %d\n", strtol(uid, NULL, 8));
      printf("  gid_t        gid: %d\n", strtol(gid, NULL, 8));
      printf("  size_t      size: %d\n", strtol(size_octal, NULL, 8));
      printf("  time_t     mtime: %d --- %s\n", hip, spdates);
      printf("  int       chksum: %d\n", strtol(chksum, NULL, 8));
      printf("  char    typeflag: %c\n", type);
      printf("  char  linkname[]: %s\n", linkname);
      printf("  char     magic[]: ustar\n");
      printf("  char   version[]: 00\n");
      printf("  char     uname[]: %s\n", uname);
      printf("  char     gname[]: %s\n", ugroup);
      printf("  int     devmajor: 0\n");
      printf("  int     devminor: 0\n");
      printf("  char    prefix[]: %s\n", prefix);
      printf("Read header\n");
    }


    if ( prefix[0] != '\0' ){
      /* Combine name and prefix */
      strcpy(combined, prefix);
      strcat(combined, "/");
      strcat(combined, name);
      //printf("%s\n", combined);
      
      if(type == '5'){
         printf("d");
      }else if(type == '2'){
     printf("l");
    }else{
         printf("-");
      }

      if(perm[4] == '0'){
         printf("-");
         printf("-");
         printf("-");
      }else if(perm[4] == '1'){
         printf("-");
         printf("-");
         printf("x");
      }else if(perm[4] == '2'){
         printf("-");
         printf("w");
         printf("-");
      }else if(perm[4] == '3'){
         printf("-");
         printf("w");
         printf("x");
      }else if(perm[4] == '4'){
         printf("r");
         printf("-");
         printf("-");
      }else if(perm[4] == '5'){
         printf("r");
         printf("-");
         printf("x");
      }else if(perm[4] == '6'){
         printf("r");
         printf("w");
         printf("-");
      }else if(perm[4] == '7'){
         printf("r");
         printf("w");
         printf("x");
      }


      if(perm[5] == '0'){
         printf("-");
         printf("-");
         printf("-");
      }else if(perm[5] == '1'){
         printf("-");
         printf("-");
         printf("x");
      }else if(perm[5] == '2'){
         printf("-");
         printf("w");
         printf("-");
      }else if(perm[5] == '3'){
         printf("-");
         printf("w");
         printf("x");
      }else if(perm[5] == '4'){
         printf("r");
         printf("-");
         printf("-");
      }else if(perm[5] == '5'){
         printf("r");
         printf("-");
         printf("x");
      }else if(perm[5] == '6'){
         printf("r");
         printf("w");
         printf("-");
      }else if(perm[5] == '7'){
         printf("r");
         printf("w");
         printf("x");
      }

      if(perm[6] == '0'){
         printf("-");
         printf("-");
         printf("-");
      }else if(perm[6] == '1'){
         printf("-");
         printf("-");
         printf("x");
      }else if(perm[6] == '2'){
         printf("-");
         printf("w");
         printf("-");
      }else if(perm[6] == '3'){
         printf("-");
         printf("w");
         printf("x");
      }else if(perm[6] == '4'){
         printf("r");
         printf("-");
         printf("-");
      }else if(perm[6] == '5'){
         printf("r");
         printf("-");
         printf("x");
      }else if(perm[6] == '6'){
         printf("r");
         printf("w");
         printf("-");
      }else if(perm[6] == '7'){
         printf("r");
      }

       printf(" %s/", uname);
       printf("%s ", ugroup);
       printf("%8d ", size);
       int value = strtol(timew, NULL, 8);
       time_t loser = value;
       date = *localtime(&loser);
       strftime(dates, sizeof(dates), "%Y-%m-%d %H:%M", &date);
       printf("%s ", dates);
       printf("%s\n", combined);

       memset(combined, '\0', 255);


    }else{
      
      //printf("%c ", type);

      if(type == '5'){
         printf("d");
      }else if(type == '2'){
     printf("l");
    }else{
         printf("-");
      }

      /*printf("%s ", perm);
      printf("%c ", perm[4]);
      printf("%c ", perm[5]);
      printf("%c\n", perm[6]);*/
      

      if(perm[4] == '0'){
         printf("-");
         printf("-");
         printf("-");
      }else if(perm[4] == '1'){
         printf("-");
         printf("-");
         printf("x");
      }else if(perm[4] == '2'){
         printf("-");
         printf("w");
         printf("-");
      }else if(perm[4] == '3'){
         printf("-");
         printf("w");
         printf("x");
      }else if(perm[4] == '4'){
         printf("r");
         printf("-");
         printf("-");
      }else if(perm[4] == '5'){
         printf("r");
         printf("-");
         printf("x");
      }else if(perm[4] == '6'){
         printf("r");
         printf("w");
         printf("-");
      }else if(perm[4] == '7'){
         printf("r");
         printf("w");
         printf("x");
      }

      if(perm[5] == '0'){
         printf("-");
         printf("-");
         printf("-");
      }else if(perm[5] == '1'){
         printf("-");
         printf("-");
         printf("x");
      }else if(perm[5] == '2'){
         printf("-");
         printf("w");
         printf("-");
      }else if(perm[5] == '3'){
         printf("-");
         printf("w");
         printf("x");
      }else if(perm[5] == '4'){
         printf("r");
         printf("-");
         printf("-");
      }else if(perm[5] == '5'){
         printf("r");
         printf("-");
         printf("x");
      }else if(perm[5] == '6'){
         printf("r");
         printf("w");
         printf("-");
      }else if(perm[5] == '7'){
         printf("r");
         printf("w");
         printf("x");
      }

      if(perm[6] == '0'){
         printf("-");
         printf("-");
         printf("-");
      }else if(perm[6] == '1'){
         printf("-");
         printf("-");
         printf("x");
      }else if(perm[6] == '2'){
         printf("-");
         printf("w");
         printf("-");
      }else if(perm[6] == '3'){
         printf("-");
         printf("w");
         printf("x");
      }else if(perm[6] == '4'){
         printf("r");
         printf("-");
         printf("-");
      }else if(perm[6] == '5'){
         printf("r");
         printf("-");
         printf("x");
      }else if(perm[6] == '6'){
         printf("r");
         printf("w");
         printf("-");
      }else if(perm[6] == '7'){
         printf("r");
         printf("w");
         printf("x");
      }

      printf(" %s/", uname);
      printf("%s ", ugroup);
      printf("%8d ", size);
      //printf("%s ", timew);
      int value = strtol(timew, NULL, 8);
      time_t loser = value;
      date = *localtime(&loser); 
      strftime(dates, sizeof(dates), "%Y-%m-%d %H:%M", &date);
      printf("%s ", dates);
      printf("%s\n", name);
    }

    lseek(tarfile, 12, SEEK_CUR);

    lseek(tarfile, size, SEEK_CUR);
    portion_filled = size % 512;

    if ( portion_filled != 0 ){
      skip_over = 512 - portion_filled;
      lseek(tarfile, skip_over, SEEK_CUR);
    }
    memset(name, '\0', 100);
    memset(prefix, '\0', 155);
    memset(size_octal, '\0', 12);
    memset(perm, '\0', 10);
    memset(uname, '\0', 32);
    memset(ugroup, '\0', 32);
    memset(timew, '\0', 16);
    type = 0;
 }
}

void print_table_find_v(int tarfile, int amount_v, 
                        char list_of_files[][256], int arr, int s){
  int j = 0;
  int i =0;
  char holdpath[257] = {0};
  char name[101] = {0};
  char prefix[155] = {0};
  char size_octal[12] = {0};
  char combined[257] = {0};
  char perm[10] = {0};
  char uname[32] = {0};
  char ugroup[32] = {0};
  char timew[16] = {0};
  char uid[8] = {0};
  char gid[8] = {0};
  char chksum[8] = {0};
  char linkname[100] = {0};
  char mode[8] = {0};
  char type;
  long size;
  int portion_filled, skip_over;
  char *ptr;
  int done = 0;
  struct tm date;
  struct tm spdate;
  char dates[50] = {0};
  char spdates[50] = {0};
  int flag2 = 0;

  while (done == 0){
    /*Read name*/
    read(tarfile, name, 100);
    /*Check to see if we are done*/
    if(name[0] == '\0'){
      done = 1;
      break;
    }
    read(tarfile, perm, 8);
    lseek(tarfile, -6, SEEK_CUR);
    read(tarfile, mode, 6);
    read(tarfile, uid, 8);
    read(tarfile, gid, 8);

    /*Seek to size and read size*/
    //lseek(tarfile, 16, SEEK_CUR);
    read(tarfile, size_octal, 12);
    size = strtol(size_octal, &ptr, 8);

    read(tarfile, timew, 12);
    read(tarfile, chksum, 8);
    //lseek(tarfile, 8, SEEK_CUR);
    read(tarfile, &type, 1);
    read(tarfile, linkname, 100);
    lseek(tarfile, 8, SEEK_CUR);

    read(tarfile, uname, 32);
    read(tarfile, ugroup, 32);

    /* Seek to prefix and read */
    lseek(tarfile, 16, SEEK_CUR);
    read(tarfile, prefix, 155);
    
  if(amount_v > 1){
      int hip = strtol(timew, NULL, 8);
      time_t winner = hip;
      uint32_t user_converted;
      spdate = *localtime(&winner);
      strftime(spdates, sizeof(spdates), "%a %b %d %H:%M:%S %Y", &spdate);

      printf("reading header:\n");
      printf("  char      name[]: %s\n", name);
      printf("  mode_t      mode: %s\n", mode);
      user_converted = extract_special_int(uid, 8);
      if ( 2097151 < user_converted )
        printf("  uid_t        uid: %d\n", user_converted);
      else
        printf("  uid_t        uid: %d\n", strtol(uid, NULL, 8));
      printf("  gid_t        gid: %d\n", strtol(gid, NULL, 8));
      printf("  size_t      size: %d\n", strtol(size_octal, NULL, 8));
      printf("  time_t     mtime: %d --- %s\n", hip, spdates);
      printf("  int       chksum: %d\n", strtol(chksum, NULL, 8));
      printf("  char    typeflag: %c\n", type);
      printf("  char  linkname[]: %s\n", linkname);
      printf("  char     magic[]: ustar\n");
      printf("  char   version[]: 00\n");
      printf("  char     uname[]: %s\n", uname);
      printf("  char     gname[]: %s\n", ugroup);
      printf("  int     devmajor: 0\n");
      printf("  int     devminor: 0\n");
      printf("  char    prefix[]: %s\n", prefix);
      printf("Read header\n");
    
    if ( prefix[0] != '\0' ){
      /* Combine name and prefix */
      strcpy(combined, prefix);
      strcat(combined, "/");
      strcat(combined, name);
    for(j = 0; j < arr; j++){   
        if(strlen(combined) >= strlen(list_of_files[j])){
        for(i = 0; i <= strlen(combined); i++){
          //printf("%s ", holdpath);
          if(((combined[i] == '/') 
            && (strlen(holdpath)>= (strlen(list_of_files[j])-1))) 
             || (!(strcmp(holdpath, list_of_files[j])) 
       && (combined[i] == '\0')) ){
               
              if(strstr(list_of_files[j], holdpath)){
            flag2 = 1;
              printf(
        "(file_in_list:211) looking for \"%s\"...looking for \"%s\"...",
        combined, combined);
              printf("FOUND.\n");
              if(type == '5'){
               printf("d");
              }else if(type == '2'){
               printf("l");
              }else{
               printf("-");
              }

              if(perm[4] == '0'){
               printf("-");
               printf("-");
               printf("-");
              }else if(perm[4] == '1'){
               printf("-");
               printf("-");
               printf("x");
              }else if(perm[4] == '2'){
               printf("-");
               printf("w");
               printf("-");
              }else if(perm[4] == '3'){
               printf("-");
               printf("w");
               printf("x");
              }else if(perm[4] == '4'){
               printf("r");
               printf("-");
               printf("-");
              }else if(perm[4] == '5'){
               printf("r");
               printf("-");
               printf("x");
              }else if(perm[4] == '6'){
               printf("r");
               printf("w");
               printf("-");
              }else if(perm[4] == '7'){
               printf("r");
               printf("w");
               printf("x");
              }


              if(perm[5] == '0'){
               printf("-");
               printf("-");
               printf("-");
              }else if(perm[5] == '1'){
               printf("-");
               printf("-");
               printf("x");
              }else if(perm[5] == '2'){
               printf("-");
               printf("w");
               printf("-");
              }else if(perm[5] == '3'){
               printf("-");
               printf("w");
               printf("x");
              }else if(perm[5] == '4'){
               printf("r");
               printf("-");
               printf("-");
              }else if(perm[5] == '5'){
               printf("r");
               printf("-");
               printf("x");
              }else if(perm[5] == '6'){
               printf("r");
               printf("w");
               printf("-");
              }else if(perm[5] == '7'){
               printf("r");
               printf("w");
               printf("x");
              }

              if(perm[6] == '0'){
               printf("-");
               printf("-");
               printf("-");
              }else if(perm[6] == '1'){
               printf("-");
               printf("-");
               printf("x");
              }else if(perm[6] == '2'){
               printf("-");
               printf("w");
               printf("-");
              }else if(perm[6] == '3'){
               printf("-");
               printf("w");
               printf("x");
              }else if(perm[6] == '4'){
               printf("r");
               printf("-");
               printf("-");
              }else if(perm[6] == '5'){
               printf("r");
               printf("-");
               printf("x");
              }else if(perm[6] == '6'){
               printf("r");
               printf("w");
               printf("-");
              }else if(perm[6] == '7'){
               printf("r");
              }

               printf(" %s/", uname);
               printf("%s ", ugroup);
               printf("%8d ", size);
               int value = strtol(timew, NULL, 8);
               time_t loser = value;
               date = *localtime(&loser);
               strftime(dates, sizeof(dates), 
               "%Y-%m-%d %H:%M", &date);
               printf("%s ", dates);
               printf("%s\n", combined);
               i = 258;
               j = arr+1;
              //memset(holdpath, '\0', 255);
                   }else{ 
              holdpath[i] = combined[i];
             }
          }else{
             holdpath[i] = combined[i];
           }
                }
        memset(holdpath, '\0', 257);
      }
      }
    if(flag2 != 1){
      printf(
      "(file_in_list:211) looking for \"%s\"...looking for \"%s\"...", 
      combined, combined);
      printf("nope.\n");
    }
    flag2 = 0;
      j = 0;
        memset(combined, '\0', 257);
    }else{
     for(j = 0; j < arr; j++){    
        if(strlen(name) >= strlen(list_of_files[j])){
        for(i = 0; i <= strlen(name); i++){
          //printf("%s ", holdpath);
          //printf("%d ", i);
          //printf("%d ", strlen(name));
          if( ((name[i] == '/') 
            && (strlen(holdpath)>= (strlen(list_of_files[j])-1)))
               || ((!strcmp(holdpath, list_of_files[j])) 
         && (combined[i] == '\0')) ) {
          
              if(strstr(list_of_files[j], holdpath)){
                flag2 = 1;
                printf(
    "(file_in_list:211) looking for \"%s\"...looking for \"%s\"...",
        name, name);
                printf("FOUND.\n");
                if(type == '5'){
                 printf("d");
                }else if(type == '2'){
                 printf("l");
                }else{
                 printf("-");
                }

                if(perm[4] == '0'){
                 printf("-");
                 printf("-");
                 printf("-");
                }else if(perm[4] == '1'){
                 printf("-");
                 printf("-");
                 printf("x");
                }else if(perm[4] == '2'){
                 printf("-");
                 printf("w");
                 printf("-");
                }else if(perm[4] == '3'){
                 printf("-");
                 printf("w");
                 printf("x");
                }else if(perm[4] == '4'){
                 printf("r");
                 printf("-");
                 printf("-");
                }else if(perm[4] == '5'){
                 printf("r");
                 printf("-");
                 printf("x");
                }else if(perm[4] == '6'){
                 printf("r");
                 printf("w");
                 printf("-");
                }else if(perm[4] == '7'){
                 printf("r");
                 printf("w");
                 printf("x");
                }

                if(perm[5] == '0'){
                 printf("-");
                 printf("-");
                 printf("-");
                }else if(perm[5] == '1'){
                 printf("-");
                 printf("-");
                 printf("x");
                }else if(perm[5] == '2'){
                 printf("-");
                 printf("w");
                 printf("-");
                }else if(perm[5] == '3'){
                 printf("-");
                 printf("w");
                 printf("x");
                }else if(perm[5] == '4'){
                 printf("r");
                 printf("-");
                 printf("-");
                }else if(perm[5] == '5'){
                 printf("r");
                 printf("-");
                 printf("x");
                }else if(perm[5] == '6'){
                 printf("r");
                 printf("w");
                 printf("-");
                }else if(perm[5] == '7'){
                 printf("r");
                 printf("w");
                 printf("x");
                }

                if(perm[6] == '0'){
                 printf("-");
                 printf("-");
                 printf("-");
                }else if(perm[6] == '1'){
                 printf("-");
                 printf("-");
                 printf("x");
                }else if(perm[6] == '2'){
                 printf("-");
                 printf("w");
                 printf("-");
                }else if(perm[6] == '3'){
                 printf("-");
                 printf("w");
                 printf("x");
                }else if(perm[6] == '4'){
                 printf("r");
                 printf("-");
                 printf("-");
                }else if(perm[6] == '5'){
                 printf("r");
                 printf("-");
                 printf("x");
                }else if(perm[6] == '6'){
                 printf("r");
                 printf("w");
                 printf("-");
                }else if(perm[6] == '7'){
                 printf("r");
                }

                 printf(" %s/", uname);
                 printf("%s ", ugroup);
                 printf("%8d ", size);
                 int value = strtol(timew, NULL, 8);
                 time_t loser = value;
                 date = *localtime(&loser);
                 strftime(dates, sizeof(dates), "%Y-%m-%d %H:%M", &date);
                 printf("%s ", dates);
                 printf("%s\n", name);
                 i = 258;
         j = arr+1;
                     }else{
            holdpath[i] = name[i];
           }
           }else{
             holdpath[i] = name[i];
           }
                }
        memset(holdpath, '\0', 257);
      }
      }
    if(flag2 != 1){
      printf(
      "(file_in_list:211) looking for \"%s\"...looking for \"%s\"...", 
      name, name);
      printf("nope.\n");
    }
    flag2 = 0;
    j = 0;
    }  

    lseek(tarfile, 12, SEEK_CUR);

    lseek(tarfile, size, SEEK_CUR);
    portion_filled = size % 512;

    if ( portion_filled != 0 ){
      skip_over = 512 - portion_filled;
      lseek(tarfile, skip_over, SEEK_CUR);
    }
    memset(name, '\0', 101);
    memset(prefix, '\0', 155);
    memset(size_octal, '\0', 12);
    memset(perm, '\0', 10);
    memset(uname, '\0', 32);
    memset(ugroup, '\0', 32);
    memset(timew, '\0', 16);
    type = 0;
  
    }else{
  
    if ( prefix[0] != '\0' ){
      /* Combine name and prefix */
      strcpy(combined, prefix);
      strcat(combined, "/");
      strcat(combined, name);
    for(j = 0; j < arr; j++){   
        if(strlen(combined) >= strlen(list_of_files[j])){
        for(i = 0; i <= strlen(combined); i++){
          if(((combined[i] == '/') 
            && (strlen(holdpath)>= (strlen(list_of_files[j])-1))) 
            || ((!strcmp(holdpath, list_of_files[j])) 
            && (combined[i] == '\0'))){
                   if(strstr(list_of_files[j], holdpath)){
                      if(type == '5'){
               printf("d");
              }else if(type == '2'){
               printf("l");
              }else{
               printf("-");
              }

              if(perm[4] == '0'){
               printf("-");
               printf("-");
               printf("-");
              }else if(perm[4] == '1'){
               printf("-");
               printf("-");
               printf("x");
              }else if(perm[4] == '2'){
               printf("-");
               printf("w");
               printf("-");
              }else if(perm[4] == '3'){
               printf("-");
               printf("w");
               printf("x");
              }else if(perm[4] == '4'){
               printf("r");
               printf("-");
               printf("-");
              }else if(perm[4] == '5'){
               printf("r");
               printf("-");
               printf("x");
              }else if(perm[4] == '6'){
               printf("r");
               printf("w");
               printf("-");
              }else if(perm[4] == '7'){
               printf("r");
               printf("w");
               printf("x");
              }


              if(perm[5] == '0'){
               printf("-");
               printf("-");
               printf("-");
              }else if(perm[5] == '1'){
               printf("-");
               printf("-");
               printf("x");
              }else if(perm[5] == '2'){
               printf("-");
               printf("w");
               printf("-");
              }else if(perm[5] == '3'){
               printf("-");
               printf("w");
               printf("x");
              }else if(perm[5] == '4'){
               printf("r");
               printf("-");
               printf("-");
              }else if(perm[5] == '5'){
               printf("r");
               printf("-");
               printf("x");
              }else if(perm[5] == '6'){
               printf("r");
               printf("w");
               printf("-");
              }else if(perm[5] == '7'){
               printf("r");
               printf("w");
               printf("x");
              }

              if(perm[6] == '0'){
               printf("-");
               printf("-");
               printf("-");
              }else if(perm[6] == '1'){
               printf("-");
               printf("-");
               printf("x");
              }else if(perm[6] == '2'){
               printf("-");
               printf("w");
               printf("-");
              }else if(perm[6] == '3'){
               printf("-");
               printf("w");
               printf("x");
              }else if(perm[6] == '4'){
               printf("r");
               printf("-");
               printf("-");
              }else if(perm[6] == '5'){
               printf("r");
               printf("-");
               printf("x");
              }else if(perm[6] == '6'){
               printf("r");
               printf("w");
               printf("-");
              }else if(perm[6] == '7'){
               printf("r");
              }

               printf(" %s/", uname);
               printf("%s ", ugroup);
               printf("%8d ", size);
               int value = strtol(timew, NULL, 8);
               time_t loser = value;
               date = *localtime(&loser);
               strftime(dates, sizeof(dates),
               "%Y-%m-%d %H:%M", &date);
               printf("%s ", dates);
               printf("%s\n", combined);

               memset(combined, '\0', 257);
               i = 258;
               j = arr+1;
               //memset(holdpath, '\0', 255);
                   }else{
              holdpath[i] = combined[i];
             }
          }else{
             holdpath[i] = combined[i];
           }
                }
        memset(holdpath, '\0', 257);
      }
      }
    j = 0;
      memset(combined, '\0', 257);
    }
    else{
     for(j = 0; j < arr; j++){    
        if(strlen(name) >= strlen(list_of_files[j])){
        for(i = 0; i <= strlen(name); i++){
          if(((name[i] == '/') 
            && (strlen(holdpath)>= (strlen(list_of_files[j])-1)))
            || ((!strcmp(holdpath, list_of_files[j])) 
            && (combined[i] == '\0'))) {
                   if(strstr(list_of_files[j], holdpath)){
                      if(type == '5'){
                 printf("d");
                }else if(type == '2'){
                 printf("l");
                }else{
                 printf("-");
                }

                /*printf("%s ", perm);
                printf("%c ", perm[4]);
                printf("%c ", perm[5]);
                printf("%c\n", perm[6]);*/
                

                if(perm[4] == '0'){
                 printf("-");
                 printf("-");
                 printf("-");
                }else if(perm[4] == '1'){
                 printf("-");
                 printf("-");
                 printf("x");
                }else if(perm[4] == '2'){
                 printf("-");
                 printf("w");
                 printf("-");
                }else if(perm[4] == '3'){
                 printf("-");
                 printf("w");
                 printf("x");
                }else if(perm[4] == '4'){
                 printf("r");
                 printf("-");
                 printf("-");
                }else if(perm[4] == '5'){
                 printf("r");
                 printf("-");
                 printf("x");
                }else if(perm[4] == '6'){
                 printf("r");
                 printf("w");
                 printf("-");
                }else if(perm[4] == '7'){
                 printf("r");
                 printf("w");
                 printf("x");
                }

                if(perm[5] == '0'){
                 printf("-");
                 printf("-");
                 printf("-");
                }else if(perm[5] == '1'){
                 printf("-");
                 printf("-");
                 printf("x");
                }else if(perm[5] == '2'){
                 printf("-");
                 printf("w");
                 printf("-");
                }else if(perm[5] == '3'){
                 printf("-");
                 printf("w");
                 printf("x");
                }else if(perm[5] == '4'){
                 printf("r");
                 printf("-");
                 printf("-");
                }else if(perm[5] == '5'){
                 printf("r");
                 printf("-");
                 printf("x");
                }else if(perm[5] == '6'){
                 printf("r");
                 printf("w");
                 printf("-");
                }else if(perm[5] == '7'){
                 printf("r");
                 printf("w");
                 printf("x");
                }

                if(perm[6] == '0'){
                 printf("-");
                 printf("-");
                 printf("-");
                }else if(perm[6] == '1'){
                 printf("-");
                 printf("-");
                 printf("x");
                }else if(perm[6] == '2'){
                 printf("-");
                 printf("w");
                 printf("-");
                }else if(perm[6] == '3'){
                 printf("-");
                 printf("w");
                 printf("x");
                }else if(perm[6] == '4'){
                 printf("r");
                 printf("-");
                 printf("-");
                }else if(perm[6] == '5'){
                 printf("r");
                 printf("-");
                 printf("x");
                }else if(perm[6] == '6'){
                 printf("r");
                 printf("w");
                 printf("-");
                }else if(perm[6] == '7'){
                 printf("r");
                 printf("w");
                 printf("x");
                }

                printf(" %s/", uname);
                printf("%s ", ugroup);
                printf("%8d ", size);
                //printf("%s ", timew);
                int value = strtol(timew, NULL, 8);
                time_t loser = value;
                date = *localtime(&loser); 
                strftime(dates, sizeof(dates), 
                "%Y-%m-%d %H:%M", &date);
                printf("%s ", dates);
                printf("%s\n", name);
                  i = 258;
                  j = arr+1;
              //memset(holdpath, '\0', 255);
                   }else{
              holdpath[i] = name[i];
             }
           }else{
             holdpath[i] = name[i];
           }
                }
        memset(holdpath, '\0', 257);
      }
      }
    j = 0;
    }   

    lseek(tarfile, 12, SEEK_CUR);

    lseek(tarfile, size, SEEK_CUR);
    portion_filled = size % 512;

    if ( portion_filled != 0 ){
      skip_over = 512 - portion_filled;
      lseek(tarfile, skip_over, SEEK_CUR);
    }
    memset(name, '\0', 101);
    memset(prefix, '\0', 155);
    memset(size_octal, '\0', 12);
    memset(perm, '\0', 10);
    memset(uname, '\0', 32);
    memset(ugroup, '\0', 32);
    memset(timew, '\0', 16);
    type = 0;
  }
 }
}
