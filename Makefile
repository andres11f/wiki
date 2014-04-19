#export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/home/andres/cs/zmq/lib
#error while loading shared libraries: libzmq.so.3: cannot open shared object file: No such file or directory
CXX=g++ -std=c++11
ZMQ=/home/andres/cs/zmq
ZMQ_INCLUDES=$(ZMQ)/include
ZMQ_LIBS=$(ZMQ)/lib

all: client broker editserver server

client: client.o
	$(CXX) -L$(ZMQ_LIBS) -o client client.o -lzmq -lczmq

client.o: client.cc
	$(CXX) -I$(ZMQ_INCLUDES) -c client.cc

broker: broker.o
	$(CXX) -L$(ZMQ_LIBS) -o broker broker.o -lzmq -lczmq

broker.o: broker.cc
	$(CXX) -I$(ZMQ_INCLUDES) -c broker.cc

editserver: editserver.o
	$(CXX) -L$(ZMQ_LIBS) -o editserver editserver.o -lzmq -lczmq

editserver.o: editserver.cc
	$(CXX) -I$(ZMQ_INCLUDES) -c editserver.cc

server: server.o
	$(CXX) -L$(ZMQ_LIBS) -o server server.o -lzmq -lczmq

server.o:server.cc
	$(CXX) -I$(ZMQ_INCLUDES) -c server.cc

clean:
	rm -f client client.o broker broker.o editserver editserver.o server server.o
