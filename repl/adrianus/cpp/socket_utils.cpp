#include "socket_utils.h"
#include <iostream>
#include <unistd.h>
#include <netinet/in.h>

void create_server(Ad_Object* rawSocket) {
    Ad_Socket_Object* socketObject = (Ad_Socket_Object*) rawSocket;

    int listenfd = 0, connfd = 0;
    struct sockaddr_in serv_addr;

    char sendBuff[1024];

    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    memset(&serv_addr, '0', sizeof(serv_addr));
    memset(sendBuff, '0', sizeof(sendBuff));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(5003);

    bind(listenfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
    listen(listenfd, 10);
    socketObject->listenfd = listenfd;
    socketObject->connfd = connfd;
}

void create_client(Ad_Object* rawSocket) {
    Ad_Socket_Object* socketObject = (Ad_Socket_Object*) rawSocket;

    // TODO: create a client connfd, right?
}