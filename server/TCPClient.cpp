//
//  TCPClient.cpp
//  server
//
//  Created by Виктор Шатров on 15.12.14.
//  Copyright (c) 2014 Виктор Шатров. All rights reserved222.
//

#include "TCPCLient.h"

int client::get_fd()
{
    return socket.fd;
}

void client::event()
{
    std::string message = "";
    char buffer[512];
    ssize_t nread;
    bzero(buffer,512);
    do {
        if((nread = recv(socket.fd, buffer, 511, 0)) < 0)
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
            message.append(buffer);
        }
    } while (nread == 511);

    try {
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
    if (n < 0)
        throw tcp_exception("ERROR writing to socket");
}

void client::close()
{
    server->clients.erase(socket.fd);
}

