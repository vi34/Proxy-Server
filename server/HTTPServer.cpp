#include "HTTPServer.h"
#include "HTTPClient.h"


HTTPServer::HTTPServer(int port):tcp_server(kq,port)
{
    tcp_server.set_accept_callback([this](Client* c){
        HTTPClient h_client(c);
        clients.insert(std::pair<int, HTTPClient>(c->get_fd(),h_client));
        std::cout << std::endl << "new connection " << c->get_fd() << std::endl;
    });

    tcp_server.set_read_callback([this](std::string in, Client* c){
        std::cout << "got request from " << c->get_fd();
        HTTPRequest request;
        request.input = in;
        request.parse();
        std::cout << " : " << request.uri << std::endl;
        this->do_on_request(std::move(request),&clients[c->get_fd()]); // may add &&
    });
    tcp_server.set_disconnect_callback([this](Client* c) {
        clients.erase(c->get_fd());
        printf("connection %d closed\r\n", c->get_fd());
    });
}

void HTTPServer::run()
{
    kq.run();
}

void HTTPServer::send_request(HTTPRequest request, HTTPClient* http_client, std::function<void (HTTPResponse*)> callback)
{

    http_client->keep_alive = request.keep_alive;

    // what to do if we have remaining response?
    http_client->response = new HTTPResponse; // be care about memory leak
    if (http_client->tcp_remote == nullptr || !http_client->keep_alive) {
        http_client->tcp_remote = tcp_server.connect_to(request.host, 80);
    }


    printf("server request(%d) connection: %d\r\n", http_client->tcp_client->get_fd(), http_client->tcp_remote->get_fd());

    http_client->tcp_remote->set_read_callback([callback, http_client, this](std::string message, Client* c){
        if (http_client == nullptr) {
            printf("ERROR: connection closed");
        } else {
            printf("get some info for %d  length: %lu \r\n", http_client->tcp_client->get_fd(),message.length());
           // printf(message.c_str());
            http_client->response->input += message;
            try {
                http_client->response->parse();
            } catch (std::exception e){
                printf("parse exception \r\n%s%s\r\n%s\r\n\r\n",http_client->response->to_string().c_str(),http_client->response->input.c_str(),e.what());
            }
            if(http_client->response->correct()) {
                printf("correct response\n---------------------------\n%s\n-----------------------",
                       http_client->response->print_headers().c_str());
                http_client->wait_response = false;
                callback(http_client->response);
                http_client->response = nullptr;
                if(!http_client->keep_alive) {
                    http_client->tcp_client->close();
                    http_client->tcp_remote->close();
                    clients.erase(http_client->tcp_client->get_fd());
                }

            }
        }
    });

    http_client->tcp_remote->set_disconnect_callback([http_client, callback, this](Client* c){
        if(http_client->wait_response) {
            printf("!!\n%s\n\n", http_client->response->body.c_str());
            http_client->response->body+= http_client->response->input;
            printf("!!\n%s\n\n", http_client->response->body.c_str());
            std::cout << http_client->response->input.length() << std::endl;
            printf("remote closed connection... send partly data\n---------------------------\n%s\n-----------------------",
                   http_client->response->print_headers().c_str());
            callback(http_client->response);
            http_client->response = nullptr;
            if(!http_client->keep_alive) {
                http_client->tcp_client->close();
                clients.erase(http_client->tcp_client->get_fd());
            }
        }
        http_client->tcp_remote = nullptr;
    });

    http_client->tcp_remote->send(request.to_string());
    http_client->wait_response = true;
}