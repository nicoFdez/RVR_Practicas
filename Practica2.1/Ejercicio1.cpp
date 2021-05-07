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

    int e = getaddrinfo(argv[1], NULL, &hints, &result);

    if(e != 0){
        std::cerr << gai_strerror(e) << "\n";
        return -1;
    }

    struct addrinfo* it;
    for(it = result; it != NULL; it = it->ai_next){
        char host[NI_MAXHOST];
        char serv[NI_MAXSERV];

        getnameinfo(it->ai_addr, it->ai_addrlen, host, NI_MAXHOST, serv, NI_MAXSERV, NI_NUMERICHOST);

        std::cout << "Host: "  << host << "    Family: "  << it->ai_family
            << "    SocketType: "  << it->ai_socktype << "\n";
    }

    freeaddrinfo(result);

    return 0;
}