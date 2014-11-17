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


std::string message = "GET http://www.example.com/ HTTP/1.1";
std::string addr = "localhost";

int main(int argc, char *argv[])
{
    int n;
    sockaddr_in serv_addr;
    hostent *server;

    std::string buf;
    char buffer[256];

    sock_handle sock;
    //sockfd = socket(AF_INET, SOCK_STREAM, 0);
    //if (sockfd < 0)
      //  error("ERROR opening socket");
    server = gethostbyname(addr.c_str());
    if (server == NULL) {
        std::cout << "ERROR, no such host\n";
        return 0;
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr,
          (char *)&serv_addr.sin_addr.s_addr,
          server->h_length);
    serv_addr.sin_port = htons(portnum);

    if (connect(sock.sockfd,(const sockaddr*)&serv_addr,sizeof(serv_addr)) < 0)
    {
        perror("ERROR connecting");
        return 0;
    }

    for(int i = 0; i < 10; i++)
    {

        std::cout << "Please enter the message: ";
        std::cin >> message;

        n = send(sock.sockfd, message.c_str(), message.length(), 0);
        if (n < 0)
            error("ERROR writing to socket");
        else if (n < strlen(buffer))
        {
            // some data wasn't sent
        }
    }

    bzero(buffer,256);


   // n = recv(sock.sockfd, buffer, 255, 0);
    if (n < 0)
        error("ERROR reading from socket");

    buf = buffer;
    std::cout << buf;
    //close(sockfd);
    return 0;
}