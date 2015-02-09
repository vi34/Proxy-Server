//
//  HTTPRequest.cpp
//  server
//
//  Created by Виктор Шатров on 16.12.14.
//  Copyright (c) 2014 Виктор Шатров. All rights reserved.
//

#include <cstdio>
#include <string>

struct request {
    const std::string VERSION = "1.1";
    std::string method;
    std::string uri;
    std::string version;
    std::string host;
//"GET / HTTP/1.1\r\nHost: www.example.com\r\n\r\n"
    std::string build()
    {
        std::string res;
        res = method + " " + uri + " " + version + "\r\nHost: " + host + "\r\n\r\n";

        return res;
    }

private:


};
