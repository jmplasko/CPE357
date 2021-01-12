#include <stdio.h>
#include <hellomake.h>

void myPrintHelloMake(void){
   int c;
   while((c = getchar()) != EOF){
      putchar(c);
      putchar(c);
   }
   return;
}
