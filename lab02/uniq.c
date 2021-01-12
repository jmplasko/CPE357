#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_long_line(FILE *file){
   char *arrayStart = NULL;
   size_t size = 2;
   int len = 0;
   int flag = 1;
  
   //allocates memory for one character
   arrayStart = (char *) malloc(size);
   

   //finds the first character of the string
   fgets(arrayStart, size, file);
   printf("%p", arrayStart);
   //loop that allows the buffer to be extended while saving the string
   while(flag == 1){
      for(len = 0; len <= strlen(arrayStart); len++){
         if(feof(file) != 0){
            return NULL;
         }//else if(arrayStart[len] == '\n'){
            //for(len = 0; len <= strlen(arrayStart); len++){
            //   fputc(arrayStart[len], stdout); 
            //}
          //  fputs(arrayStart, stdout);
         //   printf("\n");
        //    return arrayStart;
        // }
      }
      
      size = size*2;
      arrayStart = (char *) realloc(arrayStart, size);
      int i = 0;
      char hld;
      for(i = (int)strlen(arrayStart); i < size; i++){
         hld = fgetc(file);
         
         if(hld == '\n'){
            fputc('?', stdout);
            fputc(hld, stdout);
            arrayStart[i] = hld;
            i = size;
            flag = 0;
         }else{
            fputc(hld, stdout);
            arrayStart[i] = hld;
           // fputc(arrayStart[i], stdout);
         }
      }
      
   }
   //fputs(arrayStart, stdout);
   printf("%p", arrayStart);
   return arrayStart;
}


int main(){

   char *p;
   int e;
   int hold;

   //runs through the entire input from stdin
   while((p = read_long_line(stdin)) != NULL){
      fputs(p,stdout);
      printf("%p\n", p);
      free(p);
      printf("%p\n", p);
      //hold = strlen(p);
      //for(e = 0; e<hold; e++){
      //   free(p[e]);
      //}
      fputs(p,stdout);
      printf("h");
   }

   return 0;
}
