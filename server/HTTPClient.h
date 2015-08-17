//
//  HTTPClient.h
//  server
//
//  Created by Виктор Шатров on 09.02.15.
//  Copyright (c) 2015 Виктор Шатров. All rights reserved.
//

#ifndef server_HTTPClient_h
#define server_HTTPClient_h
#include "HTTPServer.h"
struct HTTPServer;

struct HTTPClient {
    HTTPClient(){}
    HTTPClient(Client* tcp_c):tcp_client(tcp_c) {}
    HTTPClient(HTTPClient &c):tcp_client(c.tcp_client) {c.tcp_client = nullptr;}
    HTTPClient(HTTPClient &&c):tcp_client(c.tcp_client) {c.tcp_client = nullptr;}
    HTTPClient(HTTPClient const &c) = delete;
    ~HTTPClient();

    void send_response(HTTPResponse*);

    Client* tcp_client;
    Client* tcp_remote = nullptr;
    HTTPResponse* response = nullptr;
    bool wait_response = false;
    bool keep_alive = false;
};


#endif
