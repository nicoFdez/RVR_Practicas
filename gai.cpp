#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>

int main(int argc, char **argv){
    
    struct addrinfo hints;
    struct addrinfo* result;

    if (argc != 2){
        std::cerr << "Invalid arguments\n";
        return -1;
    }

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    int e = getaddrinfo(argv[1], NULL, &hints, &result);

    if(e != 0){
        std::cerr << gai_strerror(e) << "\n";
        return -1;
    }

    struct addrinfo* it;
    for(it = result; it !=NULL; it = it->ai_next){
        char host[NI_MAXHOST];
        char serv[NI_MAXHOST];

        getnameinfo( it->ai_addr, it->ai_addrlen, host, NI_MAXHOST, serv, NI_MAXHOST,
            NI_NUMERICHOST | NI_NUMERICSERV);

        std::cout << "Host: "  << host << "\n";
    }

    freeaddrinfo(result);
}