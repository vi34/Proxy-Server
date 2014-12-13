//
//  TCPObject.h
//  server
//
//  Created by Виктор Шатров on 13.12.14.
//  Copyright (c) 2014 Виктор Шатров. All rights reserved.
//

#ifndef server_TCPObject_h
#define server_TCPObject_h

struct TCPObject {
    virtual void event();
    virtual int get_fd();
};

#endif
