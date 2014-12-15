//
//  KqueueWrap.h
//  server
//
//  Created by Виктор Шатров on 13.12.14.
//  Copyright (c) 2014 Виктор Шатров. All rights reserved.
//

#ifndef server_KqueueWrap_h
#define server_KqueueWrap_h
#include <sys/event.h>
#include <vector>
#include "TCPSocket.cpp"
#include "TCPObject.h"
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <map>



struct Kqueue_wrap{
    Kqueue_wrap();
    ~Kqueue_wrap();
    void add_to_watch(TCPObject *obj);
    void run();
    int get_fd() {return this->fd;}

private:
    int fd;
    bool running;
    std::map<int, TCPObject*> watches;
    
    
};



#endif
