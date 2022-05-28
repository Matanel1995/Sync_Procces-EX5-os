#include <stdio.h>
#include <unistd.h>
int main(){
    void* ProgramBreak1 = sbrk(1);
    void* ProgramBreak2 = sbrk(0);
    // pid = fork()
    // if pid == 0{
        // execvp()
    // }
    printf("ProgramBreak: %p\n",ProgramBreak1);
    printf("ProgramBreak: %p\n",ProgramBreak2);
    // int a[5];
    // printf("%d\n",a[15000]);

    return 0;
}