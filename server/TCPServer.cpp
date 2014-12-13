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
#include "KqueueWrap.h"

const int BACKLOG = 100;

TCPServer::TCPServer(Kqueue_wrap& kq_i, int port):kq(kq_i)
{

    sockaddr_in serv_addr;
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(port);
    std::string buf;
    socklen_t client_len;
    sockaddr_in client_addr;
    client_len = sizeof(client_addr);


    int yes = 1;
   
    if (setsockopt(listener.fd,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof(int)) == -1) {
        throw tcp_exception("setsockopt");
    }
    if (bind(listener.fd, (sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
    {
        throw tcp_exception("ERROR on binding");
    }
    if(listen(listener.fd,BACKLOG) == -1)
    {
        throw tcp_exception("listen");
    }

    /*kq.set_event(listener.fd, [&](int fd){
        try {
            tcp_socket s_tmp(listener.fd, (sockaddr *) &client_addr, &client_len);
            int tmp_fd = s_tmp.fd;
            client_sockets.insert(std::pair<int, tcp_socket>(s_tmp.fd,s_tmp));

          //  kq.set_event(tmp_fd, read_from);
            do_on_accept(tmp_fd);
        } catch (tcp_exception e) {
            std::cout << e.message << std::endl;
        }
        catch (...)
        {
            std::cout << "user exception" << std::endl;
        }
    });*/


}

void TCPServer::event()
{
    
}

int TCPServer::get_fd()
{
    return listener.fd;
}

TCPServer::~TCPServer()
{
}

void TCPServer::stop()
{
    running = false;
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
            client_sockets.erase(fd);
            do_on_disconnect(fd);
            return message;
        }
        else {
            message.append(buffer);
        }
    } while (nread == 511);

    do_on_read(message,fd);
    return message;
}

void TCPServer::close_client(int fd)
{
    client_sockets.erase(fd);
}

void TCPServer::send_to(int fd, std::string message)
{
    ssize_t n;
    n = send(fd, message.c_str(), message.length(), 0);
    if (n < 0)
        throw tcp_exception("ERROR writing to socket");
}

int TCPServer::connect_to(std::string addr, int port)
{
    sockaddr_in serv_addr;
    hostent *server;

    tcp_socket s_tmp;
    server = gethostbyname(addr.c_str());
    if (server == NULL) {
        std::cout << "ERROR, no such host\n";
    }

    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr,
          (char *)&serv_addr.sin_addr.s_addr,
          server->h_length);
    serv_addr.sin_port = htons(port);

    if (connect(s_tmp.fd,(const sockaddr*)&serv_addr,sizeof(serv_addr)) < 0)
    {
        throw tcp_exception("ERROR connecting");
    }
    int tmp_fd = s_tmp.fd;
    struct kevent ev;
    client_sockets.insert(std::pair<int, tcp_socket>(s_tmp.fd,s_tmp)); //
    EV_SET(&ev,tmp_fd, EVFILT_READ, EV_ADD, 0, 0, 0);
    int kevent_res = kevent(kq.get_fd(), &ev, 1, NULL, 0, NULL);
    if (kevent_res == -1) {
        throw tcp_exception("kevent()");
    }
    return tmp_fd;

}

void TCPServer::run()
{

    running = true;


    struct kevent ev;
    EV_SET(&ev, listener.fd, EVFILT_READ, EV_ADD, 0, 0, 0);
    int kevent_res = kevent(kq.get_fd(), &ev, 1, NULL, 0, NULL);
    if (kevent_res == -1) {
        throw tcp_exception("kevent()");
    }

    


    while (running) {
        memset(&ev, 0, sizeof(ev));
        kevent_res = kevent(kq.get_fd(), NULL, 0, &ev, 1, NULL);
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

            }
            else if(ev.filter == EVFILT_SIGNAL && (ev.ident == SIGINT || ev.ident == SIGTERM))
            {
                std::cout << std::endl << "-_- NO DUDE, NO... " << std::endl;
                return;
            }
            else {
                std::exception_ptr eptr;
                try {
                    read_from(ev.ident);
                } catch (tcp_exception e) {
                    std::cout << e.message << std::endl;
                } catch (...)
                {
                    std::cout << "user exception" << std::endl;
                }

            }
        }
    }
}


