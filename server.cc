#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <iostream>
#include "acceptor.h"

int main(int argc, char** argv)
{

    Stream* stream = NULL;
    Acceptor* acceptor = new Acceptor();

    if (acceptor->start()) {
        while (1) {
            stream = acceptor->acceptConnection();
        }
    }
    exit(0);
}
