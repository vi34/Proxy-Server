//
//  TCPClient.cpp
//  server
//
//  Created by Виктор Шатров on 15.12.14.
//  Copyright (c) 2014 Виктор Шатров. All rights reserved222.
//

#include "TCPCLient.h"
const int BUF_SIZE = 1025;

int client::get_fd()
{
    return socket.fd;
}

void client::event()
{
    std::vector<char> mess;
    std::string message = "";
    char buffer[BUF_SIZE];
    ssize_t nread;

    do {
        bzero(buffer,BUF_SIZE);
        if((nread = recv(socket.fd, buffer, BUF_SIZE, 0)) < 0)
        {
            throw tcp_exception("read from socket");
        }
        else if(nread == 0)
        {
            if(message != "")
            {
                try {
                  do_on_read(message, this);
                } catch (tcp_exception e) {
                    std::cout << e.message << std::endl;
                } catch (...)
                {
                    std::cout << "caught exception" << std::endl;
                }
            }

            try {
                server->do_on_disconnect(this);
            } catch (tcp_exception e) {
                std::cout << e.message << std::endl;
            } catch (...)
            {
                std::cout << "caught exception" << std::endl;
            }
            server->clients.erase(socket.fd);
            return;
        }
        else {
            message.append(buffer, 0, nread);
        }
    } while (nread == BUF_SIZE);

    try {
        int kk = message.length();
        do_on_read(message, this);
    } catch (tcp_exception e) {
        std::cout << e.message << std::endl;
    } catch (...)
    {
        std::cout << "caught exception" << std::endl;
    }

}

void client::send(std::string message)
{
    ssize_t n;
    n = ::send(socket.fd, message.c_str(), message.length(), 0);
    int k = message.length();
    if (n < 0 )
        throw tcp_exception("ERROR writing to socket");
}

void client::close()
{
    server->clients.erase(socket.fd);
}

