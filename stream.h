
#ifndef __stream_h__
#define __stream_h__

#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string>

using namespace std;

class Stream
{
    int sock;

  public:
    friend class Acceptor;
    friend class Connector;

    Stream(int sd);
    ~Stream();

    ssize_t sendMessage(string message);
    ssize_t receiveMessage(char** buffer);


};

#endif
