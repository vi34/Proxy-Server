/* A simple server in the internet domain using TCP
 The port number is passed as an argument */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <cstdlib>
#include <cstring>
#include <unistd.h>


const int portnum = 1112;

void error(const char *msg)
{
    perror(msg);
    exit(1);
}

struct sock_handle
{
    int sockfd;
    sock_handle()
    {
        sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if (sockfd < 0)
            error("ERROR opening socket");
    }

    ~sock_handle()
    {
        close(sockfd);
    }
};


int main(int argc, char *argv[])
{
    int sockfd, newsockfd;
    socklen_t clilen;
    char buffer[256];
    struct sockaddr_in serv_addr, cli_addr;
    int n;



    sock_handle sock;

    int yes=1;
    // lose "Address already in use" error message
    if (setsockopt(sock.sockfd,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof(int)) == -1) {
        perror("setsockopt");
        exit(1);
    }

    bzero((char *) &serv_addr, sizeof(serv_addr));
    //portnum = atoi(argv[1]);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portnum);
    if (bind(sock.sockfd, (struct sockaddr *) &serv_addr,
             sizeof(serv_addr)) < 0)
    {
        error("ERROR on binding");
    }



    listen(sock.sockfd,5);
    clilen = sizeof(cli_addr);
    newsockfd = accept(sock.sockfd,
                       (struct sockaddr *) &cli_addr,
                       &clilen);
    if (newsockfd < 0)
    {
        error("ERROR on accept");
    }

    bzero(buffer,256);
    for(int i = 0; i < 3; i++)
    {
        n = read(newsockfd,buffer,255);
        if (n < 0)
        {
            close(newsockfd);
            error("ERROR reading from socket");
        }
        printf("Here is the message: %s\n",buffer);
    }

    n = write(newsockfd,"I got your message",18);
    if (n < 0) error("ERROR writing to socket");
    close(newsockfd);

    return 0;
}