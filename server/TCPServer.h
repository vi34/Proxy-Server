//
//  TCPServer.h
//  server
//
//  Created by Виктор Шатров on 18.11.14.
//  Copyright (c) 2014 Виктор Шатров. All rights reserved.
//

#ifndef server_TCPServer_h
#define server_TCPServer_h

#include "TCPSocket.cpp"
#include <map>

struct TCPServer {
    TCPServer(int port);

    void run();
    void stop();
    ~TCPServer();

    std::string read_from(int fd);
    void send_to(int fd, std::string message);
    int connect_to(std::string addr, int port);
    void close_client(int fd);

    std::function<void(int fd)> do_on_accept;
    std::function<void(int fd)> do_on_disconnect;
    std::function<void(std::string s, int fd)> do_on_read;
private:
    bool running;
    tcp_socket listener;
    int kq;
    std::map<int, tcp_socket> client_sockets;

};

#endif
