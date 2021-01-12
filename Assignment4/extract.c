#include "extract.h"

void extract(int tarfile, int v_flags, int s_flag){
  struct stat currentFile;
  struct utimbuf file_times;
  /* Arrays for the file contents */
  char name[100] = {0};
  char mode[8] = {0};
  char uid[8] = {0};
  char gid[8] = {0};
  char size[12] = {0};
  char mtime[12] = {0};
  char chksum[8] = {0};
  char typeflag;
  char linkname[100] = {0};
  char magic[6] = {0};
  char version[2] = {0};
  char uname[32] = {0};
  char gname[32] = {0};
  char prefix[155] = {0};
  char fullpath[256] = {0};
  
  char *contents;
  char *ptr; /* Used for strtol */
  
  int extracted_file;
  int portion_filled, skip_over;
  int done = 0, use_fullpath = 0;
  uint32_t user_converted;
  long file_mode, user, group, file_size, file_time, checksum;
  time_t mod_time;
  
  while ( done == 0 ){
    /* Read in the name */
    read(tarfile, name, 100);
    /* Check if we're done */
    if ( name[0] == '\0' ){
      done = 1;
      break;
    }
    
    /* Read in the mode */
    read(tarfile, mode, 8);
    file_mode = strtol(mode, &ptr, 8);
    
    /* Read in the user and group */
    read(tarfile, uid, 8);
    user_converted = extract_special_int(uid, 8);
    if ( 2097151 < user_converted )
      user = user_converted;
    else
      user = strtol(uid, &ptr, 8);
    
    read(tarfile, gid, 8);
    group = strtol(gid, &ptr, 8);
    
    /* Read in the size */
    read(tarfile, size, 12);
    file_size = strtol(size, &ptr, 8);
    /* Read in the mtime */
    read(tarfile, mtime, 12);
    file_time = strtol(mtime, &ptr, 8);
    mod_time = file_time;
    
    /* Read in the chksum */
    read(tarfile, chksum, 8);
    checksum = strtol(chksum, &ptr, 8);
    
    /* Read in the typeflag */
    read(tarfile, &typeflag, 1);
    
    /* Read in the linkname */
    if ( typeflag == '2' )
      read(tarfile, linkname, 100);
    else
      lseek(tarfile, 100, SEEK_CUR);
    
    /* Read in the magic number */
    read(tarfile, magic, 6);
    
    /* Read in the version */
    read(tarfile, version, 2);
    if(s_flag == 1){
      if ( strcmp(magic, "ustar\0") != 0 )
        break;
      
      if ( strcmp(version, "00") != 0 )
        break;
    }
    
    
    if ( 1 < v_flags ){
      read(tarfile, uname, 32);
      read(tarfile, gname, 32);
      lseek(tarfile, 16, SEEK_CUR);
    }
    else{
      lseek(tarfile, 80, SEEK_CUR); 
    }
    
    /* Read in the prefix */
    read(tarfile, prefix, 155);
    
    if ( prefix[0] != '\0' ){
      strcpy(fullpath, prefix);
      strcat(fullpath, "/");
      strcat(fullpath, name);
      use_fullpath = 1;
    }
    
    lseek(tarfile, 12, SEEK_CUR);
    
    /* Create our new file */
    if ( typeflag == '5' ){
      /* We have a directory */
      if ( use_fullpath == 1 ){
        make_full_path(fullpath);
        mkdir(fullpath, file_mode);
      }
      else{
        make_full_path(name);
        mkdir(name, file_mode);
      }
    }
    
    else if ( typeflag == '2' ){
      if ( use_fullpath == 1 ){
        make_full_path(fullpath);
        symlink(linkname, fullpath);
      }
      else{
        make_full_path(name);
        symlink(linkname, name);
      }
    }
    
    else{
      if ( use_fullpath == 1 ){
        make_full_path(fullpath);
        extracted_file = open(fullpath, O_RDWR | O_CREAT | O_TRUNC, S_IRUSR |
                    S_IWUSR);
      }
      else{
        make_full_path(name);
        extracted_file = open(name, O_RDWR | O_CREAT | O_TRUNC, S_IRUSR |
                    S_IWUSR);
        
        if ( -1 == stat(name, &currentFile) ){
          printf("stat_error\n");
          break;
        }
      }
      /* Allocate memory for the file contents */
      contents = malloc(file_size);
      if ( !contents ){
        printf("malloc error\n");
        exit(EXIT_FAILURE);
      }
      /* Read in the file contents */
      read(tarfile, contents, file_size);
      /* Write content to our extracted file*/
      write(extracted_file, contents, file_size);
      free(contents);
      close(extracted_file);
      
      if ( use_fullpath == 1 ){
        chmod(fullpath, file_mode);
        chown(fullpath, user, group);
        if ( -1 == stat(fullpath, &currentFile) ){
          printf("stat_error\n");
          break;
        }
        
        file_times.actime = currentFile.st_atime;
        file_times.modtime = file_time;
        
        if ( -1 == utime(fullpath, &file_times) ){
          printf("time error\n");
          break;
        }
      }
      else{
        chmod(name, file_mode);
        chown(name, user, group);
        
        if ( -1 == stat(name, &currentFile) ){
          printf("stat_error\n");
          break;
        }
        
        file_times.actime = currentFile.st_atime;
        file_times.modtime = file_time;
        
        if ( -1 == utime(name, &file_times) ){
          printf("time error\n");
          break;
        }
      }
      
      portion_filled = file_size % 512;
      if ( portion_filled != 0 ){
        skip_over = 512 - portion_filled;
        lseek(tarfile, skip_over, SEEK_CUR);
      }
    }
    
    if ( 0 < v_flags ){
      if ( 1 < v_flags ){
        char time_string[30] = {0};
        struct tm *tmp;
        
        tmp = localtime(&mod_time);
        strftime(time_string, 30, "%a %b %e %l:%M:%S %Y", tmp);
        
        printf("reading header:\n");
        printf("  char      name[]: %s\n", name);
        printf("  mode_t      mode: %s\n", mode + 2);
        printf("  uid_t        uid: %d\n", user);
        printf("  gid_t        gid: %d\n", group);
        printf("  size_t      size: %d\n", file_size);
        printf("  time_t     mtime: %d --- %s\n", file_time, time_string);
        printf("  int       chksum: %d\n", checksum);
        printf("  char    typeflag: %c\n", typeflag);
        printf("  char  linkname[]: %s\n", linkname);
        printf("  char     magic[]: %s\n", magic);
        printf("  char   version[]: %s\n", version);
        printf("  char     uname[]: %s\n", uname);
        printf("  char     gname[]: %s\n", gname);
        printf("  int     devmajor: 0\n");
        printf("  int     devminor: 0\n");
        printf("  char    prefix[]: %s\n", prefix);
        printf("Read header\n");
        memset(time_string, '\0', 24);
      }
      if ( use_fullpath == 1 )
        printf("%s\n", fullpath);
      else
        printf("%s\n", name);
    }
    
    
    memset(name, '\0', 100);
    memset(mode, '\0', 8);
    memset(uid, '\0', 8);
    memset(gid, '\0', 8);
    memset(size, '\0', 12);
    memset(mtime, '\0', 12);
    memset(chksum, '\0', 8);
    if ( typeflag == '2' )
      memset(linkname, '\0', 100);
    memset(magic, '\0', 6);
    memset(version, '\0', 2);
    memset(prefix, '\0', 155);
    
    if ( use_fullpath == 1 ){
      memset(fullpath, '\0', 256);
      use_fullpath = 0;
    }
  }
}

void extract_find(int tarfile, int v_flags,
                  char list_of_files[][256], int len, int s_flag){
  struct stat currentFile;
  struct utimbuf file_times;
  /* Arrays for the file contents */
  char name[101] = {0};
  char mode[8] = {0};
  char uid[8] = {0};
  char gid[8] = {0};
  char size[12] = {0};
  char mtime[12] = {0};
  char chksum[8] = {0};
  char typeflag;
  char linkname[100] = {0};
  char magic[6] = {0};
  char version[2] = {0};
  char uname[32] = {0};
  char gname[32] = {0};
  char prefix[155] = {0};
  char fullpath[257] = {0};
  char holdpath[257] = {0};
  
  char *contents;
  char *ptr; /* Used for strtol */
  
  int extracted_file;
  int portion_filled, skip_over, i, j;
  int done = 0, use_fullpath = 0, found_flag = 0;
  uint32_t user_converted;
  long file_mode, user, group, file_size, file_time, checksum;
  time_t mod_time;
  
  while ( done == 0 ){
    /* Read in the name */
    read(tarfile, name, 100);
    /* Check if we're done */
    if ( name[0] == '\0' ){
      done = 1;
      break;
    }
    
    /* Read in the mode */
    read(tarfile, mode, 8);
    file_mode = strtol(mode, &ptr, 8);
    
    /* Read in the user and group */
    read(tarfile, uid, 8);
    user_converted = extract_special_int(uid, 8);
    if ( 2097151 < user_converted )
      user = user_converted;
    else
      user = strtol(uid, &ptr, 8);
    
    read(tarfile, gid, 8);
    group = strtol(gid, &ptr, 8);
    
    /* Read in the size */
    read(tarfile, size, 12);
    file_size = strtol(size, &ptr, 8);
    /* Read in the mtime */
    read(tarfile, mtime, 12);
    file_time = strtol(mtime, &ptr, 8);
    mod_time = file_time;
    
    /* Read in the chksum */
    /*implement function to verify checksum is correct */
    read(tarfile, chksum, 8);
    checksum = strtol(chksum, &ptr, 8);
    
    /* Read in the typeflag */
    read(tarfile, &typeflag, 1);
    
    /* Read in the linkname */
    if ( typeflag == '2' )
      read(tarfile, linkname, 100);
    else
      lseek(tarfile, 100, SEEK_CUR);
    
    /* Read in the magic number */
    read(tarfile, magic, 6);
    if ( strcmp(magic, "ustar\0") != 0 ){
      printf("wrong magic number\n");
      break;
    }
    
    /* Read in the version */
    read(tarfile, version, 2);
    
    if ( strcmp(version, "00") != 0 ){
      printf("wrong version number\n");
      break;
    }
    
    if ( 1 < v_flags ){
      read(tarfile, uname, 32);
      read(tarfile, gname, 32);
      lseek(tarfile, 16, SEEK_CUR);
    }
    else{
      lseek(tarfile, 80, SEEK_CUR); 
    }
    
    /* Read in the prefix */
    read(tarfile, prefix, 155);
    
    if ( prefix[0] != '\0' ){
      strcpy(fullpath, prefix);
      strcat(fullpath, "/");
      strcat(fullpath, name);
      use_fullpath = 1;
    }
    
    lseek(tarfile, 12, SEEK_CUR);
    
    /* IMPLEMENT FIND HERE */
    for ( i = 0; i < len; i++ ){
      if ( use_fullpath == 1 ){
        /*FIND FOR FULLPATH*/
        if ( strlen(fullpath) >= strlen(list_of_files[i]) ){
          for ( j = 0; j <= strlen(fullpath); j++){
            //printf("j: %d\n", j);
            if( (fullpath[j] == '\0')
              && ( strlen(holdpath) >= (strlen(list_of_files[i]) - 1)) ){
              if( strstr(list_of_files[i], holdpath) ){
                found_flag = 1;
                i = len;
                j = 257;
              }
              else{
                holdpath[j] = fullpath[j];
              }
            }
            else{
              holdpath[j] = fullpath[j];
            }
          }
          memset(holdpath, '\0', 257);
        }
      }
      else{
        if ( strlen(name) >= strlen(list_of_files[i]) ){
          for ( j = 0; j <= strlen(name); j++){
            if( (name[j] == '\0')
              && ( strlen(holdpath) >= (strlen(list_of_files[i]) - 1)) ){
              if( strstr(list_of_files[i], holdpath) ){
                found_flag = 1;
                i = len;
                j = 257;
              }
              else{
                holdpath[j] = name[j];
              }
            }
            else{
              holdpath[j] = name[j];
            }
          }
          memset(holdpath, '\0', 257);
        } 
      }
      memset(holdpath, '\0', 257);
    }
    
    /* Create our new file */
    if ( typeflag == '5' && found_flag == 1){
      /* We have a directory */
      if ( use_fullpath == 1){
        make_full_path(fullpath);
        mkdir(fullpath, file_mode);
      }
      else{
        make_full_path(name);
        mkdir(name, file_mode);
      }
    }
    else if ( typeflag == '2' && found_flag == 1){
      if ( use_fullpath == 1 ){
        make_full_path(fullpath);
        symlink(linkname, fullpath);
      }
      else{
        make_full_path(name);
        symlink(linkname, name);
      }
    }
    
    else if ( typeflag == '0' ){
      if ( use_fullpath == 1 && found_flag == 1){
        make_full_path(fullpath);
        extracted_file = open(fullpath, O_RDWR | O_CREAT | O_TRUNC, S_IRUSR |
                    S_IWUSR);
      }
      else if ( use_fullpath == 0 && found_flag == 1 ){
        make_full_path(name);
        extracted_file = open(name, O_RDWR | O_CREAT | O_TRUNC, S_IRUSR |
                    S_IWUSR);
        
        if ( -1 == stat(name, &currentFile) ){
          printf("stat_error\n");
          break;
        }
      }
      /* Allocate memory for the file contents */
      if (found_flag == 1){
        contents = malloc(file_size);
        if ( !contents ){
          printf("malloc error\n");
          exit(EXIT_FAILURE);
        }
        /* Read in the file contents */
        read(tarfile, contents, file_size);
        /* Write content to our extracted file*/
        write(extracted_file, contents, file_size);
        free(contents);
        close(extracted_file);
      }
      else{
        //printf("SKIPPING over file: %s\n", name);
        lseek(tarfile, file_size, SEEK_CUR);
      }
    
      if ( use_fullpath == 1 && found_flag == 1){
        chmod(fullpath, file_mode);
        chown(fullpath, user, group);
        if ( -1 == stat(fullpath, &currentFile) ){
          printf("stat_error\n");
          break;
        }
        
        file_times.actime = currentFile.st_atime;
        file_times.modtime = file_time;
        
        if ( -1 == utime(fullpath, &file_times) ){
          printf("time error\n");
          break;
        }
      }
      else if ( use_fullpath == 0 && found_flag == 1 ){
        chmod(name, file_mode);
        chown(name, user, group);
        
        if ( -1 == stat(name, &currentFile) ){
          printf("stat_error\n");
          break;
        }
        
        file_times.actime = currentFile.st_atime;
        file_times.modtime = file_time;
        
        if ( -1 == utime(name, &file_times) ){
          printf("time error\n");
          break;
        }
      }
      
      /* DONE WITH FIND */
      
      
      portion_filled = file_size % 512;
      if ( portion_filled != 0 ){
        skip_over = 512 - portion_filled;
        lseek(tarfile, skip_over, SEEK_CUR);
      }
    }
    else
      continue;
    if ( 0 < v_flags ){
      if ( 1 < v_flags ){
        char time_string[30] = {0};
        struct tm *tmp;
        
        tmp = localtime(&mod_time);
        strftime(time_string, 30, "%a %b %e %l:%M:%S %Y", tmp);
        
        printf("reading header:\n");
        printf("  char      name[]: %s\n", name);
        printf("  mode_t      mode: %s\n", mode + 2);
        printf("  uid_t        uid: %d\n", user);
        printf("  gid_t        gid: %d\n", group);
        printf("  size_t      size: %d\n", file_size);
        printf("  time_t     mtime: %d --- %s\n", file_time, time_string);
        printf("  int       chksum: %d\n", checksum);
        printf("  char    typeflag: %c\n", typeflag);
        printf("  char  linkname[]: %s\n", linkname);
        printf("  char     magic[]: %s\n", magic);
        printf("  char   version[]: %s\n", version);
        printf("  char     uname[]: %s\n", uname);
        printf("  char     gname[]: %s\n", gname);
        printf("  int     devmajor: 0\n");
        printf("  int     devminor: 0\n");
        printf("  char    prefix[]: %s\n", prefix);
        printf("Read header\n");
        memset(time_string, '\0', 24);
      }
      if ( use_fullpath == 1 ){
        printf("(file_in_list:211) looking for \"%s\"...looking for \"%s\"...",
            fullpath, fullpath);
        if ( found_flag == 1 )
          printf("FOUND.\n");
        else
          printf("nope.\n");
        
        printf("%s\n", fullpath);
      }
      else{
        printf(
          "(file_in_list:211) looking for \"%s\"...looking for \"%s\"...",
            name, name);
        if ( found_flag == 1 )
          printf("FOUND.\n");
        else
          printf("nope.\n");
        
        printf("%s\n", name);
      }
    }
    
    memset(name, '\0', 101);
    memset(mode, '\0', 8);
    memset(uid, '\0', 8);
    memset(gid, '\0', 8);
    memset(size, '\0', 12);
    memset(mtime, '\0', 12);
    memset(chksum, '\0', 8);
    if ( typeflag == '2' )
      memset(linkname, '\0', 100);
    memset(magic, '\0', 6);
    memset(version, '\0', 2);
    memset(prefix, '\0', 155);
    
    if ( use_fullpath == 1 ){
      memset(fullpath, '\0', 257);
      use_fullpath = 0;
    }
    
    found_flag = 0;
    
  }
}