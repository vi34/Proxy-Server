#include "HTTPServer.h"
#include "HTTPClient.h"


HTTPServer::HTTPServer(int port):tcp_server(kq,port)
{
    tcp_server.set_accept_callback([this](client* c){
        HTTPClient h_client(c);
        clients.insert(std::pair<int, HTTPClient>(c->get_fd(),h_client));
        std::cout << std::endl << "new connection" << c->get_fd() << std::endl;
    });

    tcp_server.set_read_callback([this](std::string in, client* c){
        std::cout << std::endl << "got request from" << c->get_fd() << ":" << in << std::endl;
        HTTPRequest request;
        request.input = in;
        request.parse();
        this->do_on_request(request,&clients[c->get_fd()]); // may add &&
    });
    tcp_server.set_disconnect_callback([this](client* c) {
        clients.erase(c->get_fd());
        printf("\r\n connection %d closed", c->get_fd());
    });
}

void HTTPServer::run()
{
    kq.run();
}

void HTTPServer::send_request(HTTPRequest request, HTTPClient* http_client, std::function<void (HTTPResponse)> callback)
{
    http_client->response.clear();
    client* connection = tcp_server.connect_to(request.host, 80);
    printf("\r\n server request connection: %d",connection->get_fd());
    connection->set_read_callback([callback,http_client](std::string message, client* c){
        if (http_client == nullptr) {
            printf("ERROR: connection closed");
        } else {
            printf("\r\n get some info for %d  length: %lu \r\n", http_client->tcp_client->get_fd(),message.length());
            http_client->response.input += message;
            try {
                http_client->response.parse();
            } catch (...){
                printf("parse exception \r\n");
            }
            if(http_client->response.correct())
                callback(http_client->response);
        }
    });
    connection->send(request.to_string());

}