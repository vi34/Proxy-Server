#include <cstdio>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <iostream>
#include "TCPServer.h"


int main(int argc, char *argv[])
{
    try {
        Kqueue_wrap kq;
        TCPServer serv(kq, 11133);


        client* remote = serv.connect_to("localhost", 1112);
        remote->set_read_callback([](std::string message, client* c2){
            std::cout << message << std::endl;
        });

        remote->send("OPTIONS / HTTP/1.1\r\nHost: www.example.com\r\n\r\n");
        //remote->send("GET / HTTP/1.1\r\nHost: www.opera.com\r\n\r\n");

        kq.run();

        
/*
        

        tcp_client client("localhost", 1112);
        client.connect();



        std::string message = "GET / HTTP/1.1\r\nHost: www.opera.com\r\n\r\n";
        std::cout << message << std::endl;
        client.send(message);
        std::cout << client.read() << std::endl;
        for(int i = 0; i < 2; ++i)
        {
            std::cout << "Please enter the message: ";
            std::cin >> message;
            client.send(message);
        }
  */

    } catch (tcp_exception e) {
        std::cout << e.message << std::endl;
    }

    return 0;
}