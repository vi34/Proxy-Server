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
#include <signal.h>


using namespace std;

long long INF = 10000000000;


TCPServer* serv;

void signal_handler(int sig)
{
    cout << endl << "-_- NO DUDE, NO... " << endl;
    serv->stop();
}

int main ()
{
    signal(SIGINT, signal_handler);

    int clients[100];
    try {
        TCPServer server(1112);
        serv = &server;
        server.doOnAccept = [&server](){
            //cout << "YABI YABI DOOO!!!" << endl;
            server.test();

        };

        server.doOnRead = [&server, &clients](string message, int fd) {
            std::cout <<"client " << fd << ": " << message << std::endl;
            clients[fd]++;
            if(clients[fd] == 2)
                server.send_to(fd, "OK, bye");
        };


        server.run();
    } catch (tcp_exception exc) {
        cout << exc.message << endl;
    }
    cout <<"Bye!";

    return 0;
}
