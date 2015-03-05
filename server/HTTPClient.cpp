//
//  HTTPClient.cpp
//  server
//
//  Created by Виктор Шатров on 10.02.15.
//  Copyright (c) 2015 Виктор Шатров. All rights reserved222.
//

#include "HTTPClient.h"

void HTTPClient::send_response(HTTPResponse response)
{
    tcp_client->send(response.to_string());
}

HTTPClient::~HTTPClient()
{
    //if (tcp_client != nullptr) {
      //  tcp_client->close();
    //}
}