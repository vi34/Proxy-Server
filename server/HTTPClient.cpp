//
//  HTTPClient.cpp
//  server
//
//  Created by Виктор Шатров on 10.02.15.
//  Copyright (c) 2015 Виктор Шатров. All rights reserved.
//

#include <iomanip>
#include <iostream>
#include <sstream>

#include "HTTPClient.h"

template< typename T >
std::string int_to_hex( T i )
{
    std::stringstream stream;
    stream << std::hex << i;
    return stream.str();
}

void HTTPClient::send_response(std::shared_ptr<HTTPResponse> response)
{
    if(response->encoding == CHUNKED){
        tcp_client->send(response->print_headers());
        int chunk_size = 100;
        for (int i = 0; i < response->body.length() / chunk_size; ++i)
        {
            tcp_client->send(int_to_hex(chunk_size) + "\r\n" + response->body.substr(chunk_size * i, chunk_size) + "\r\n");
        }
        int remainder = response->body.length() % chunk_size;
        tcp_client->send(int_to_hex(remainder) + "\r\n"
                         + response->body.substr(response->body.length() - (remainder), remainder) + "\r\n");
        if (remainder) {
            tcp_client->send(int_to_hex(0) + "\r\n\r\n");
        }
    } else {
        tcp_client->send(response->to_string());
    }
}



HTTPClient::~HTTPClient()
{
    if(tcp_remote != nullptr)
        tcp_remote->close();
}