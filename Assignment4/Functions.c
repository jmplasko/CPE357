#include "Functions.h"

int insert_special_int(char *where, size_t size, int32_t val){
    int err=0;

    if ( val < 0 || ( size < sizeof( val ) ) ){
        err++;
    }else{
        memset(where,0,size);
        *(int32_t *)(where+size-sizeof(val)) = htonl(val);
        *where |= 0x80;
    }

    return err;
}

uint32_t extract_special_int(char *where, int len){
  int32_t val = -1;
  if ( (len >= sizeof(val) ) && (where[0] & 0x80) ){
    val = *(int32_t *)(where+len-sizeof(val));
    val = ntohl(val);
  }
  
  return val;
}

int make_full_path(char *path){
  char temp[255] = {0};
  int done = 0, slash = 0;
  struct stat sb;
  int i, len;
  
  len = strlen(path);
  
  for ( i = 0; i < len; i++ ){
    if ( path[i] == '/')
      slash++;
  }
  
  if ( slash == 0 )
    return -1; /* No Directories */
  
  while ( done == 0 ){
    for ( i = 0; i < len; i++ ){
      if ( slash == 0 )
        done = 1;
      
      else{
        if ( path[i] != '/')
          temp[i] = path[i];
        else{
          temp[i] = path[i];
          slash--;
          
          if ( 0 == stat(temp, &sb) && S_ISDIR(sb.st_mode) )
            continue;
          else{
            if( mkdir(temp, S_IRWXU) != 0 ){
              printf("error making directory");
              exit(5);
            }
          }
        }
      }
    }
  }
  
  return 0; /* Directories now exist */
}
