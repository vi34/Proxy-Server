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
#include "TCPServer.h"


using namespace std;

long long INF = 10000000000;

int main ()
{
    TCPServer server(1112);
    server.run();
    return 0;
}
