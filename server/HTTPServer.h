#ifndef server_HTTPServer_h
#define server_HTTPServer_h

#include "TCPServer.h"
#include "KqueueWrap.h"

struct HTTPClient;

struct HTTPServer {
    HTTPServer(int port);
    void set_read_callback(std::function<void(std::string, HTTPClient*)> f) {do_on_request = f;};
    void set_disconnect_callback(std::function<void(HTTPClient*)> f) {do_on_disconnect = f;};

private:
    Kqueue_wrap kq;
    TCPServer tcp_server;
    std::function<void(HTTPClient*)> do_on_disconnect;
    std::function<void(std::string, HTTPClient*)> do_on_request;
    std::map<int, client> clients;

    friend struct HTTPClient;
    
};



#endif
