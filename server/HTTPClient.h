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
    HTTPClient() {}
    HTTPClient(const HTTPClient &c) = delete;
    HTTPClient(HTTPClient &c):tcp_client(c.tcp_client) {}
    HTTPClient(HTTPClient &&c):tcp_client(c.tcp_client) {}

    void send_response(HTTPResponse);
    client* tcp_client;
    HTTPResponse response;
};


#endif
