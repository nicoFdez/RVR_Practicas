#include <iostream>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>
#include <thread>

#define MAX_THREADS 2

class MessageThread
{
public:
    MessageThread(int sd){
        _sd = sd;
    }

    void procesaMensaje(){
        while(true){
            int buff_size = 80;
            char buff[buff_size];
            char host[NI_MAXHOST];
            char serv[NI_MAXSERV];

            struct sockaddr client;
            socklen_t client_l = sizeof(struct sockaddr);

            int recvBytes = recvfrom(_sd, buff, buff_size, 0, &client, &client_l);
            if(recvBytes == -1){
                std::cerr << "Error receiving\n";
                return;
            }
            buff[recvBytes] == '\0';

            getnameinfo(&client, client_l, host, NI_MAXHOST, serv, NI_MAXSERV, NI_NUMERICHOST);

            std::cout << "Host: "  << host << "    Port: "  << serv << "\n";
            std::cout << "\t["  << std::this_thread::get_id() << "] Data: "  << buff << "\n";

            sleep(5);

            int sendBytes = sendto(_sd, buff, buff_size, 0, &client, client_l);
            if(sendBytes == -1){
                std::cerr << "Error sending\n";
                return;
            }
        }   
    }

private:
    int _sd;
};

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

    for(int i=0; i<MAX_THREADS; ++i){
        MessageThread *mt = new MessageThread(sock);

        std::thread([&mt](){
            mt->procesaMensaje();
            delete mt;
        }).detach();
    }
    
    char c = ' ';
    while(c != 'q'){
        std::cin.get(c);
    }

    close(sock);

    return 0;
}