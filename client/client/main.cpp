#include <cstdio>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <iostream>


const int portnum = 1112;

void error(const char *msg)
{
    perror(msg);
    //exit(0);
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
    int sockfd, n;
    sockaddr_in serv_addr;
    hostent *server;

    char buffer[256];
    /*
    if (argc < 3) {
        std::cout << "usage %s hostname port" << std::endl;
        return 0;
    }
     */

    sock_handle sock;
    //sockfd = socket(AF_INET, SOCK_STREAM, 0);
    //if (sockfd < 0)
      //  error("ERROR opening socket");
    server = gethostbyname(argv[1]);
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr,
          (char *)&serv_addr.sin_addr.s_addr,
          server->h_length);
    serv_addr.sin_port = htons(portnum);
    if (connect(sock.sockfd,(const sockaddr*)&serv_addr,sizeof(serv_addr)) < 0)
    {
        error("ERROR connecting");
        return 0;
    }
    for(int i = 0; i < 3; i++)
    {
        printf("Please enter the message: ");
        bzero(buffer,256);
        fgets(buffer,255,stdin);
        n = write(sock.sockfd,buffer,strlen(buffer));
    }
    if (n < 0)
        error("ERROR writing to socket");
    bzero(buffer,256);
    n = read(sock.sockfd,buffer,255);
    if (n < 0)
        error("ERROR reading from socket");
    printf("%s\n",buffer);
    //close(sockfd);
    return 0;
}