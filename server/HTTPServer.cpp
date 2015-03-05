#include "HTTPServer.h"
#include "HTTPClient.h"


HTTPServer::HTTPServer(int port):tcp_server(kq,port)
{
    tcp_server.set_accept_callback([this](client* c){
        HTTPClient h_client(c);
        clients.insert(std::pair<int, HTTPClient>(c->get_fd(),h_client));
        std::cout << std::endl << "new connection " << c->get_fd() << std::endl;
    });

    tcp_server.set_read_callback([this](std::string in, client* c){
        std::cout << "got request from " << c->get_fd();
        HTTPRequest request;
        request.input = in;
        request.parse();
        std::cout << " : " << request.uri << std::endl;
        this->do_on_request(std::move(request),&clients[c->get_fd()]); // may add &&
    });
    tcp_server.set_disconnect_callback([this](client* c) {
        clients.erase(c->get_fd());
        printf("connection %d closed\r\n", c->get_fd());
    });
}

void HTTPServer::run()
{
    kq.run();
}

void HTTPServer::send_request(HTTPRequest request, HTTPClient* http_client, std::function<void (HTTPResponse)> callback)
{
    if(http_client->tcp_remote != nullptr) {// maybe need to check if we have remaining response
        http_client->tcp_remote->close();
        http_client->tcp_remote = nullptr;
    }
    http_client->response.clear();
    http_client->tcp_remote = tcp_server.connect_to(request.host, 80);
    printf("server request(%d) connection: %d\r\n", http_client->tcp_client->get_fd(), http_client->tcp_remote->get_fd());

    http_client->tcp_remote->set_read_callback([callback,http_client](std::string message, client* c){
        if (http_client == nullptr) {
            printf("ERROR: connection closed");
        } else {
            printf("get some info for %d  length: %lu \r\n", http_client->tcp_client->get_fd(),message.length());
           // printf(message.c_str());
            http_client->response.input += message;
            try {
                http_client->response.parse();
            } catch (std::exception e){
                printf("parse exception \r\n%s%s\r\n%s\r\n\r\n",http_client->response.to_string().c_str(),http_client->response.input.c_str(),e.what());
            }
            if(http_client->response.correct()) {
                http_client->wait_response = false;
                callback(http_client->response);
            }
        }
    });
    http_client->tcp_remote->set_disconnect_callback([http_client,callback](client* c){
        if(http_client->wait_response) {
            printf("closing connection... send partly data ");
            callback(http_client->response);
        }
    });

    http_client->tcp_remote->send(request.to_string());
    http_client->wait_response = true;
}