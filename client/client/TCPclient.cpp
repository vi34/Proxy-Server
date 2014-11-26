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
#include "../../server/TCPSocket.cpp"

const int portnum = 1112;

std::string message = "klm";
std::string addr = "localhost";

int main(int argc, char *argv[])
{

    sockaddr_in serv_addr;
    hostent *server;
    std::string buf;
    char buffer[512];

    tcp_socket sock;
    server = gethostbyname(addr.c_str());
    if (server == NULL) {
        std::cout << "ERROR, no such host\n";
        return 0;
    }
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr,
          (char *)&serv_addr.sin_addr.s_addr,
          server->h_length);
    serv_addr.sin_port = htons(portnum);

    if (connect(sock.fd,(const sockaddr*)&serv_addr,sizeof(serv_addr)) < 0)
    {
        std::cout << tcp_exception("ERROR connecting").message;
        return 0;

    }

    for(int i = 0; i < 2; i++)
    {

        std::cout << "Please enter the message: ";
        std::cin >> message;
        int n;
        n = send(sock.fd, message.c_str(), message.length(), 0);
        if (n < 0)
        {
             std::cout << tcp_exception("ERROR writing to socket").message;
            return 0;
        }

    }

    std::string message = "";
    ssize_t nread;
    bzero(buffer,512);
    do {
        if((nread = recv(sock.fd, buffer, 511, 0)) < 0)
        {
            std::cout << tcp_exception("read from socket").message;
            return 0;
        }
        else if(nread == 0)
        {
            std::cout << "client "<< sock.fd << " disconnected" << std::endl;
        }
        else {
            message.append(buffer);
        }
    } while (nread == 511);

    std::cout << message;
    
    return 0;
}