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

    if (argc != 3){
        std::cerr << "Invalid arguments\n";
        return -1;
    }

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    int e = getaddrinfo(argv[1], argv[2], &hints, &server_result);
    if(e != 0){
        std::cerr << gai_strerror(e) << "\n";
        return -1;
    }

    char* client_addr = "127.0.0.1";
    char* client_serv = "7778";
    e = getaddrinfo(client_addr, client_serv, &hints, &client_result);
    if(e != 0){
        std::cerr << gai_strerror(e) << "\n";
        return -1;
    }

    int sock = socket(client_result->ai_family, client_result->ai_socktype, 0);
    if(sock == -1){
        std::cerr << "Error creating socket" << "\n";
        return -1;
    }

    connect(sock, server_result->ai_addr, server_result->ai_addrlen);

    freeaddrinfo(client_result);
    freeaddrinfo(server_result);

    while(true){

        int buff_size = 80;
        char buff[buff_size];

        std::cin.getline(buff, buff_size);
        
        if(strcmp(buff, "Q") == 0){
            std::cout << "Cierre de conexión\n";
            break;
        }

        int sendBytes = send(sock, &buff, buff_size, 0);
        if(sendBytes < 0){
            std::cout << "Cierre de conexión\n";
            break;
        }
        recv(sock, buff, buff_size, 0);
        std::cout << buff << "\n";
    }

    close(sock);

    return 0;
}