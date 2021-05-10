#include <iostream>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>

int main(int argc, char **argv){
    
    struct addrinfo hints;
    struct addrinfo* result;

    if (argc != 3){
        std::cerr << "Invalid arguments\n";
        return -1;
    }

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    int e = getaddrinfo(argv[1], argv[2], &hints, &result);
    if(e != 0){
        std::cerr << gai_strerror(e) << "\n";
        return -1;
    }

    int sock = socket(result->ai_family, result->ai_socktype, 0);
    if(sock == -1){
        std::cerr << "Error creating socket" << "\n";
        return -1;
    }

    bind(sock, result->ai_addr, result->ai_addrlen);

    freeaddrinfo(result);

    listen(sock, 16);

    struct sockaddr client;
    socklen_t client_l = sizeof(struct sockaddr);
    int sock_client = accept(sock, &client, &client_l);

    char host[NI_MAXHOST];
    char serv[NI_MAXSERV];
    getnameinfo(&client, client_l, host, NI_MAXHOST, serv, NI_MAXSERV, NI_NUMERICHOST);
    std::cout << "Conexión desde host: "  << host << "   puerto: "  << serv << "\n";

    while(true){

        int buff_size = 80;
        char buff[buff_size];

        int recvBytes = recv(sock_client, buff, buff_size, 0);
        if(recvBytes <= 0){
            std::cout << "Cierre de conexión\n";
            break;
        }
        buff[recvBytes] = '\0';
        //std::cout << "\t"  << buff;
        send(sock_client, buff, recvBytes, 0);
    }

    close(sock);

    return 0;
}