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

//
int main(int argc, char *argv[])
{
    int pagesize = getpagesize();
    printf("page size is: %d\n",pagesize);
    /*Pointer to shared memory region*/    
    int *addr;   

    addr = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);     
    *addr = 1;      /*Initialize integer in mapped region*/    
    if(fork() == 0){
        (*addr)+=2;   
    }
    else{
        (*addr)++;   
    }
    printf("BEfore creating child the value is: %d\n", *addr);
    switch(fork()) {        /*Parent and child share mapping*/     
    case 0:         /*Child: increment shared integer and exit*/     
        printf("Child started, value = %d\n", *addr);   
        (*addr)+=10; 
             
    default:        /*Parent: wait for child to terminate*/      
        if (wait(NULL) == -1) {    
            fprintf(stderr, "wait() failed\n");    
            exit(EXIT_FAILURE);      
        }     

        printf("In parent, value = %d\n", *addr);         
        if (munmap(addr, sizeof(int)) == -1) {   //deallocates memory given pointer to the start of the allocated page    
            fprintf(stderr, "munmap()() failed\n");      
            exit(EXIT_FAILURE);       
        }      
        if(!fork()){
            printf("%d\n",*addr);
        }  
        exit(EXIT_SUCCESS);
}
}
