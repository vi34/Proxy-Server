//
//  TCPClient.cpp
//  server
//
//  Created by Виктор Шатров on 15.12.14.
//  Copyright (c) 2014 Виктор Шатров. All rights reserved222.
//

#include "TCPCLient.h"
const int BUF_SIZE = 1025;

int Client::get_fd()
{
    return socket.fd;
}

void Client::event(int filter)
{
    if (filter) {
        std::string message = "";
        char buffer[BUF_SIZE];
        ssize_t nread;

        do {
            bzero(buffer,BUF_SIZE);
            if ((nread = recv(socket.fd, buffer, BUF_SIZE, 0)) < 0)
            {
                throw tcp_exception("read from socket");
            }
            else if (nread == 0)
            {
                if(message != "")
                {
                    try {
                        do_on_read(std::move(message), this);
                    } catch (tcp_exception e) {
                        std::cout << e.message << std::endl;
                    } catch (...) {
                        std::cout << "caught exception" << std::endl;
                    }
                }
                try {
                    do_on_disconnect(this);
                } catch (tcp_exception e) {
                    std::cout << e.message << std::endl;
                } catch (...) {
                    std::cout << "caught exception" << std::endl;
                }
                if(server->clients.size() == 0) {
                    printf("bad tcp map!\r\n");
                }
                server->clients.erase(socket.fd);
                return;
            }
            else {
               // printf("socket %d: read %d bytes, buffer %d-", socket.fd, nread, message.length());
                message.append(buffer,nread);
                //printf ("%d\n", message.length());
                 //printf("%s\n\n%s", buffer,message.c_str());
            }
        } while (nread == BUF_SIZE);

        try {
            if (message == "") {
                printf("bad message \r\n");
            }
            do_on_read(std::move(message), this);
        } catch (tcp_exception e) {
            std::cout << e.message << std::endl;
        } catch (...) {
            std::cout << "caught exception" << std::endl;
        }
    } else {
        ssize_t n;
        n = ::send(socket.fd, bufout.c_str(), bufout.length(), 0);
        if (n < 0 ) {
            throw tcp_exception("ERROR writing to socket");
        } else {
            bufout = bufout.substr(n);
            if (bufout.length() == 0) {
                out_data = false;
                server->kq.remove_from_watching(this, 0);
            }
        }
    }
}

void Client::send(std::string message)
{
    server->kq.add_to_watch(this, 0);
    out_data = true;
    bufout.append(message);
}

void Client::close()
{
    server->clients.erase(socket.fd);
    server->kq.remove_from_watching(this, 1);
}

