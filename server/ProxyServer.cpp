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

TCPServer* serv;

int main ()
{
    try {
        TCPServer server(1112);
        serv = &server;

        server.doOnAccept = [&server](int fd){
            std::cout << "client " << fd << " connected" << std::endl;
            server.send_to(fd, "Hello");
        };

        server.doOnRead = [&server](string message, int fd) {
            std::cout <<"client " << fd << ": " << message << std::endl;
            int sock = server.connect_to("localhost", 1113);
            server.send_to(sock, message);
        };

        server.doOnDisconnect = [&server](int fd){
            std::cout << "client " << fd << " disconnected" << std::endl;

        };

        server.run();
    } catch (tcp_exception exc) {
        cout << exc.message << endl;
    }
    cout <<"Bye!";

    return 0;
}
