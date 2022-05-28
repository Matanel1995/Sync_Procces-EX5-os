#ifdef USE_MAP_ANON
#define _BSD_SOURCE
#endif  
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
int* addr;

void increment (int num){
    for (int i = 0; i < 200; i++)
    {
        (*addr)++;
        printf("process number %d ,counter is: %d\n",num,*addr);
    }
}

int main(int argc, char const *argv[])
{
    int pagesize = getpagesize();
    // int fd = open("./lock.txt",O_RDWR, S_IRUSR | S_IWUSR);
    addr = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);     
    *addr = 0;      /*Initialize integer in mapped region*/    

    if(fork() == 0){
        increment(1);
    }
    else{
        if(fork() == 0){
        increment(2);
        }
    }
    wait(NULL);
    return 0;
}
