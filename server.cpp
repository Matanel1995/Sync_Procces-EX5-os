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
#include <pthread.h>
#include <iostream>
#include <sys/mman.h>
#include <fcntl.h>
#include "Stack.cpp"

#define PORT "3490"  // the port users will be connecting to

#define MAX_LEN 1024

#define BACKLOG 10   // how many pending connections queue will hold

struct flock lock;

using namespace std;

Stack *mystack;
//Function to send the message to the client (threads!)
// void *send_msg (void* newfd){
//     int newfdfTemp = (int)newfd;
//     send(newfdfTemp,"Hello World from threads",24,0);
// }

void sigchld_handler(int s)
{
    // waitpid() might overwrite errno, so we save and restore it:
    int saved_errno = errno;
    while(waitpid(-1, NULL, WNOHANG) > 0);

    errno = saved_errno;
}

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

void* getDataFromClient(void* mySock){
    // creating new chars[] to check if want to push or pop/top
    char userPush[5];
    char userPopTop[4];
    char usershow[5];
    pthread_mutex_t myLock;
    while(1){
        // init necessary data
        unsigned long int sock = (unsigned long int)mySock;
        char userData[MAX_LEN];
        //cleaning the memory
        memset(userData,0,MAX_LEN);
        memset(userPush,0,5);
        memset(usershow,0,5);
        memset(userPopTop,0,4);
        //recive the operation from the user 
        recv(sock,userData,MAX_LEN,0);
        strncpy(userPopTop , userData,3);
        strncpy(userPush, userData, 4);
        strncpy(usershow, userData, 4);
        //check if i got PUSH from the user
        if(strcmp(userPush,"PUSH") == 0){
            //pthread_mutex_lock(&myLock);
            lock.l_type = F_WRLCK;
            char *temp = (char *)myMalloc::malloc(MAX_LEN);
            memset(temp,0,MAX_LEN);
            //copy the data from the user after the word PUSH
            for(int i=0; i < strlen(userData)-4;i++){
                temp[i] = userData[i+4];
            }
            mystack->push(temp);
            //pthread_mutex_unlock(&myLock);
            lock.l_type = F_UNLCK;
            send(sock," ",MAX_LEN,0);
        }
        // check if i got POP from user
        if(strcmp(userPopTop,"POP") == 0){
            //pthread_mutex_lock(&myLock);
            lock.l_type = F_WRLCK;
            mystack->pop();
            //mystack->printStack();
            //pthread_mutex_unlock(&myLock);
            lock.l_type = F_UNLCK;
            send(sock," ",MAX_LEN,0);
        }
        if(strcmp(userPopTop,"TOP") == 0){
            //pthread_mutex_lock(&myLock);
            lock.l_type = F_WRLCK;
            send(sock,mystack->top().data,MAX_LEN,0);
            //pthread_mutex_unlock(&myLock);
            lock.l_type = F_UNLCK;
        }
        // For my own debug not part of the assingment
        if(strcmp(usershow,"SHOW") == 0){
            mystack->printStack();
            send(sock," ",MAX_LEN,0);
        }
        
    }
}

int main(void)
{
    int client_counter=0;
    int sockfd, new_fd;  // listen on sock_fd, new connection on new_fd
    struct addrinfo hints, *servinfo, *p;
    struct sockaddr_storage their_addr; // connector's address information
    socklen_t sin_size;
    struct sigaction sa;
    int yes=1;
    char s[INET6_ADDRSTRLEN];
    int rv;
    //mystack = (Stack *)malloc(sizeof(Stack));
    mystack = (Stack *)mmap(NULL,1000000,PROT_READ | PROT_WRITE, MAP_ANONYMOUS|MAP_SHARED,-1,0);
    mystack->initStack();
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
    // main accept() loop
    while(1) {
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

        //creating fork
        pid_t pid=fork();
        if(pid < 0){
            perror("Fork failed!");
        }
        else if (pid ==0){
            printf("Created new child! \n");
            getDataFromClient((void *)new_fd);
            exit(0);
        }

        if(client_counter>10){
            break;
        }
    }
    return 0;

}