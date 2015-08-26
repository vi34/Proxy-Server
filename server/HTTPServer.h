#ifndef server_HTTPServer_h
#define server_HTTPServer_h

#include "TCPServer.h"
#include "KqueueWrap.h"
#include "HTTPRequest.h"
#include "HTTPResponse.h"
#include <memory>

struct HTTPClient;

struct HTTPServer {
    HTTPServer(int port);

    void run();
    void send_request(std::shared_ptr<HTTPRequest>,HTTPClient*, std::function<void(std::shared_ptr<HTTPResponse>)>);

    void set_request_callback(std::function<void(std::shared_ptr<HTTPRequest>, HTTPClient*)> f) {do_on_request = f;};
    void set_disconnect_callback(std::function<void(HTTPClient*)> f) {do_on_disconnect = f;};
private:
    Kqueue_wrap kq;
    TCPServer tcp_server;
    std::function<void(std::shared_ptr<HTTPRequest>, HTTPClient*)> do_on_request;
    std::function<void(HTTPClient*)> do_on_disconnect;
    std::map<int, HTTPClient> clients;

    friend struct HTTPClient;
    
};



#endif
