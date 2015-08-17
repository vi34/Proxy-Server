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
#include "KqueueWrap.h"
#include "TCPObject.h"
#include "TCPCLient.h"
#include <map>
#include <vector>

struct Client;

struct TCPServer: TCPObject {
    TCPServer(Kqueue_wrap& kq,int port);

    Client* connect_to(std::string addr, int port);
    virtual void event();
    virtual int get_fd();

    void set_accept_callback(std::function<void(Client*)> f) {do_on_accept = f;};
    void set_read_callback(std::function<void(std::string, Client*)> f) {do_on_read = f;};
    void set_disconnect_callback(std::function<void(Client*)> f) {do_on_disconnect = f;};

private:
    std::function<void(Client*)> do_on_accept;
    std::function<void(Client*)> do_on_disconnect;
    std::function<void(std::string, Client*)> do_on_read;
    tcp_socket listener;
    Kqueue_wrap& kq;
    std::map<int, Client> clients;

    friend struct Client;

};

#endif
