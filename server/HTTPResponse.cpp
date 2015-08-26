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
            if(line == -1) { //negotiate this response
                printf("strange, line = -1");
                body_parsed = true;
                return;
            }
            std::string current = input.substr(0,line);//check for keep-alive
            std::string header,value;
            header = current.substr(0,input.find(":"));
            value = current.substr(current.find(":") + 2);
            std::transform(header.begin(), header.end(), header.begin(), ::tolower);  // Transfer <-> transfer
            std::transform(value.begin(), value.end(), value.begin(), ::tolower);
            if(header == "connection") {
                printf("connection: %s\r\n",value.c_str());
            } else if (header == "transfer-encoding") {
                if(value == "chunked") {
                    encoding = CHUNKED;
                    printf("chunked connection\n");
                } else {
                    encoding = IDENTITY;
                }
            } else if (header == "content-length" && encoding != CHUNKED) {
                encoding = CONTENT_LENGTH;
            }
            input = input.substr(line + 2);
            headers[header] = value;
        }
    }
    if (status_code < 200 || status_code == 204 || status_code == 304)
    {
        body_parsed = true;
    } else {
        //  //tools.ietf.org/html/rfc2616#section-4.4 -- chunked
        // //www.mysterylife.ru/navodneniya/pensilvaniya
        //printf("%s",input.c_str());
        if(encoding == CHUNKED) {
            if(input.find("\r\n") == 0) { // first chunk
                input = input.substr(2);
            }
            while(input.length() > 0) {
                if(!partly_data)
                {
                    std::string num = input.substr(0, input.find("\r\n"));
                    try {
                        chunk_size = std::stoul(num, nullptr, 16);
                    } catch (...) {
                        printf("stoi exception %s\r\n\r\n", num.c_str());
                    }
                    if(chunk_size == 0) {
                        body_parsed = true;
                        break;
                    }
                    input = input.substr(input.find("\r\n") + 2);
                }
                long len = input.length();
                //printf("%d", len);
                //int chunk_end = input.find("\r\n");
                if(len < chunk_size) { // got part of data
                    //printf("waiting for another %ld\r\n",chunk_size - len);
                    partly_data = true;
                    break;
                }
                body += input.substr(0, chunk_size);
                input = input.substr(chunk_size + 2);
                partly_data = false;
            }
        }
        else if(encoding == CONTENT_LENGTH) {
            if(!partly_data) {
                input = input.substr(2);
            }
            long len = input.length();
            long cl =  std::stol(headers["content-length"]);
            if(len < cl)
            {
                partly_data = true;
                //printf("waiting for another %ld (total %ld)\r\n",cl - len, cl);
                // check connection - if closed - cl mismatch
            }
            else if(len == cl)
            {
                body = input;
                body_parsed = true;
            } else {
                printf("\r\n Content-Length mismatch: recieved %ld - need %ld \r\n %s",len,cl, this->to_string().c_str());
            }
        } else {
            body = input.substr(2);
            body_parsed = true;
        }
    }
}

std::string HTTPResponse::print_headers()
{
    std::string res = version + " " + std::to_string(status_code) + " " + reason + "\r\n";
    for(std::map<std::string,std::string>::iterator it = headers.begin(); it != headers.end(); ++it)
    {
        res += (*it).first + ": " + (*it).second + "\r\n";
    }
    res += "\r\n";
    return res;
}

std::string HTTPResponse::to_string()
{

    return print_headers() + body;
}

std::string HTTPResponse::get_body() {
    return body;
}