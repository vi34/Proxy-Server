//
//  HTTPRequest.cpp
//  server
//
//  Created by Виктор Шатров on 16.12.14.
//  Copyright (c) 2014 Виктор Шатров. All rights reserved.
//

#include <cstdio>
#include "HTTPRequest.h"

void HTTPRequest::parse()
{
    if(!start_line_parsed) {
        method = input.substr(0,input.find(" "));
        input = input.substr(input.find(" ") + 1);
        uri = input.substr(0, input.find(" "));
        input = input.substr(input.find(" ") + 1);
        version = input.substr(0, input.find("\r\n"));
        input = input.substr(input.find("\r\n") + 2);
        start_line_parsed = true;

        if(version == "HTTP/1.1")
            keep_alive = true;
    }
    if(!headers_parsed) {
        while(!headers_parsed) {
            int line = input.find("\r\n");
            if(line == 0) {
                headers_parsed = true;
                break;
            }
            if(line == -1) {//
            }
            std::string current = input.substr(0,line);
            std::string header,value;
            header = current.substr(0,input.find(":"));
            value = current.substr(current.find(":") + 2);
            if (header == "Host") {
                host = value;
            } else if(header == "connection") {
                if(value != "close")
                    keep_alive = true;
                else
                    keep_alive = false;
            }
            input = input.substr(line + 2);
            headers[header] = value;
        }
    }

}

std::string HTTPRequest::to_string()
{
    std::string res = method + " " + uri + " " + version + "\r\n";
    for(std::map<std::string,std::string>::iterator it = headers.begin(); it != headers.end(); ++it)
    {
        res += (*it).first + ": " + (*it).second + "\r\n";
    }
    res += "\r\n";
    return res;
}

