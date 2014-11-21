#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/event.h>
#include <netinet/in.h>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <netdb.h>
#include <vector>
#include <map>
#include "TCPServer.h"

const int BACKLOG = 100;

struct server_exception {
    
};

TCPServer::TCPServer(int port)
{

    sockaddr_in serv_addr;
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(port);
    int yes = 1;
    if ((kq = kqueue()) == -1) {
        throw tcp_exception("kqueue()");
    }
    if (setsockopt(listener.fd,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof(int)) == -1) {
        close(kq);
        throw tcp_exception("setsockopt");
    }
    if (bind(listener.fd, (sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
    {
        close(kq);
        throw tcp_exception("ERROR on binding");
    }
    if(listen(listener.fd,BACKLOG) == -1)
    {
        close(kq);
        throw tcp_exception("listen");
    }


}

TCPServer::~TCPServer()
{
    close(kq);
}

void TCPServer::stop()
{
    running = false;
}

void TCPServer::test()
{
    //std::cout << "WOW" << std::endl;
}

std::string TCPServer::read_from(int fd)
{
    std::string message = "";
    char buffer[512];
    ssize_t nread;
    bzero(buffer,512);
    do {
        if((nread = recv(fd, buffer, 511, 0)) < 0)
        {
            throw tcp_exception("read from socket");
        }
        else if(nread == 0)
        {
            std::cout << "client "<< fd << " disconnected" << std::endl;
            client_sockets.erase(fd);
            return message;
        }
        else {
            message.append(buffer);
        }
    } while (nread == 511);

    doOnRead(message,fd);
    return message;
}

void TCPServer::send_to(int fd, std::string message)
{
    ssize_t n;
    n = send(fd, message.c_str(), message.length(), 0);
    if (n < 0)
        throw tcp_exception("ERROR writing to socket");
}

void TCPServer::run()
{

    running = true;
    std::string buf;
    socklen_t client_len;
    sockaddr_in client_addr;
    client_len = sizeof(client_addr);


    struct kevent ev;
    EV_SET(&ev, listener.fd, EVFILT_READ, EV_ADD, 0, 0, 0);
    int kevent_res = kevent(kq, &ev, 1, NULL, 0, NULL);
    if (kevent_res == -1) {
        throw tcp_exception("kevent()");
        return;
    }

    while (running) {
        memset(&ev, 0, sizeof(ev));
        kevent_res = kevent(kq, NULL, 0, &ev, 1, NULL);
        if(kevent_res < 0)
        {
            if(!running)
                return;
            throw tcp_exception("kevent()");
        }
        if(kevent_res > 0)
        {
                if(ev.ident == listener.fd)
                {
                    tcp_socket s_tmp(listener.fd, (sockaddr *) &client_addr, &client_len);
                    int tmp_fd = s_tmp.fd;
                    client_sockets.insert(std::pair<int, tcp_socket>(s_tmp.fd,s_tmp));
                    EV_SET(&ev,tmp_fd, EVFILT_READ, EV_ADD, 0, 0, 0);
                    kevent_res = kevent(kq, &ev, 1, NULL, 0, NULL);
                    if (kevent_res == -1) {
                        throw tcp_exception("kevent");
                    }
                    doOnAccept();
                    std::cout << "client " << tmp_fd << " connected" << std::endl;
                }
                else {
                    buf = read_from(ev.ident);
                }
        }
    }
}


