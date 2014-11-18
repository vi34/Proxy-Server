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

const int PORTNUM = 1112;
const int BACKLOG = 100;


TCPServer::TCPServer(int port)
{

    sockaddr_in serv_addr;
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(port);
    int yes = 1;
    if (setsockopt(listener.fd,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof(int)) == -1) {
        perror("setsockopt");
        return;
    }
    if (bind(listener.fd, (sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
    {
        perror("ERROR on binding");
        return;
    }
    if(listen(listener.fd,BACKLOG) == -1)
    {
        perror("listen");
        return;
    }
    if ((kq = kqueue()) == -1) {
        perror("kqueue");
        return;
    }

}

TCPServer::~TCPServer()
{
    close(kq);
}


void TCPServer::run()
{

    char buffer[512];
    std::string buf;
    socklen_t client_len;
    sockaddr_in client_addr;
    client_len = sizeof(client_addr);
    int nread;
    std::map<int, tcp_socket> client_sockets;
    struct kevent ev;
    EV_SET(&ev, listener.fd, EVFILT_READ, EV_ADD, 0, 0, 0);
    int client_num = 0;
    int kevent_res = kevent(kq, &ev, 1, NULL, 0, NULL);
    if (kevent_res == -1) {
        perror("kevent");
        return;
    }
    while (true) {
        memset(&ev, 0, sizeof(ev));
        kevent_res = kevent(kq, NULL, 0, &ev, 1, NULL);
        if(kevent_res < 0)
        {
            perror("kevent()");
            return;
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
                        perror("kevent");
                        return;
                    }
                    std::cout << "client " << tmp_fd << " connected" << std::endl;
                }
                else {
                    bzero(buffer,512);
                    if((nread = recv(ev.ident, buffer, 511, 0)) < 0)
                    {
                        perror("read from socket");
                        return;
                    }
                    else if(nread == 0)
                    {
                        std::cout << "client "<< ev.ident << " disconnected" << std::endl;
                        client_sockets.erase(ev.ident);
                    }
                    else {
                        buf = buffer;
                        std::cout <<"client " << ev.ident << ": " << buf << std::endl;
                    }

                }

        }
        /*
        //std::string s = "I got your ";
        //s += buffer;
        //n = send(nsock.fd,  s.c_str(), 18 + s.length(), 0);
        if (n < 0)
        {
            perror("ERROR writing to socket");
            return 0;
        }
        else if (n == 0)
        {
            //
        }
         */
    }
}