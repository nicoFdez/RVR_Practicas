#include <iostream>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>

int main(int argc, char **argv){
    
    struct addrinfo hints;
    struct addrinfo* client_result;
    struct addrinfo* server_result;

    if (argc != 4){
        std::cerr << "Invalid arguments\n";
        return -1;
    }

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_DGRAM;

    int e = getaddrinfo(argv[1], argv[2], &hints, &server_result);
    if(e != 0){
        std::cerr << gai_strerror(e) << "\n";
        return -1;
    }

    char* client_addr = "127.0.0.1";
    char* client_serv = "7778";

    e = getaddrinfo( client_addr, client_serv, &hints, &client_result);
    if(e != 0){
        std::cerr << gai_strerror(e) << "\n";
        return -1;
    }

    int sock = socket(server_result->ai_family, server_result->ai_socktype, 0);
    if(sock == -1){
        std::cerr << "Error creating socket" << "\n";
        return -1;
    }

    bind(sock, client_result->ai_addr, client_result->ai_addrlen);

    int buff_size = 80;
    char buff[buff_size];

    int sendBytes = sendto(sock, argv[3], buff_size, 0, server_result->ai_addr, server_result->ai_addrlen);
    if(sendBytes == -1){
        std::cerr << "Error sending\n";
        return -1;
    }

    struct sockaddr client;
    socklen_t client_l = sizeof(struct sockaddr);

    if(argv[3][0] != 'q'){
        int recvBytes = recvfrom(sock, buff, buff_size, 0, &client, &client_l);
        if(recvBytes == -1){
            std::cerr << "Error receiving\n";
            return -1;
        }
        buff[recvBytes] == '\0';
        std::cout << buff << "\n";
    }

    freeaddrinfo(client_result);
    freeaddrinfo(server_result);
    close(sock);

    return 0;
}