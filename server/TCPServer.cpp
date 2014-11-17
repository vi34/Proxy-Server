#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/event.h>
#include <netinet/in.h>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <iostream>
#include <netdb.h>
#include <vector>


const int PORTNUM = 1112;
const int BACKLOG = 50;


void error(const char *msg)
{
    perror(msg);
    //exit(1);
}

struct tcp_exception {
    std::string message;
    tcp_exception(std::string s)
    {
        message = s;
    }
};
// remember about perror() when catch expection

struct tcp_socket
{
    int fd;
    tcp_socket()
    {
        fd = socket(AF_INET, SOCK_STREAM, 0);

        if (fd < 0)
            throw tcp_exception("ERROR opening socket");
    }

    tcp_socket(tcp_socket &&s)
    {
        fd = s.fd;//////
        s.fd = -1;
    }

    tcp_socket(int l_fd, sockaddr* client_addr, socklen_t* client_len)
    {
        fd = accept(l_fd, client_addr, client_len);
        if (fd < 0)
        {
            throw tcp_exception("ERROR on accept");
        }
    }

    tcp_socket(tcp_socket const& s) = delete;

    ~tcp_socket()
    {
        close(fd);
    }
};


int main(int argc, char *argv[])
{
    socklen_t client_len;
    sockaddr_in serv_addr, client_addr;
    char buffer[512];
    int n;
    tcp_socket listener;


    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;

    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(PORTNUM);

     //lose "Address already in use" error message
    int yes = 1;
    if (setsockopt(listener.fd,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof(int)) == -1) {
        perror("setsockopt");
        return 0;
    }
    if (bind(listener.fd, (sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
    {
        perror("ERROR on binding");
        return 0;
    }
    if(listen(listener.fd,BACKLOG) == -1)
    {
        perror("listen");
        return 0;
    }
    client_len = sizeof(client_addr);


    int kq;
    if ((kq = kqueue()) == -1) {
        perror("kqueue");
        return 0;
    }

    struct kevent ev[100];
    std::vector<tcp_socket> client_sockets;
    int ev_count = 1;
    struct kevent event_list[100];
    EV_SET(&ev[0], listener.fd, EVFILT_READ, EV_ADD, 0, 0, 0);

    bool work = true;
    int cllll = 0;

    std::string buf;
    while (work) {
        int nev = kevent(kq, ev, ev_count, event_list, ev_count, NULL);
        if(nev < 0)
        {
            perror("kevent()");
            return 0;
        }
        if(nev > 0)
        {
            for(int i = 0; i < nev; ++i)
            {
                if(event_list[i].ident == listener.fd)
                {
                    //tcp_socket nsock(listener.fd, (sockaddr *) &client_addr, &client_len);


                    client_sockets.push_back(tcp_socket (listener.fd, (sockaddr *) &client_addr, &client_len));
                    EV_SET(&ev[ev_count++], client_sockets[client_sockets.size() - 1].fd, EVFILT_READ, EV_ADD, 0, 0, 0);
                     std::cout << "client " << cllll++ << " connected" << std::endl;
                    //recv(nsock.fd, buffer, 511, 0);
                }
                else {
                    bzero(buffer,512);
                    if((n = recv(event_list[i].ident, buffer, 511, 0)) < 0)
                    {
                        perror("read from socket");
                        return 0;
                    }
                    else if(n == 0)
                    {
                        std::cout << "client disconnected" << std::endl;
                        //
                    }
                    else {
                        buf = buffer;
                        std::cout << buf << std::endl << std::endl;
                    }

                }
            }

        }


        /*
        //std::string s = "I got your ";
        //s += buffer;
        //n = send(nsock.fd,  s.c_str(), 18 + s.length(), 0);
        if (n < 0)
        {
            perror("ERROR writing to socket");
            return 0;
        }
        else if (n == 0)
        {
            //
        }
         */
    }

    close(kq);
    return 0;
}