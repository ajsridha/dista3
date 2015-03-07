#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string>
#include <iostream>
#include <queue>
#include <mutex>
#include "connector.h"

using namespace std;

queue<string> bufferedInput;
mutex messages_mutex;

void *sendMessages(void* arg){
    string input;
    int len;
    char line[256];
    while (getline(std::cin, input)) {
        if (input == ""){
            continue;
        }
        messages_mutex.lock();
        bufferedInput.push(input);
        messages_mutex.unlock();
    }

    return 0;      
}

void *bufferMessages(void* arg) {
    Stream* stream = ((Stream*) (arg));
    while (true) {
        if (!bufferedInput.empty()) {
            string input = bufferedInput.front();
            int result = stream->sendMessage(input);
            bufferedInput.pop();
            sleep(2);
        }

    }
    free(stream);
    return 0;
}

void *receiveMessages(void* arg){
    Stream* stream = ((Stream*) (arg));
    char* buffer;
    while (true) {
        int result = stream->receiveMessage(&buffer);
        if (result > 0) {
            string str(buffer);
            cout << "Server: " << buffer << endl;            
        }
        free(buffer);
    }
    return 0;
}

int main(int argc, char** argv)
{
    Connector* connector = new Connector();
    Stream* stream = connector->requestConnection();
    pthread_t sender, receiver, buffer;

    if (stream) {
        pthread_create(&sender, NULL, sendMessages, (void*) stream);
        pthread_create(&buffer, NULL, bufferMessages, (void*) stream);
        pthread_create(&receiver, NULL, receiveMessages, (void*) stream);
    }
    pthread_exit(NULL);
    exit(0);
}