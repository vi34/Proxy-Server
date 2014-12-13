//
//  KqueueWrap.cpp
//  server
//
//  Created by Виктор Шатров on 09.12.14.
//  Copyright (c) 2014 Виктор Шатров. All rights reserved.
//

#include "KqueueWrap.h"

Kqueue_wrap::Kqueue_wrap()
{
    if ((fd = kqueue()) == -1) {
        throw tcp_exception("kqueue()");
    }
}


Kqueue_wrap::~Kqueue_wrap()
{
    close(fd);
}

void Kqueue_wrap::add_to_watch(TCPObject *obj)
{
    struct kevent ev;

}

void Kqueue_wrap::run()
{
    running = true;
    struct kevent ev;
    signal(SIGINT, SIG_IGN);
    EV_SET(&ev, SIGINT, EVFILT_SIGNAL, EV_ADD, 0, 0, NULL);
    int kevent_res = kevent(fd, &ev, 1, NULL, 0, NULL);
    if (kevent_res == -1) {
        throw tcp_exception("kevent()");
    }
    signal(SIGTERM, SIG_IGN);
    EV_SET(&ev, SIGTERM, EVFILT_SIGNAL, EV_ADD, 0, 0, NULL);
    kevent_res = kevent(fd, &ev, 1, NULL, 0, NULL);
    if (kevent_res == -1) {
        throw tcp_exception("kevent()");
    }

    while(running)
    {
        kevent_res = kevent(fd, NULL, 0, &ev, 1, NULL);
        if(kevent_res > 0)
        {
            if(ev.filter == EVFILT_SIGNAL && (ev.ident == SIGINT || ev.ident == SIGTERM))
            {
                std::cout << std::endl << "-_- NO DUDE, NO... " << std::endl;
                return;
            } else {
                 events[ev.ident](ev.ident);
            }
        }

    }
}


