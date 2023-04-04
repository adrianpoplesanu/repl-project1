#ifndef __SOCKET_UTILS_H
#define __SOCKET_UTILS_H

#include "objects.h"

void create_server(Ad_Object*);
void create_client(Ad_Object*);
void accept(Ad_Object*);
void send(Ad_Object*, Ad_Object*);
void read(Ad_Object*);
void close(Ad_Object*);

#endif