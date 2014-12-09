


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

#ifndef server_TCPClient_h
#define server_TCPClient_h

const int portnum = 1112;


struct tcp_client{

    tcp_client(std::string addr)
    {
        server = gethostbyname(addr.c_str());
        if (server == NULL) {
            throw tcp_exception("no such host");
        }
        serv_addr.sin_family = AF_INET;
        bcopy((char *)server->h_addr,
              (char *)&serv_addr.sin_addr.s_addr,
              server->h_length);
        serv_addr.sin_port = htons(portnum);

    }
    void connect()
    {
        if (::connect(sock.fd,(const sockaddr*)&serv_addr,sizeof(serv_addr)) < 0)
        {
            std::cout << tcp_exception("ERROR connecting").message;
        }
    }

    void send(std::string message)
    {
        std::string buf;
        int n;
        n = ::send(sock.fd, message.c_str(), message.length(), 0);
        if (n < 0)
        {
            std::cout << tcp_exception("ERROR writing to socket").message;
        }
    }
private:
    tcp_socket sock;
    sockaddr_in serv_addr;
    hostent *server;
};

#endif
