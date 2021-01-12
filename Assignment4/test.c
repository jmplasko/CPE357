#include <sys/types.h>
#include <sys/stat.h>

int main(){
   mkdir("yoo", S_IRWXU);
}

