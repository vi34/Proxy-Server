#ifndef server_HTTPServer_h
#define server_HTTPServer_h

#include "TCPServer.h"
#include "KqueueWrap.h"
#include "HTTPRequest.h"
#include "HTTPResponse.h"

struct HTTPClient;

struct HTTPServer {
    HTTPServer(int port);

    void run();
    void send_request(HTTPRequest,HTTPClient*, std::function<void(HTTPResponse)>);

    void set_request_callback(std::function<void(HTTPRequest, HTTPClient*)> f) {do_on_request = f;};
    void set_disconnect_callback(std::function<void(HTTPClient*)> f) {do_on_disconnect = f;};
private:
    Kqueue_wrap kq;
    TCPServer tcp_server;
    std::function<void(HTTPRequest, HTTPClient*)> do_on_request;
    std::function<void(HTTPClient*)> do_on_disconnect;
    std::map<int, HTTPClient> clients;

    friend struct HTTPClient;
    
};



#endif
