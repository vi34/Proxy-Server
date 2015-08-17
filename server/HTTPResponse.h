//
//  HTTPResponse.h
//  server
//
//  Created by Виктор Шатров on 10.02.15.
//  Copyright (c) 2015 Виктор Шатров. All rights reserved.
//

#ifndef server_HTTPResponse_h
#define server_HTTPResponse_h
#include <string>
#include <map>

struct HTTPResponse {
    std::string input;
    std::string version;
    std::string reason;
    std::string body; // should be private?
    int status_code;
    unsigned int chunk_size;

    void parse();
    std::string print_headers();
    std::string to_string();
    std::string get_body();
    bool correct();

private:
    bool headers_parsed = false;
    bool start_line_parsed = false;
    bool body_parsed = false;
    bool partly_data = false;
    std::map<std::string,std::string> headers;

};

#endif
