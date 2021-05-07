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
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_DGRAM;

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

    while(true){
        int buff_size = 80;
        char buff[buff_size];
        char host[NI_MAXHOST];
        char serv[NI_MAXSERV];

        struct sockaddr client;
        socklen_t client_l = sizeof(struct sockaddr);

        int recvBytes = recvfrom(sock, buff, buff_size, 0, &client, &client_l);
        if(recvBytes == -1){
            std::cerr << "Error receiving\n";
            return -1;
        }
        buff[recvBytes] == '\0';

        getnameinfo(&client, client_l, host, NI_MAXHOST, serv, NI_MAXSERV, NI_NUMERICHOST);

        std::cout << "Host: "  << host << "    Port: "  << serv << "\n";

        if(buff[0] == 'q'){
            std::cout << "Server closed\n";
            break;
        }
        
        char buffTime[buff_size];
        time_t t = time(NULL);
        struct tm* time_ = localtime(&t);
        int timeSize = 0;

        if(buff[0] == 't'){
            timeSize = strftime(buffTime, buff_size, "%R", time_);
        }
        else if(buff[0] == 'd'){
            timeSize = strftime(buffTime, buff_size, "%D", time_);
        }

        if(timeSize == -1){
            std::cerr << "DateError\n";
            return -1;
        }

        int sendBytes = sendto(sock, buffTime, timeSize, 0, &client, client_l);
        if(sendBytes == -1){
            std::cerr << "Error sending\n";
            return -1;
        }
    }

    close(sock);

    return 0;
}