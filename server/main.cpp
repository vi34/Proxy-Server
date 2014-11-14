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
#include <iostream>

const int portnum = 1112;

void error(const char *msg)
{
    perror(msg);
    //exit(1);
}

struct sock_handle
{
    int sockfd;
    sock_handle()
    {
        sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if (sockfd < 0)
            perror("ERROR opening socket");
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
    sockaddr_in serv_addr, cli_addr;
    int n;

    sock_handle sock;

    int yes=1;
     //lose "Address already in use" error message
    if (setsockopt(sock.sockfd,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof(int)) == -1) {
        perror("setsockopt");
        return 0;
    }

    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portnum);

    if (bind(sock.sockfd, (sockaddr *) &serv_addr,
             sizeof(serv_addr)) < 0)
    {
        perror("ERROR on binding");
        return 0;
    }

    listen(sock.sockfd,5);
    clilen = sizeof(cli_addr);
    newsockfd = accept(sock.sockfd,(sockaddr *) &cli_addr, &clilen);
    if (newsockfd < 0)
    {
        perror("ERROR on accept");
        return 0;
    }

    bzero(buffer,256);
    for(int i = 0; i < 3; i++)
    {
        n = recv(newsockfd, buffer, strlen(buffer), 0);
        if (n < 0)
        {
            close(newsockfd);
            perror("ERROR reading from socket");
            return 0;
        }
        std::cout << "Here is the message: " << buffer << std::endl;
    }

    n = send(newsockfd, "I got your message", 18, 0);
    if (n < 0)
    {
        perror("ERROR writing to socket");
        return 0;
    }
    else if (n == 0)
    {
        //
    }
    close(newsockfd);

    return 0;
}