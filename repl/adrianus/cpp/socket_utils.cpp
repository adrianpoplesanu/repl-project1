#include "socket_utils.h"
#include <iostream>
#include <unistd.h>
#include <netinet/in.h>

std::string cleanupUnescapedCharaters(std::string text) {
    std::string result;
    result = std::regex_replace(text, std::regex("\\\\r"), "\r");
    result = std::regex_replace(result, std::regex("\\\\n"), "\n");
    return result;
}

void create_server(Ad_Object* rawSocket) {
    Ad_Socket_Object* socketObject = (Ad_Socket_Object*) rawSocket;

    int listenfd = 0, connfd = 0;
    struct sockaddr_in serv_addr;

    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    memset(&serv_addr, '0', sizeof(serv_addr));
    memset(socketObject->sendBuff, '0', sizeof(socketObject->sendBuff));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(5003); // socketObject->port

    bind(listenfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
    listen(listenfd, 10);
    socketObject->listenfd = listenfd;
    socketObject->connfd = connfd;
}

void create_client(Ad_Object* rawSocket) {
    Ad_Socket_Object* socketObject = (Ad_Socket_Object*) rawSocket;

    // TODO: create a client connfd, right?
}

Ad_Object* accept(Ad_Object* rawSocket) {
    Ad_Socket_Object* socketObject = (Ad_Socket_Object*) rawSocket;

    int connfd = accept(socketObject->listenfd, (struct sockaddr*)NULL, NULL);
    Ad_Socket_Object *clientSocket = new Ad_Socket_Object("client", "localhost", 5003, true, false, true, false);
    clientSocket->connfd = connfd;
    return clientSocket;
}

void send(Ad_Object* rawSocket, Ad_Object* rawString) {
    Ad_Socket_Object* socketObject = (Ad_Socket_Object*) rawSocket;
    Ad_String_Object* stringObject = (Ad_String_Object*) rawString;

    //snprintf(socketObject->sendBuff, sizeof(socketObject->sendBuff), "HTTP/1.1 200 OK\r\nServer: AdServer\r\nContent-Type: text/html\r\nContent-Length: 91\r\nConnection: close\r\n\r\n<html>this is a <b>http bebe dex</b> response from Ad Server cpp with better reading</html>");
    std::string escapedText = cleanupUnescapedCharaters(stringObject->value);
    snprintf(socketObject->sendBuff, sizeof(socketObject->sendBuff), "%s", escapedText.c_str());
    write(socketObject->connfd, socketObject->sendBuff, strlen(socketObject->sendBuff));
}