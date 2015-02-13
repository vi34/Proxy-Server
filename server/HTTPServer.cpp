#include "HTTPServer.h"
#include "HTTPClient.h"


HTTPServer::HTTPServer(int port):tcp_server(kq,port)
{
    tcp_server.set_accept_callback([this](client* c){
        HTTPClient client;
    });

    tcp_server.set_read_callback([this](std::string in, client* c){
        std::cout << "connection" << c->get_fd() << ":" << in << std::endl;
        HTTPRequest request;
        request.input = in;
        request.parse();
        HTTPClient client;
        client.tcp_client = c;
        clients.insert(std::pair<int, HTTPClient>(c->get_fd(),client));
        this->do_on_request(request,&clients[c->get_fd()]); // may add &&
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
    connection->set_read_callback([callback,http_client](std::string message, client* c){
        http_client->response.input += message;
        try {
            http_client->response.parse();
        } catch (...){
            printf("parse exception \r\n");
        }
        if(http_client->response.correct())
            callback(http_client->response);
    });
    connection->send(request.to_string());

}