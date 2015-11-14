//
//  ProxyServer.cpp
//  server
//
//  Created by Виктор Шатров on 18.11.14.
//  Copyright (c) 2014 Виктор Шатров. All rights reserved.
//

#include <cstring>
#include <cstdio>
#include <iostream>
#include <queue>
#include <curl/curl.h>
#include <curl/curlbuild.h>
#include <curl/curlrules.h>
#include <curl/curlver.h>
#include <curl/easy.h>
#include <curl/typecheck-gcc.h>
#include <math.h>
#include <memory>

#include "TCPServer.h"
#include "HTTPServer.h"
#include "HTTPClient.h"

using namespace std;

long long INF = 10000000000;

const long DEF_MAX_AGE = 43200;


long calc_current_age(std::shared_ptr<HTTPResponse> response) {
    time_t date_value = curl_getdate(response->headers["date"].c_str(), NULL);
    time_t apparent_age = max((time_t)0, response->response_time - date_value);
    long age_value = 0;
    if (response->headers.find("age") != response->headers.end())
        age_value = stol(response->headers["age"]);
    long corrected_received_age = max(apparent_age, age_value);
    long response_delay = response->response_time - response->request_time;
    long corrected_initial_age = corrected_received_age + response_delay;
    long resident_time = time(0) - response->response_time;
    return corrected_initial_age + resident_time;
}

int main ()
{
    //int shell_res = system("sudo networksetup -setwebproxystate \"Wi-Fi\" on");
    curl_global_init(CURL_GLOBAL_NOTHING);
    std::map<std::string, std::shared_ptr<HTTPResponse> > cache;
    try {
        HTTPServer server(1112);

        server.set_request_callback([&server, &cache](std::shared_ptr<HTTPRequest> request, HTTPClient* client){
            printf("Proxy: request %s\n", request->uri.c_str());
            bool cache_valid = false;
            long age = 0;
            if (cache.find(request->uri) != cache.end()) {
                cache_valid = true;
                age = calc_current_age(cache[request->uri]);
                long freshness = DEF_MAX_AGE;
                if (cache[request->uri]->headers.find("max-age") != cache[request->uri]->headers.end()) {
                    freshness = stol(cache[request->uri]->headers["max-age"]);
                } else if (cache[request->uri]->headers.find("s-maxage") != cache[request->uri]->headers.end()) {
                    freshness = stol(cache[request->uri]->headers["s-maxage"]);
                } else if (cache[request->uri]->headers.find("expires") != cache[request->uri]->headers.end()) {
                    freshness = curl_getdate(cache[request->uri]->headers["expires"].c_str(), NULL) -
                    curl_getdate(cache[request->uri]->headers["date"].c_str(), NULL);
                }
                if (freshness <= age) {
                    if (cache_valid) { // we have stale response in cache
                        cache.erase(request->uri);
                        printf("remove from cache\n");
                    }
                    cache_valid = false;
                }

            }
            if (cache_valid) {
                printf("send response from cache");
                cache[request->uri]->headers["age"] = to_string(age);
                printf(" age %ld\n", age);
                client->send_response(cache[request->uri]);
            } else {

                server.send_request(request, client, [request, client, &cache](std::shared_ptr<HTTPResponse> response) {
                    cout << "Proxy: got response from " << request->host << endl;
                    printf("---------------------------\n%s-----------------------\n", response->print_headers().c_str());

                    if (!(response->status_code == 100 && request->version == "HTTTP/1.0")) {
                        client->send_response(response);
                        //cout << ", Sent to " << client->tcp_client->get_fd() <<  endl;
                    }
                    bool cacheable = true;
                    if (response->headers.find("Cache-control") != response->headers.end() &&
                        (response->headers["Cache-control"].find("no-cache") ||
                         response->headers["Cache-control"].find("no-store"))) {
                            cacheable = false;
                    }
                    if (request->method == "OPTIONS")
                        cacheable = false;
                    if (cacheable) {
                        cache[request->uri] = response;
                    }
                });

            }
        });

        server.run();
    } catch (tcp_exception exc) {
        cout << exc.message << endl;
    }
    curl_global_cleanup();
    cout <<"Bye!";
    //shell_res = system("sudo networksetup -setwebproxystate \"Wi-Fi\" off");
    return 0;
}




