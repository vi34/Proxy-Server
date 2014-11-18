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

struct TCPServer {
    TCPServer(int port);

    void run();
    ~TCPServer();

private:
    tcp_socket listener;
    int kq;
};

#endif
