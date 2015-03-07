#ifndef __acceptor_h__
#define __acceptor_h__

#include <string>
#include <netinet/in.h>
#include <sys/types.h>
#include "stream.h"

class Acceptor
{
    int sock;
    int port;
    char* hostname;
    bool isListening;
    fd_set master;
    fd_set read_fds;
    int fdmax;
    socklen_t addrlen;
 	std::string titleCase(std::string line);

  public:
    Acceptor();
    ~Acceptor();

    int start();
    Stream* acceptConnection();
};

#endif
