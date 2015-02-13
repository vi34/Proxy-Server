//
//  HTTPResponse.cpp
//  server
//
//  Created by Виктор Шатров on 10.02.15.
//  Copyright (c) 2015 Виктор Шатров. All rights reserved222.
//

#include "HTTPResponse.h"

bool HTTPResponse::correct() {
    return start_line_parsed && headers_parsed && body_parsed;
}

void HTTPResponse::parse()
{
    //printf("%s",input.c_str());
    if(!start_line_parsed) {
        version = input.substr(0,input.find(" "));
        input = input.substr(input.find(" ") + 1);
        status_code = std::stoi(input.substr(0, input.find(" ")));
        input = input.substr(input.find(" ") + 1);
        reason = input.substr(0, input.find("\r\n"));
        input = input.substr(input.find("\r\n") + 2);
        start_line_parsed = true;
    }
    if(!headers_parsed) {
        while(!headers_parsed) {
            long line = input.find("\r\n");
            if(line == 0) {
                headers_parsed = true;
                break;
            }
            if(line == -1) {//
                printf("strange, line = -1");
            }
            std::string current = input.substr(0,line);
            std::string header,value;
            header = current.substr(0,input.find(":"));
            value = current.substr(current.find(":") + 2);
            input = input.substr(line + 2);
            headers[header] = value;
        }
    }
    if (status_code < 200 || status_code == 204 || status_code == 304)
    {
        body_parsed = true;
    } else {
        //printf("%s",input.c_str());
        if(headers.find("transfer-encoding") != headers.end()) {
            if(headers["transfer-encoding"] == "chunked") {
                if(input.find("\r\n") == 0) { // first chunk
                    input = input.substr(2);
                }
                while(input.length() > 0) {
                    if(!partly_data)
                    {
                        chunk_size = std::stoi(input.substr(0, input.find("\r\n")), nullptr, 16);
                        if(chunk_size == 0) {
                            body_parsed = true;
                            break;
                        }
                        input = input.substr(input.find("\r\n") + 2);
                    }
                    int len = input.length();
                    //printf("%d", len);
                    //int chunk_end = input.find("\r\n");
                    if(len < chunk_size) { // got part of data
                        partly_data = true;
                        break;
                    }
                    body += input.substr(0, chunk_size);
                    input = input.substr(chunk_size + 2);
                    partly_data = false;
                }
            } else {
                // unknown encoding - should return 501 and close connection
            }
        }
        else if(headers.find("Content-Length") != headers.end()) {
            body = input.substr(2);
            input = input.substr(2);
            long len = input.length();
            long cl =  std::stol(headers["Content-Length"]);
            if(len != cl)
                printf("\r\n Content-Length mismatch: %ld - %ld \r\n %s",len,cl, this->to_string().c_str());
            else
                body_parsed = true;
        } else {
            body = input.substr(2);
        }
    }
}

std::string HTTPResponse::to_string()
{
    std::string res = version + " " + std::to_string(status_code) + " " + reason + "\r\n";
    for(std::map<std::string,std::string>::iterator it = headers.begin(); it != headers.end(); ++it)
    {
        res += (*it).first + ": " + (*it).second + "\r\n";
    }
    res += "\r\n";
    res += body;

    return res;
}

std::string HTTPResponse::get_body() {
    return body;
}

void HTTPResponse::clear() {
    input = "";
    body = "";
    version = "";
    reason = "";
    status_code = 0;
    headers.clear();
    body_parsed = false;
    start_line_parsed = false;
    headers_parsed = false;
    partly_data = false;
}