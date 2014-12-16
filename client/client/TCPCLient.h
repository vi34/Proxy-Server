//
//  TCPCLient.h
//  server
//
//  Created by Виктор Шатров on 15.12.14.
//  Copyright (c) 2014 Виктор Шатров. All rights reserved.
//

#ifndef server_TCPCLient_h
#define server_TCPCLient_h
#include "TCPObject.h"
#include "TCPSocket.cpp"
#include "TCPServer.h"

struct TCPServer;

struct client: TCPObject {

    client(){}
    client(const client &c) = delete;
    client(client &c):socket(c.socket),server(c.server){do_on_read = c.do_on_read;}
    client(client &&c):socket(c.socket),server(c.server){do_on_read = c.do_on_read;}

    std::string read();
    void send(std::string message);
    void close();
    virtual int get_fd();
    virtual void event();
    void set_read_callback(std::function<void(std::string, client*)> f) {do_on_read = f;};

private:
    client(TCPServer* serv, tcp_socket &sock):socket(sock),server(serv) {}

    std::function<void(std::string, client*)> do_on_read;
    tcp_socket socket;
    TCPServer* server;
    friend struct TCPServer;
};


#endif
