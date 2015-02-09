#include "HTTPServer.h"


HTTPServer::HTTPServer(int port):tcp_server(kq,port) {
    tcp_server.set_accept_callback([this](client* c){
    });
}

