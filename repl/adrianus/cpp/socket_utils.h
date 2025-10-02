#ifndef __SOCKET_UTILS_H
#define __SOCKET_UTILS_H

#include "objects.h"

void create_server(Ad_Object*);
void create_client(Ad_Object*);
int create_client2(Ad_Object*);
Ad_Object* accept_socket(Ad_Object *rawSocket);
void send_socket(Ad_Object *rawSocket, Ad_Object *rawString);
Ad_Object* read_socket(Ad_Object*);
Ad_Object* readHTTP(Ad_Object*);
Ad_Object* sendAndReadBackHTTP(Ad_Object*, Ad_Object*);
void close_socket(Ad_Object*);
std::string cleanupUnescapedCharaters(std::string);

#endif