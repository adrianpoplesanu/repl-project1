#ifndef __SOCKET_UTILS_H
#define __SOCKET_UTILS_H

#include "objects.h"

void create_server(Ad_Object*);
void create_client(Ad_Object*);
Ad_Object* accept(Ad_Object*);
void send(Ad_Object*, Ad_Object*);
Ad_Object* read(Ad_Object*);
void close(Ad_Object*);
std::string cleanupUnescapedCharaters(std::string);

#endif