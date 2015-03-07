all: server client 
server: server.cc stream.cc acceptor.cc 
	g++ -std=c++0x -o stringServer server.cc stream.cc acceptor.cc 
client: client.cc stream.cc connector.cc
	g++ -std=c++0x -pthread -o stringClient client.cc stream.cc connector.cc

clean: 
	rm -f *.o *.d server client



