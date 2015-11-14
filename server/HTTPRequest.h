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
#include <cstdio>

struct HTTPRequest {
    std::string method;
    std::string uri;
    std::string version;
    std::string host;
    std::string input;
    bool keep_alive = false;

    bool is_correct();
    void parse();
    std::string to_string();
    std::map<std::string,std::string> headers;

private:
    bool headers_parsed = false;
    bool start_line_parsed = false;
};


#endif
