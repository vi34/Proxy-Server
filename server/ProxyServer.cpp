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
#include "NetworkManager.h"


using namespace std;

long long INF = 10000000000;

int main ()
{
    try {
        Kqueue_wrap kq;
        TCPServer server(kq, 1112);
        string mes;

        bool conn = false;
        server.set_accept_callback([&server, &conn](client* c){
            std::cout << "client " << c->get_fd() << " connected" << std::endl;
           // c->send("Thank you for using vi34 Proxy");

        });

        server.set_read_callback([&server, &mes](std::string message, client* c) {

            int k = message.find("Host: ");
            k += 6;
            int kk = message.find("\r\n",k);
            string host = message.substr(k, kk - k);
            //server.close_client(fd);
            std::cout <<"client " << c->get_fd() << ": " << host << std::endl;

            client* c2 = server.connect_to(host, 80);
            c2->set_read_callback([c](std::string message, client* c2){
                c->send(message);
                cout << message << endl;
            });
            c2->send(message);


            //c->close();
            //int sock = server.connect_to("localhost", 1113);
        });

        server.set_disconnect_callback([&server](client* c){
            std::cout << "client " << c->get_fd() << " disconnected" << std::endl;
        });

/*
        request req;
        req.method = "GET";
        req.host = "www.example.com";
        req.uri = "/";
        req.version = "HTTP/1.1";

        client* c2 = server.connect_to(req.host, 80);
        c2->set_read_callback([](std::string message, client* c2){
            std::cout << message << std::endl;
        });
        c2->send(req.build());
*/

        kq.run();
    } catch (tcp_exception exc) {
        cout << exc.message << endl;
    }
    cout <<"Bye!";

    return 0;
}
