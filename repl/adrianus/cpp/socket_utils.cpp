#include "socket_utils.h"
#include <iostream>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

std::string cleanupUnescapedCharaters(std::string text) {
    std::string result;
    result = std::regex_replace(text, std::regex("\\\\r"), "\r");
    result = std::regex_replace(result, std::regex("\\\\n"), "\n");
    return result;
}

void create_server(Ad_Object* rawSocket) {
    auto* socketObject = (Ad_Socket_Object*) rawSocket;

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
    // this works only for IPs, it does not do DNS resolving
    auto* socketObject = (Ad_Socket_Object*) rawSocket;

    struct sockaddr_in serv_addr;

    memset(socketObject->recvBuff, '0', sizeof(socketObject->recvBuff));
    socketObject->connfd = socket(AF_INET, SOCK_STREAM, 0);
    memset(&serv_addr, '0', sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    //serv_addr.sin_port = htons(5003); // socketObject->port
    serv_addr.sin_port = htons(socketObject->port);
    inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr);
    //inet_pton(AF_INET, "adrianus.ro", &serv_addr.sin_addr); // this does not work

    //std::cout << "[ INTERNAL ][ create_client ] port: " << socketObject->port << "\n";
    connect(socketObject->connfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
}

int create_client2(Ad_Object* rawSocket) {
    auto* socketObject = (Ad_Socket_Object*) rawSocket;

    struct addrinfo hints{}, *res;
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    std::string portStr = std::to_string(socketObject->port);

    std::string url = socketObject->host;

    std::string domain, uri;

    // Remove protocol if present
    std::size_t protocol_pos = url.find("://");
    if (protocol_pos != std::string::npos) {
        url = url.substr(protocol_pos + 3); // cut "http://" or "https://"
    }

    // Find first slash (domain vs path split)
    std::size_t pos = url.find('/');
    if (pos != std::string::npos) {
        domain = url.substr(0, pos);
        uri = "/" + url.substr(pos + 1);
    } else {
        domain = url;
        uri = "/";  // no path
    }

    const char* hostname = domain.c_str();
    int err = getaddrinfo(hostname, portStr.c_str(), &hints, &res);
    if (err != 0) {
        std::cerr << "[ INTERNAL ][ create_client2 ] " << gai_strerror(err) << "\n";
        return -1;
    }

    socketObject->connfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (socketObject->connfd < 0) {
        perror("[ INTERNAL ][ create_client2 ] socket create error");
        freeaddrinfo(res);
        return -1;
    }

    if (connect(socketObject->connfd, res->ai_addr, res->ai_addrlen) < 0) {
        perror("[ INTERNAL ][ create_client2 ] socket connect error");
        close(socketObject->connfd);
        freeaddrinfo(res);
        return -1;
    }

    freeaddrinfo(res);
    return 0;
}

Ad_Object* accept_socket(Ad_Object* rawSocket) {
    Ad_Socket_Object* socketObject = (Ad_Socket_Object*) rawSocket;

    int connfd = accept(socketObject->listenfd, (struct sockaddr*)NULL, NULL);
    Ad_Socket_Object *clientSocket = new Ad_Socket_Object("client", "localhost", 5003, true, false, true, false);
    clientSocket->connfd = connfd;
    return clientSocket;
}

Ad_Object* sendAndReadBackHTTP(Ad_Object* rawSocket, Ad_Object* rawString) {
    Ad_Socket_Object* socketObject = (Ad_Socket_Object*) rawSocket;
    Ad_String_Object* stringObject = (Ad_String_Object*) rawString;

    // START

    std::string url = socketObject->host;
    std::string payload = stringObject->value;

    std::string domain, uri;

    // Remove protocol if present
    std::size_t protocol_pos = url.find("://");
    if (protocol_pos != std::string::npos) {
        url = url.substr(protocol_pos + 3); // fara "http://" sau "https://" sau "xxx://"
    }

    // Find first slash (domain vs path split)
    std::size_t pos = url.find('/');
    if (pos != std::string::npos) {
        domain = url.substr(0, pos);
        uri = "/" + url.substr(pos + 1);
    } else {
        domain = url;
        uri = "/";  // no path
    }

    //std::cout << "Domain: " << domain << std::endl;
    //std::cout << "Path: " << uri << std::endl;

    /*std::size_t start = 0;
    std::size_t protocol_pos = socketObject->host.find("://");
    if (protocol_pos != std::string::npos) {
        start = protocol_pos + 3;  // skip "://"
    }

    // Find first slash after domain
    std::size_t pos = socketObject->host.find('/', start);

    std::string domain;
    std::string uri;
    if (pos != std::string::npos) {
        domain = socketObject->host.substr(0, pos);
        uri = socketObject->host.substr(pos + 1);

        std::cout << "Domain: " << domain << std::endl;
        std::cout << "Path: " << uri << std::endl;
    } else {
        // No path, just domain
        domain = socketObject->host;
        uri = "/";
        std::cout << "Domain: " << socketObject->host << std::endl;
        std::cout << "Path: (empty)" << std::endl;
    }

    // END

    domain = "adrianus.ro";
    uri = "/supaplex/supaplex8.html";*/

    //const char* host = socketObject->host.c_str();
    //std::cout << "cacat\n" << socketObject->host << "\n" << domain << "\n" << "cacat\n";
    const char* host = domain.c_str();
    //const char* path = "/";
    const char* path = uri.c_str();

    std::string escapedText =
            "GET " + std::string(path) + " HTTP/1.1\r\n"
                                         "Host: " + std::string(host) + "\r\n"
                                                                        "Connection: close\r\n"
                                                                        "\r\n";

    //std::cout << escapedText << "\n";
    //std::cout << "PAYLOAD: " << cleanupUnescapedCharaters(payload) << "\n";
    //std::string caca = "GET / HTTP/1.1\r\nHost: adrianus.ro\r\nConnection: close\r\n\r\n";
    //std::cout << "PAYLOAD: " << caca << "\n";

    if (socketObject->connfd) {
        snprintf(socketObject->sendBuff, sizeof(socketObject->sendBuff), "%s", cleanupUnescapedCharaters(payload).c_str());
        ssize_t sent = write(socketObject->connfd, socketObject->sendBuff, strlen(socketObject->sendBuff));
        if (sent < 0) {
            perror("[ INTERNAL ][ create_client2 ] socket write failed");
        } else {
            //std::cout << "Sent " << sent << " bytes\n";
        }
    }

    std::string message = "";
    int n;

    /*while ((n = read(socketObject->connfd, socketObject->recvBuff, sizeof(socketObject->recvBuff) - 1)) > 0) {
        socketObject->recvBuff[n] = 0;
        message += socketObject->recvBuff;
        if (n < sizeof(socketObject->recvBuff) - 1) break;
    }*/

    while ((n = read(socketObject->connfd, socketObject->recvBuff, sizeof(socketObject->recvBuff) - 1)) > 0) {
        socketObject->recvBuff[n] = '\0';
        message += socketObject->recvBuff;
    }

    Ad_String_Object* result = new Ad_String_Object(message);
    return result;
}

void sendAndReadBackHTTPS(Ad_Object* rawSocket, Ad_Object* rawString) {
    Ad_Socket_Object* socketObject = (Ad_Socket_Object*) rawSocket;
    Ad_String_Object* stringObject = (Ad_String_Object*) rawString;

    // TODO write this
}

void send_socket(Ad_Object* rawSocket, Ad_Object* rawString) {
    Ad_Socket_Object* socketObject = (Ad_Socket_Object*) rawSocket;
    Ad_String_Object* stringObject = (Ad_String_Object*) rawString;

    std::string escapedText = cleanupUnescapedCharaters(stringObject->value);
    if (socketObject->connfd) {
        snprintf(socketObject->sendBuff, sizeof(socketObject->sendBuff), "%s", escapedText.c_str());
        write(socketObject->connfd, socketObject->sendBuff, strlen(socketObject->sendBuff));
    }
}

void send_socket2(Ad_Object* rawSocket, Ad_Object* rawString) {
    Ad_Socket_Object* socketObject = (Ad_Socket_Object*) rawSocket;
    Ad_String_Object* stringObject = (Ad_String_Object*) rawString;

    //std::cout << "teeeest\n";
    //test_send();

    //std::string escapedText = cleanupUnescapedCharaters(stringObject->value);
    //std::string escapedText = stringObject->value;
    //std::cout << socketObject->host << "\n";
    //const char* host = "adrianus.ro";
    const char* host = socketObject->host.c_str();
    const char* path = "/";

    std::string escapedText =
            "GET " + std::string(path) + " HTTP/1.1\r\n"
                                         "Host: " + std::string(host) + "\r\n"
                                                                        "Connection: close\r\n"
                                                                        "\r\n";

    //std::cout << "caca maca\n";
    if (socketObject->connfd) {
        //std::cout << "qqq\n";
        snprintf(socketObject->sendBuff, sizeof(socketObject->sendBuff), "%s", escapedText.c_str());
        //std::cout << "www\n";
        ssize_t sent = write(socketObject->connfd, socketObject->sendBuff, strlen(socketObject->sendBuff));
        if (sent < 0) {
            perror("write failed");
        } else {
            std::cout << "Sent " << sent << " bytes\n";
        }
        //std::cout << "zzz\n";
    }
    //std::cout << "bla bla bla\n";
}

Ad_Object* read_socket(Ad_Object* rawSocket) {
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

void close_socket(Ad_Object* rawSocket) {
    auto* socketObject = (Ad_Socket_Object*) rawSocket;

    close(socketObject->connfd);
}