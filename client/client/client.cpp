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
#include "../../server/TCPSocket.cpp"
#include "TCPclient.cpp"

int main(int argc, char *argv[])
{
    try {
        tcp_client client("localhost");
        client.connect();

        std::cout << client.read() << std::endl;

        std::string message;
        for(int i = 0; i < 2; ++i)
        {
            std::cout << "Please enter the message: ";
            std::cin >> message;
            client.send(message);
        }

    } catch (tcp_exception e) {
        std::cout << e.message << std::endl;
    }
    return 0;
}