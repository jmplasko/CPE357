#include <stdio.h>
#define TABSIZE 8

int main(void)
{
   int c;
   int counter = 0;
   int totalline = 0;
   while((c = getchar()) != EOF){
      if(counter == 8){counter = 0;}
      if(c == '\t'){
         while(counter < 8){
            putchar(' ');
            totalline++;
            counter++;
         }
         counter = 0;
      }else if(c == '\n'){
         putchar(c);
         counter = 0;
         totalline = 0;
      }else if(c == '\r'){
         putchar(c);
         counter = 0;
         totalline = 0;
      }else if(c == '\b'){
         putchar(c);
         if(totalline != 0){
            if(counter == 0){
               counter = 7;
            }else{
               counter--;
            }
            totalline--;
         }
      }else{
         putchar(c);
         counter++;
         totalline++;
      }
   }
   return 0;
}
