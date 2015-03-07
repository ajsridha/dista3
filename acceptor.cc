#include <stdio.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>
#include <string.h>
#include <iostream>
#include <unistd.h>
#include <netinet/in.h>


#include <map>
#include <algorithm>
#include "acceptor.h"

Acceptor::Acceptor() {
    this->isListening = false;
    this->hostname = getenv("SERVER_ADDRESS");
    if (getenv("SERVER_PORT") == NULL){
        this->port = 0;
    } else {
        this->port = atoi(getenv("SERVER_PORT"));
    }    
    this->sock = 0;
} 

Acceptor::~Acceptor() {
    if (this->sock > 0) {
        close(this->sock);
    }
}

std::string Acceptor::titleCase(std::string line){
    transform(line.begin(), line.end(), line.begin(), ::tolower);
    if (line.length() > 0 ) {
        line[0] = toupper(line[0]);
        if (line.length() > 1) {
            for (int i = 1; i < line.length(); i++){
                if (line[i-1] == ' ') {
                    line[i] = toupper(line[i]);
                }
            }
        }
    }
    return line;
}

int Acceptor::start() {
    if (this->isListening == true) {
        return true;
    }

    this->sock = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in address;
    bool rebound = false;

    memset(&address, 0, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_port = htons(port);

    FD_ZERO(&master);
    FD_ZERO(&read_fds);

    //if hostname is set
    if (this->hostname != NULL) {
        inet_pton(AF_INET, hostname, &(address.sin_addr));
    } else {
        address.sin_addr.s_addr = INADDR_ANY;
        rebound = true;
    }

    int optval = 1;
    setsockopt(this->sock, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof optval);
    //set socket to be non-blocking
    ioctl(this->sock, FIONBIO, (char *)&optval);

    int result = ::bind(this->sock, (struct sockaddr*)&address, sizeof(address));
    if (result != 0) {
        address.sin_addr.s_addr = INADDR_ANY;
        address.sin_port = htons(port);
        ::bind(this->sock, (struct sockaddr*)&address, sizeof(address));
        rebound = true;
    }
    


    result = listen(this->sock, 32);
    if (result != 0) {
        std::cerr << "failure on listen" << std::endl;
        return false;
    }

    if (rebound) {
        //print address and port
        char hostname[1024];
        hostname[1023] = '\0';
        gethostname(hostname, 1023);
        printf("SERVER_ADDRESS %s\n", hostname);
    } else {
        printf("SERVER_ADDRESS %s\n", this->hostname);
    }

    struct sockaddr_in sin;
    socklen_t len = sizeof(sin);
    getsockname(sock, (struct sockaddr *)&sin, &len);
    printf("SERVER_PORT %d\n", ntohs(sin.sin_port));

    FD_SET(this->sock, &this->master);
    fdmax = this->sock;

    this->isListening = true;
    return true;
}

Stream* Acceptor::acceptConnection() {
    if (this->isListening == false) {
        return NULL;
    }

    int newfd;
    map<int, Stream*> streams;

    while(true){
        read_fds = master;
        if (select(fdmax+1, &read_fds, NULL, NULL, NULL) == -1) {
            perror("select");
            exit(4);
        }
        for (int i=this->sock; i <= fdmax; i++){
            if (FD_ISSET(i, &read_fds)) {
                if (i == this->sock) {
                    // new connection
                    struct sockaddr_storage remoteaddr;
                    addrlen = sizeof remoteaddr;
                    newfd = accept(this->sock, (struct sockaddr *)&remoteaddr, &addrlen);

                    if (newfd == -1) {
                        perror("accept");
                    } else {
                        FD_SET(newfd, &master); 
                        if (newfd > fdmax) {
                            fdmax = newfd;
                        }
                        streams[newfd] = new Stream(newfd);
                    }
                } else {
                    // new input
                    char *buffer= NULL;
                    int result = streams[i]->receiveMessage(&buffer);
                    if (result <= 0) {                        
                        if (result != 0) {
                            std::cerr << errno << endl;
                        } else {
                            close(i);
                            FD_CLR(i, &master);
                            streams.erase(i);
                        }
                    } else {
                        string titleCaseString = titleCase(buffer);
                        streams[i]->sendMessage(titleCaseString);                
                    }
                }
            } 
        }
    }
}
