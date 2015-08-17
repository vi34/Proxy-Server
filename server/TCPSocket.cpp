//
//  TCPSocket.cpp
//  server
//
//  Created by Виктор Шатров on 18.11.14.
//  Copyright (c) 2014 Виктор Шатров. All rights reserved222.
//

#ifndef server_TCPSocket_h
#define server_TCPSocket_h


#include <cstring>
#include <cstdio>
#include <sys/types.h>
#include <sys/socket.h>
#include <string>
#include <unistd.h>
#include <iostream>
#include <fcntl.h>

struct tcp_exception {
    std::string message;
    tcp_exception(std::string s)
    {
        message = s + ": " + strerror(errno);
    }
};

struct tcp_socket
{
    int fd;
    tcp_socket()
    {
        fd = socket(AF_INET, SOCK_STREAM, 0);

        if (fd < 0)
            throw tcp_exception("ERROR opening socket:");
    }

    tcp_socket(int l_fd, sockaddr* client_addr, socklen_t* client_len)
    {
        fd = accept(l_fd, client_addr, client_len);
        if (fd < 0)
        {
            throw tcp_exception("ERROR on accept");
        }
        //
        //fcntl(fd, F_SETFL, O_NONBLOCK);
    }

    tcp_socket(tcp_socket &&s)
    {
        fd = s.fd;
        s.fd = -1;
    }

    tcp_socket(tcp_socket const& s) = delete;

    tcp_socket(tcp_socket &s)
    {
        fd = s.fd;
        s.fd = -1;
    }

    ~tcp_socket()
    {
        if(fd > 0)
            printf("TCP: closed %d\r\n",fd);
        close(fd);
    }
};

#endif