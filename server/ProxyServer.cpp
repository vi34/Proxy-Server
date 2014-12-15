//
//  ProxyServer.cpp
//  server
//
//  Created by Виктор Шатров on 18.11.14.
//  Copyright (c) 2014 Виктор Шатров. All rights reserved.
//
#include <cstring>
#include <cstdio>
#include <iostream>
#include <queue>
#include "TCPServer.h"


using namespace std;

long long INF = 10000000000;

int main ()
{
    try {
        Kqueue_wrap kq;
        TCPServer server(kq, 1112);
        string mes;

        server.set_accept_callback([&server](client* c){
            std::cout << "client " << c->get_fd() << " connected" << std::endl;
            c->send("Hello");
        });

        server.set_read_callback([&server, &mes](std::string message, client* c) {
            mes = message;

            //server.close_client(fd);
            std::cout <<"client " << c->get_fd() << ": " << message << std::endl;
            c->close();
            //int sock = server.connect_to("localhost", 1113);
        });

        server.set_disconnect_callback([&server](client* c){
            server.connect_to("localhost",1113)->send("HI :D");
            std::cout << "client " << c->get_fd() << " disconnected" << std::endl;
        });


        TCPServer server2(kq, 1113);
        server2.set_accept_callback([&server2](client* c){
            std::cout << "server " << c->get_fd() << " connected" << std::endl;
            c->send("HI BRO :D");
        });

        server2.set_read_callback([&server2](std::string message, client* c) {
            std::cout <<"client " << c->get_fd() << ": " << message << std::endl;
        });

        kq.run();
    } catch (tcp_exception exc) {
        cout << exc.message << endl;
    }
    cout <<"Bye!";

    return 0;
}
