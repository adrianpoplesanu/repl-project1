#include "socket_utils.h"
#include <iostream>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>

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
    //serv_addr.sin_port = htons(5003); // socketObject->port
    //std::cout << "[ INTERNAL ][ create_server ] port: " << socketObject->port << "\n";
    serv_addr.sin_port = htons(socketObject->port);

    bind(listenfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
    listen(listenfd, 10);
    socketObject->listenfd = listenfd;
    socketObject->connfd = connfd;
}

void create_client(Ad_Object* rawSocket) {
    Ad_Socket_Object* socketObject = (Ad_Socket_Object*) rawSocket;

    struct sockaddr_in serv_addr;

    memset(socketObject->recvBuff, '0', sizeof(socketObject->recvBuff));
    socketObject->connfd = socket(AF_INET, SOCK_STREAM, 0);
    memset(&serv_addr, '0', sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    //serv_addr.sin_port = htons(5003); // socketObject->port
    serv_addr.sin_port = htons(socketObject->port);
    inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr);

    connect(socketObject->connfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
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

    std::string escapedText = cleanupUnescapedCharaters(stringObject->value);
    if (socketObject->connfd) {
        snprintf(socketObject->sendBuff, sizeof(socketObject->sendBuff), "%s", escapedText.c_str());
        write(socketObject->connfd, socketObject->sendBuff, strlen(socketObject->sendBuff));
    }
}

Ad_Object* read(Ad_Object* rawSocket) {
    Ad_Socket_Object* socketObject = (Ad_Socket_Object*) rawSocket;

    std::string message = "";
    int n;

    while ((n = read(socketObject->connfd, socketObject->recvBuff, sizeof(socketObject->recvBuff) - 1)) > 0) {
        socketObject->recvBuff[n] = 0;
        message += socketObject->recvBuff;
        if (n < sizeof(socketObject->recvBuff) - 1) break;
    }

    //int valread = read( socketObject->connfd , socketObject->recvBuff, 1024);
    //std::cout << socketObject->recvBuff << "\n";

    //Ad_String_Object* result = new Ad_String_Object(socketObject->recvBuff);
    Ad_String_Object* result = new Ad_String_Object(message);
    return result;
}

Ad_Object* readHTTP(Ad_Object* rawSocket) {
    auto* socketObject = (Ad_Socket_Object*) rawSocket;

    std::string message;
    int n;

    while (true) {
        n = read(socketObject->connfd, socketObject->recvBuff, sizeof(socketObject->recvBuff) - 1);
        if (n <= 0) {
            break;
        }

        socketObject->recvBuff[n] = '\0';
        message += socketObject->recvBuff;

        if (message.find("\r\n\r\n") != std::string::npos) {
            break;
        }
    }

    std::string formatted_message = message;
    size_t pos = 0;
    while ((pos = formatted_message.find("\r\n", pos)) != std::string::npos) {
        formatted_message.replace(pos, 2, "\n");
        pos += 1;
    }

    std::istringstream request_stream(message);
    std::string method, path, version;
    request_stream >> method >> path >> version;

    auto* result = new Ad_String_Object(formatted_message);
    return result;
}

void close(Ad_Object* rawSocket) {
    Ad_Socket_Object* socketObject = (Ad_Socket_Object*) rawSocket;

    close(socketObject->connfd);
}