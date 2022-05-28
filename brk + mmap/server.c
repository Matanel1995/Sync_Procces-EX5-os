/************************************************************/
/* This is a stream socket server sample program for UNIX   */
/* domain sockets. This program listens for a connection    */
/* from a client program, accepts it, reads data from the   */
/* client, then sends data back to connected UNIX socket.   */
/************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>

#define SOCK_PATH  "tpf_unix_sock.server"
#define DATA "Hello from server"

int main(void){

    int server_sock, client_sock, len, rc;
    int bytes_rec = 0;
    struct sockaddr_un server_sockaddr;
    struct sockaddr_un client_sockaddr;     
    char buf[256];
    int backlog = 10;
    memset(&server_sockaddr, 0, sizeof(struct sockaddr_un));
    memset(&client_sockaddr, 0, sizeof(struct sockaddr_un));
    memset(buf, 0, 256);                
    
    /**************************************/
    /* Create a UNIX domain stream socket */
    /**************************************/
    server_sock = socket(AF_UNIX, SOCK_STREAM, 0);

    /***************************************/
    /* Set up the UNIX sockaddr structure  */
    /* by using AF_UNIX for the family and */
    /* giving it a filepath to bind to.    */
    /*                                     */
    /* Unlink the file so the bind will    */
    /* succeed, then bind to that file.    */
    /***************************************/
    server_sockaddr.sun_family = AF_UNIX;   
    strcpy(server_sockaddr.sun_path, SOCK_PATH); 
    len = sizeof(server_sockaddr);
    
    unlink(SOCK_PATH);
    rc =    (server_sock, (struct sockaddr *) &server_sockaddr, len);
    
    /*********************************/
    /* Listen for any client sockets */
    /*********************************/
    rc = listen(server_sock, backlog);

    printf("socket listening...\n");
    
    /*********************************/
    /* Accept an incoming connection */
    /*********************************/
    client_sock = accept(server_sock, (struct sockaddr *) &client_sockaddr, &len);
  
    /****************************************/
    /* Get the name of the connected socket */
    /****************************************/
    len = sizeof(client_sockaddr);
    rc = getpeername(client_sock, (struct sockaddr *) &client_sockaddr, &len);
  
    /************************************/
    /* Read and print the data          */
    /* incoming on the connected socket */
    /************************************/
    printf("waiting to read...\n");
    bytes_rec = recv(client_sock, buf, sizeof(buf), 0);
    printf("DATA RECEIVED = %s\n", buf);
    
    /******************************************/
    /* Send data back to the connected socket */
    /******************************************/
    memset(buf, 0, 256);
    strcpy(buf, DATA);      
    printf("Sending data...\n");
    rc = send(client_sock, buf, strlen(buf), 0);
    printf("Data sent!\n");
    
    /******************************/
    /* Close the sockets and exit */
    /******************************/
    close(server_sock);
    close(client_sock);
    
    return 0;
}