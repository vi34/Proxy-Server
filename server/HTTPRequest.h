//
//  HTTPRequest.h
//  server
//
//  Created by Виктор Шатров on 09.02.15.
//  Copyright (c) 2015 Виктор Шатров. All rights reserved.
//

#ifndef server_HTTPRequest_h
#define server_HTTPRequest_h
#include <map>
#include <string>
struct HTTPRequest {
    std::string method;
    std::string uri;
    std::string version;
    std::string host;
    std::string input;
    bool is_correct();
    void parse();
    //"GET / HTTP/1.1\r\nHost: www.example.com\r\n\r\n"
    std::string to_string();

private:
    bool headers_parsed = false;
    bool start_line_parsed = false;
    std::map<std::string,std::string> headers;
};


#endif
