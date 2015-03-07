#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <iostream>
#include <errno.h>
#include "connector.h"

Connector::Connector() {
    if (getenv("SERVER_ADDRESS") == NULL || getenv("SERVER_PORT") == NULL){
        std::cerr << "Please provide a valid server address and port" << std::endl;
        throw ("Invalid server address & server port");
    }
    this->hostname = getenv("SERVER_ADDRESS");
    this->port = atoi(getenv("SERVER_PORT"));
} 

Stream* Connector::requestConnection()
{
    struct sockaddr_in address;
    memset (&address, 0, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_port = htons(this->port);

    if (resolveHostname(this->hostname, &(address.sin_addr)) != 0 ) {
        inet_pton(PF_INET, this->hostname, &(address.sin_addr));        
    } 
    int sd = socket(AF_INET, SOCK_STREAM, 0);
    if (::connect(sd, (struct sockaddr*)&address, sizeof(address)) != 0) {
        perror("connect() failed");
        return NULL;
    }
    return new Stream(sd);
}

int Connector::resolveHostname(const char* hostname, struct in_addr* addr) 
{
    struct addrinfo *res;
  
    int result = getaddrinfo (hostname, NULL, NULL, &res);
    if (result == 0) {
        memcpy(addr, &((struct sockaddr_in *) res->ai_addr)->sin_addr, sizeof(struct in_addr));
        freeaddrinfo(res);
    }
    return result;
}
