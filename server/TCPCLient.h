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
#include "TCPServer.h"
#include "TCPSocket.cpp"


struct TCPServer;

struct Client: TCPObject {

    Client(){}
    Client(const Client &c) = delete;
    Client(Client &c):socket(c.socket),server(c.server),do_on_read(c.do_on_read),do_on_disconnect(c.do_on_disconnect){}
    Client(Client &&c):socket(c.socket),server(c.server),do_on_read(c.do_on_read),do_on_disconnect(c.do_on_disconnect){}

    std::string read();
    void send(std::string message);
    void close();
    virtual int get_fd();
    virtual void event(int);
    void set_read_callback(std::function<void(std::string, Client*)> f) {do_on_read = f;};
    void set_disconnect_callback(std::function<void(Client*)> f) {do_on_disconnect = f;};

private:
    Client(TCPServer* serv, tcp_socket &sock):socket(sock),server(serv) {}
    std::function<void(std::string, Client*)> do_on_read;
    std::function<void(Client*)> do_on_disconnect;
    tcp_socket socket;
    TCPServer* server;
    std::string bufout;
    bool out_data = false;
    friend struct TCPServer;
};


#endif
