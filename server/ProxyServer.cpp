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
#include "HTTPServer.h"
#include "HTTPClient.h"


using namespace std;

long long INF = 10000000000;
int main ()
{
    
    try {
        HTTPServer server(1112);

        server.set_request_callback([&server](HTTPRequest request, HTTPClient* c){
            server.send_request(request, c, [&request,c](HTTPResponse response) {
                cout << endl << "Got response";
                //cout << endl << response.to_string();
                c->send_response(response);
                cout << ", Sent to " << c->tcp_client->get_fd() <<  endl;
                //c.send_response(response);
            });
        });

        server.run();


        /*
        server.set_accept_callback([&server](client* c){
            std::cout << "client " << c->get_fd() << " connected" << std::endl;
        });

        server.set_read_callback([&server, &mes](std::string message, client* c) {
            int k = message.find("Host: ");
            k += 6;
            int kk = message.find("\r\n",k);
            string host = message.substr(k, kk - k);
            //c->close();
            std::cout <<"client " << c->get_fd() << ": " << host << std::endl;


            string mess = message;
            std::cout << mess << std::endl;


            client* c2 = server.connect_to(host, 80);
            c2->set_read_callback([c](std::string message, client* c2){
                c->send(message);
                cout << message << endl;
            });
            c2->send(message);
        });

        server.set_disconnect_callback([&server](client* c){
            std::cout << "client " << c->get_fd() << " disconnected" << std::endl;
        });
         */

    } catch (tcp_exception exc) {
        cout << exc.message << endl;
    }
    cout <<"Bye!";

    return 0;
}
