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
#include <sys/stat.h>


int main(int argc, char const *argv[])
{
    int fd = open("./random.txt",O_RDWR, S_IRUSR | S_IWUSR);
    printf("%d\n",fd);
    char* file_content = mmap(NULL,4096,PROT_READ | PROT_WRITE , MAP_SHARED, fd, 0);

    struct stat sb;
    fstat(fd, &sb);
    printf("%d\n",sb.st_ino);
    printf("size is : %d\n",sb.st_size);
    printf("Printing file content: \n");
    if (fork() == 0){
    for (int i = 0; i < sb.st_size; i++)
    {
       printf("%c",*(file_content + i));
    }
    }
    // printf("\n");
    
    return 0;
}
