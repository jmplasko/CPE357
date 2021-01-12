#include <stdio.h>
int main(){

FILE *out;

out = fopen("noreadline.txt", "w");
fprintf(out, "10000111110011001100001");
//fprintf(out, "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ");
fclose(out);
return 0;
}
