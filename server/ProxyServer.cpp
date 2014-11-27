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

struct usr_exception {
    std::string message;
    usr_exception(std::string s)
    {
message = s + ": " + strerror(errno);
    }
};


int main ()
{
    try {
        TCPServer server(1112);
        serv = &server;

        server.do_on_accept = [&server](int fd){
            std::cout << "client " << fd << " connected" << std::endl;
            server.send_to(fd, "Hello");
        };

        server.do_on_read = [&server](string message, int fd) {

            server.close_client(fd);
            std::cout <<"client " << fd << ": " << message << std::endl;
            throw std::exception();
            //int sock = server.connect_to("localhost", 1113);
            //server.send_to(sock, message);
        };

        server.do_on_disconnect = [&server](int fd){
            std::cout << "client " << fd << " disconnected" << std::endl;

        };

        server.run();
    } catch (tcp_exception exc) {
        cout << exc.message << endl;
    }
    cout <<"Bye!";

    return 0;
}
