#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>

#include <iostream>
#include "stream.h"

Stream::Stream(int sock) {
    this->sock = sock;
}

Stream::~Stream()
{
    close(this->sock);
}

ssize_t Stream::sendMessage(string message) 
{
    int length = message.length() + 1;
    char * buffer = new char [4 + length];
    for(int i=3; i>=0; i--) {
        buffer[i] = (length>> (8*i)) & 0xFF;
    }

    for (int i=0; i < message.length(); i++){
        buffer[i+4] = message[i];
    }
    buffer[4+ message.length()] = 0;
    int result = send(this->sock, buffer, message.length() + 5, 0);
    free(buffer);
    return result;
}

ssize_t Stream::receiveMessage(char **returnBuffer) 
{
    int lengthOfMessage;
    int length = recv(this->sock, &lengthOfMessage, sizeof(int), 0);
    if (length <= 0) {
        return length;
    }

    char* localBuffer = new char [lengthOfMessage];
    int result = recv(this->sock, localBuffer, lengthOfMessage, 0);
    *returnBuffer = localBuffer;

    return result;
}
