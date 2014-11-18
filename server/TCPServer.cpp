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
#include <map>


const int PORTNUM = 1112;
const int BACKLOG = 100;


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

    tcp_socket(int l_fd, sockaddr* client_addr, socklen_t* client_len)
    {
        fd = accept(l_fd, client_addr, client_len);
        if (fd < 0)
        {
            throw tcp_exception("ERROR on accept");
        }
    }

    tcp_socket(tcp_socket &&s)
    {
        fd = s.fd;//////
        s.fd = -1;
    }



    tcp_socket(tcp_socket const& s) = delete;

    tcp_socket(tcp_socket &s)
    {
        fd = s.fd;
        s.fd = -1;
    }

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

    struct kevent ev;
    std::map<int, tcp_socket> client_sockets;
    EV_SET(&ev, listener.fd, EVFILT_READ, EV_ADD, 0, 0, 0);
    bool work = true;
    int client_num = 0;
    std::string buf;
    int kevent_res = kevent(kq, &ev, 1, NULL, 0, NULL);
    if (kevent_res == -1) {
        perror("kevent");
        return 0;
    }
    while (work) {
        memset(&ev, 0, sizeof(ev));
        kevent_res = kevent(kq, NULL, 0, &ev, 1, NULL);
        if(kevent_res < 0)
        {
            perror("kevent()");
            return 0;
        }
        if(kevent_res > 0)
        {
                if(ev.ident == listener.fd)
                {
                    tcp_socket s_tmp(listener.fd, (sockaddr *) &client_addr, &client_len);
                    int tmp_fd = s_tmp.fd;
                   // s_tmp.fd = -1;
                    client_sockets.insert(std::pair<int, tcp_socket>(s_tmp.fd,s_tmp));
                    EV_SET(&ev,tmp_fd, EVFILT_READ, EV_ADD, 0, 0, 0);
                    kevent_res = kevent(kq, &ev, 1, NULL, 0, NULL);
                    if (kevent_res == -1) {
                        perror("kevent");
                        return 0;
                    }
                     std::cout << "client " << tmp_fd << " connected" << std::endl;
                    //recv(nsock.fd, buffer, 511, 0);
                }
                else {
                    bzero(buffer,512);
                    if((n = recv(ev.ident, buffer, 511, 0)) < 0)
                    {
                        perror("read from socket");
                        return 0;
                    }
                    else if(n == 0)
                    {
                        std::cout << "client disconnected" << std::endl;
                        client_sockets.erase(ev.ident);


                    }
                    else {
                        buf = buffer;
                        std::cout <<"client " << ev.ident << ": " << buf << std::endl;
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