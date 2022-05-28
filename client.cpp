/*
** client.c -- a stream socket client demo
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <iostream>

#define PORT "3490" // the port client will be connecting to

#define MAX_LEN 1024

#define MAXDATASIZE 100 // max number of bytes we can get at once

using namespace std;
// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

void* sendFunc(void* mySock){
    unsigned long int sock = (unsigned long int)mySock;
    char serverAns[MAX_LEN];
    while(1){
        char userData[MAX_LEN];
        //get data from user
        memset(userData,0,MAX_LEN);
        fgets(userData,MAX_LEN,stdin);
        //send to the server the data recived from user
        send(sock,userData,MAX_LEN,0);
        //recive feedback from server
        recv(sock,serverAns,MAX_LEN,0);
        if(strcmp(serverAns, " ") == 0){
            continue;
        }
        else{
            cout <<"OUTPUT: " <<serverAns << endl;
        }
    }
}

int main(int argc, char *argv[])
{
    int sockfd, numbytes;  
    char buf[MAXDATASIZE];
    struct addrinfo hints, *servinfo, *p;
    int rv;
    char s[INET6_ADDRSTRLEN];

    if (argc != 2) {
        fprintf(stderr,"usage: client hostname\n");
        exit(1);
    }

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    if ((rv = getaddrinfo(argv[1], PORT, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    // loop through all the results and connect to the first we can
    for(p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype,
                p->ai_protocol)) == -1) {
            perror("client: socket");
            continue;
        }

        if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            perror("client: connect");
            continue;
        }

        break;
    }

    if (p == NULL) {
        fprintf(stderr, "client: failed to connect\n");
        return 2;
    }

    inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr),
            s, sizeof s);
    printf("client: connecting to %s\n", s);

    freeaddrinfo(servinfo); // all done with this structure

    // if ((numbytes = recv(sockfd, buf, MAXDATASIZE-1, 0)) == -1) {
    //     perror("recv");
    //     exit(1);
    // }

    //buf[numbytes] = '\0';

    //creating new thread to recive messages
    pthread_t sendThread;
    pthread_create(&sendThread,NULL, sendFunc,(void *)sockfd);
    pthread_join(sendThread,NULL);
    printf("client: received '%s'\n",buf);

    // close(sockfd);

    return 0;
}