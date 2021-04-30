#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

int main(int argc, char **argv){
    
    addrinfo hints;
    addrinfo result;

    if (argc != 2)
        std::cout << "Invalid arguments\n";

    int rc = (argv[1], NULL, &hints, &result);
}