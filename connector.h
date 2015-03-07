#ifndef __connector_h__
#define __connector_h__

#include <netinet/in.h>
#include "stream.h"

class Connector
{
   
	int port;
	char* hostname;
	int resolveHostname(const char* host, struct in_addr* addr);
	
 	public:
	Connector();
	Stream* requestConnection();

};

#endif