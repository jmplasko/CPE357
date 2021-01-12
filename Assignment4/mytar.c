#include "extract.h"

int main(int argc, char *argv[]){
  int options_length, path_length, i;
  int out_tarfile, in_tarfile;
  //char path[PATH_MAX];
  /* Flags  */
  int create_flag = 0;
  int table_flag = 0;
  int extract_flag = 0;
  int verbosity_flag = 0;
  int strict_flag = 0;
  int f_flag = 0;
  int invalid_flag = 0;
  int preserve_flag = 0;
  DIR *dir;
  struct dirent *all;
  int j;
  char finalbuff[1024] = {0};
    
  options_length = strlen(argv[1]);
  for ( i = 0; i < options_length; i++ ){
    if ( 'c' == argv[1][i] )
      create_flag++;
    
    else if ( 't' == argv[1][i] )
      table_flag++;
    
    else if ( 'x' == argv[1][i] )
      extract_flag++;
    
    else if ( 'v' == argv[1][i] )
      verbosity_flag++;
    
    else if ( 'S' == argv[1][i] )
      strict_flag = 1;
    
    else if ( 'f' == argv[1][i] )
      f_flag++;

    else if ( 'p' == argv[1][i] )
      preserve_flag++;
    
    else{
      printf("unrecognized option '%c'.\n", argv[1][i]);
      invalid_flag = 1;
    }
  }
  
  if ( 1 != create_flag + table_flag + extract_flag || 0 != invalid_flag ){
    if ( 0 == create_flag + table_flag + extract_flag )
      printf("you must choose one of the 'ctx' options\n");
    else if ( 1 < create_flag + table_flag + extract_flag )
      printf("you may only choose one of the 'ctx' options.\n");
    if (3 <= argc){
      out_tarfile = open(argv[2], O_RDWR | O_CREAT | O_TRUNC, S_IRUSR |
                    S_IWUSR | S_IRGRP | S_IROTH );
      write(out_tarfile, finalbuff, 1024);
    }
    printf("usage: mytar [ctxvS]f tarfile [ path [...] ]\n");
    exit(2);
  }
  
  if (argc < 3 ){
    printf("usage: mytar [ctxvS]f tarfile [ path [...] ]\n");
    exit(1); /*INCORRECT ARGUMENTS*/
  }
  /*PROCESS v FLAGS if more than 1, check if there is an f flag in there*/
  if ( 1 < verbosity_flag ){
      printf("opt_verbose  = %d\n", verbosity_flag );
      printf("opt_strict   = %d\n", strict_flag);

      if(create_flag != 0)
         printf("opt_mode     = 3\n", create_flag);
      else if(extract_flag != 0)
         printf("opt_mode     = 2\n", extract_flag);
      else
         printf("opt_mode     = 1\n", table_flag);

      printf("opt_preserve = %d\n", preserve_flag);

      if(f_flag == 0)
         printf("opt_tarfile  = (null)\n");
      else
         printf("opt_tarfile  = %s\n", argv[2]);
  }
  /* ok so we have 0 or 1 v flag now check if f flag is 0 */
  if ((0 == f_flag) && (create_flag == 1)){
    printf("Bad address\n");
    printf("write: Bad file descriptor\n");
    printf("Bad file descriptor\n");
    out_tarfile = open(argv[2], O_RDWR | O_CREAT | O_TRUNC, S_IRUSR |
                    S_IWUSR | S_IRGRP | S_IROTH );
    write(out_tarfile, finalbuff, 1024);
    exit(3);
  }else if(0 == f_flag){
    printf("Bad address\n");
    out_tarfile = open(argv[2], O_RDWR | O_CREAT | O_TRUNC, S_IRUSR |
                    S_IWUSR | S_IRGRP | S_IROTH );
    write(out_tarfile, finalbuff, 1024);
    exit(3);
  }
  
  /* JUMP IN AND HANDLE OUR CASES */
  if ( 1 == create_flag ){
    /* Create our tarfile */
    out_tarfile = open(argv[2], O_RDWR | O_CREAT | O_TRUNC, S_IRUSR |
                    S_IWUSR | S_IRGRP | S_IROTH );
    
    /* Error handling */
    if ( -1 == out_tarfile ){
      perror(argv[2]);
      exit(EXIT_FAILURE);
    }
   
    if(argc < 4){
	dir = opendir(".");
        if(dir == NULL){
           perror("Cannot open .");
           exit(0);
        }
        while((all = readdir(dir)) != NULL){
           if(strcmp(all->d_name, ".") && strcmp(all->d_name, "..")
             && strcmp(all->d_name, argv[2])){
              create_header(all->d_name, out_tarfile, 0, verbosity_flag, strict_flag);
           }
        }
    }else{
       for(j = 3; j < argc; j++){
          create_header(argv[j], out_tarfile, 0, verbosity_flag, strict_flag);
       }
    }
    
    write(out_tarfile, finalbuff, 1024); 
    /* Close our file */
    close(out_tarfile);
  }
  
  if ( 1 == table_flag ){
    in_tarfile = open(argv[2], O_RDONLY);
    
    if ( -1 == in_tarfile ){
      perror(argv[2]);
      exit(EXIT_FAILURE);
    }
    
    
    if((verbosity_flag == 0) && (argc == 3)){
       print_table(in_tarfile, strict_flag);
    }else if((verbosity_flag == 0) && (argc > 3)){
       char all_arg[argc-3][256];
       for(i = 3; i < argc; i++){
          if(strlen(argv[i]) <= 256){
             strcpy(all_arg[i-3], argv[i]);
             //printf("%s ", all_arg[i]);
             //printf("%d, %d", i, argc);
          }
       }
       //printf("%s ", all_arg[0]);
       //printf("%s ", all_arg[1]);
       print_table_find(in_tarfile, all_arg, argc-3, strict_flag);
    }else if((verbosity_flag >= 1) && (argc == 3)){
       print_table_v(in_tarfile, verbosity_flag, strict_flag);
    }else{
       char all_arg[argc-3][256];
       for(i = 3; i < argc; i++){
          if(strlen(argv[i]) <= 256){
             strcpy(all_arg[i-3], argv[i]);
             //printf("%s ", all_arg[i]);
             //printf("%d, %d", i, argc);
          }
       }
       //printf("%s ", all_arg[0]);
       //printf("%s ", all_arg[1]);
       print_table_find_v(in_tarfile, verbosity_flag, all_arg, argc-3, strict_flag);
    }
    close(in_tarfile);
  }
  
  if ( 1 == extract_flag ){
    in_tarfile = open(argv[2], O_RDONLY);
    
    if ( -1 == in_tarfile ){
      perror(argv[2]);
      exit(EXIT_FAILURE);
    }
    if ( argc == 3){    
      extract(in_tarfile, verbosity_flag, strict_flag);
    }
    else{
      char all_arg[argc-3][256];
      for ( i = 3; i < argc; i++ ){
        if( strlen(argv[i]) <= 256 ){
          strcpy(all_arg[i-3], argv[i]);
        }
      }
      
      extract_find(in_tarfile, verbosity_flag, all_arg, argc-3, strict_flag);
    }
    close(in_tarfile);
  }
  
  return 0;
}
