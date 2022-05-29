#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include "Stack.c"
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#define MAX_LENGTH 1024
#define PORT "3491"  // the port users will be connecting to
#define BACKLOG 10   // how many pending connections queue will hold
Stack *myStack ;
struct flock lock;
void sigchld_handler(int s)
{
    // waitpid() might overwrite errno, so we save and restore it:
    int saved_errno = errno;
    while(waitpid(-1, NULL, WNOHANG) > 0);

    errno = saved_errno;
}

void getDataFromClients(int  newfd1) {
    while (1) {
        memset(&lock, 0, sizeof(lock));
        //lock.l_type = F_WRLCK;
        // fcntl(newfd1, F_SETLK, &lock);
        char user_input[MAX_LENGTH];
        char user_push[5];
        char rest[MAX_LENGTH];
        char user_pop_top[4];
        memset(user_input, 0, MAX_LENGTH);
        memset(user_pop_top, 0, 4);
        memset(rest, 0, MAX_LENGTH);
        recv(newfd1, user_input, MAX_LENGTH, 0);
        strncpy(user_pop_top, user_input, 3);
        strncpy(user_push, user_input, 4);
        //If got POP from user
        if (strcmp(user_pop_top, "POP") == 0) {
            lock.l_type = F_WRLCK;
            fcntl(newfd1, F_SETLK, &lock);
            pop(&myStack);
            lock.l_type = F_UNLCK;
            fcntl(newfd1, F_SETLKW, &lock);
        }
        //If got PUSH from user
        else if (strcmp(user_push, "PUSH") == 0) {
            lock.l_type = F_WRLCK;
            fcntl(newfd1, F_SETLK, &lock);
            for (int i = 5; i < strlen(user_input); i++) {
                rest[i - 5] = user_input[i];
            }
            push(&myStack, rest);
            lock.l_type = F_UNLCK;
            fcntl(newfd1, F_SETLKW, &lock);
        } 
        //If got TOP from user
        else if (strcmp(user_pop_top, "TOP") == 0) {
            lock.l_type = F_UNLCK;
            fcntl(newfd1, F_SETLKW, &lock);
            send(newfd1, top(&myStack)->data, MAX_LENGTH, 0);
            lock.l_type = F_UNLCK;
            fcntl(newfd1, F_SETLKW, &lock);
        }
        // lock.l_type = F_UNLCK;
        // fcntl(newfd1, F_SETLKW, &lock);
    }
}


// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int main(void)
{
//    while (1)
//    {
//        // main accept() loop
//        sin_size = sizeof their_addr;
//
//        // locking the access to the server
//        struct flock lock;
//        memset(&lock, 0, sizeof(lock));
//        lock.l_type = F_WRLCK;
//        fcntl(sockfd, F_SETLK, &lock);
//        //unlock file discriptor
//        lock.l_type = F_UNLCK;
//        fcntl(sockfd, F_SETLKW, &lock);


    int sockfd, new_fd;  // listen on sock_fd, new connection on new_fd
    struct addrinfo hints, *servinfo, *p;
    myStack = (Stack *)mmap(NULL,1000000,PROT_READ |PROT_WRITE, MAP_ANONYMOUS|MAP_SHARED ,-1,0);
//    myStack = (Stack*)smalloc(sizeof (Stack));
    initializeStack(&myStack);
//    myStack->head->data = "dsadas";

    struct sockaddr_storage their_addr; // connector's address information
    socklen_t sin_size;
    struct sigaction sa;
    int yes=1;
    char s[INET6_ADDRSTRLEN];
    int rv;
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE; // use my IP

    if ((rv = getaddrinfo(NULL, PORT, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    // loop through all the results and bind to the first we can
    for(p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype,
                             p->ai_protocol)) == -1) {
            perror("server: socket");
            continue;
        }

        if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes,
                       sizeof(int)) == -1) {
            perror("setsockopt");
            exit(1);
        }

        if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            perror("server: bind");
            continue;
        }

        break;
    }

    freeaddrinfo(servinfo); // all done with this structure

    if (p == NULL)  {
        fprintf(stderr, "server: failed to bind\n");
        exit(1);
    }

    if (listen(sockfd, BACKLOG) == -1) {
        perror("listen");
        exit(1);
    }

    sa.sa_handler = sigchld_handler; // reap all dead processes
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    if (sigaction(SIGCHLD, &sa, NULL) == -1) {
        perror("sigaction");
        exit(1);
    }

    printf("server: waiting for connections...\n");
    int couneter = 0;

    while(1) {  // main accept() loop
        couneter++;
        sin_size = sizeof their_addr;

        new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
        if (new_fd == -1) {
            perror("accept");
            continue;
        }

        inet_ntop(their_addr.ss_family,
                  get_in_addr((struct sockaddr *)&their_addr),
                  s, sizeof s);
        printf("server: got connection from %s\n", s);
        pid_t pid = fork();
        if( pid <0){
            printf("failed fork");

        }
        else if(pid==0){
            getDataFromClients(new_fd);
            exit(0);
        }
        if(couneter>10){
            break;

        }

    }

    return 0;
}