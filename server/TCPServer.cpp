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

    do_on_disconnect = [](client*){};
    do_on_accept = [](client*){};
    do_on_read = [](std::string, client*){};
    kq.add_to_watch(this);

}


void TCPServer::event()
{
    socklen_t client_len;
    sockaddr_in client_addr;
    try {
        tcp_socket s_tmp(listener.fd, (sockaddr *) &client_addr, &client_len);
        int tmp_fd = s_tmp.fd;
        client t(this,s_tmp);
        t.set_read_callback(do_on_read);
        clients.insert(std::pair<int, client>(tmp_fd,t));

        kq.add_to_watch(&clients[tmp_fd]);
        do_on_accept(&clients[tmp_fd]);

    } catch (tcp_exception e) {
        std::cout << e.message << std::endl;
    }
    catch (...)
    {
        std::cout << "user exception" << std::endl;
    }
}

int TCPServer::get_fd()
{
    return listener.fd;
}

void TCPServer::close_client(int fd)
{
    clients.erase(fd);
}

client* TCPServer::connect_to(std::string addr, int port)
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
    client t(this,s_tmp);
    t.set_read_callback(do_on_read);
    clients.insert(std::pair<int, client>(tmp_fd,t));
    kq.add_to_watch(&clients[tmp_fd]);

    return &clients[tmp_fd];

}


