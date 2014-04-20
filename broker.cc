#include <czmq.h>
#include <string>
#include <stdio.h>
#include <unistd.h>
#include <assert.h>
#include <stdlib.h>
#include <iostream>

using namespace std;

int main(void){
	list<void*> listServers;
	zctx_t *context = zctx_new();
    void *broker = zsocket_new(context, ZMQ_REP); 
    int pn = zsocket_bind(broker, "tcp://*:12350"); 
    cout << "Port number " << pn << "\n"; 

    //conection to ES
    void *editserver = zsocket_new(context, ZMQ_REQ);
    int rc = zsocket_connect (editserver, "tcp://localhost:12351");
    assert(rc == 0);

    /*
    connect to all server
    for (int i = 0; i < listServers.size(); i++){
        void *tmpsocket = zsocket_new(context, ZMQ_REQ);
        zsocket_connect (editserver, unordered_map.key(i));
        unordered_map[i] = editserver;
    }
    */

    while(1){
    	zmsg_t *msg = zmsg_new();
    	msg = zmsg_recv(broker);
    	zmsg_dump();
    	zmsg_t *response = zmsg_new();
    	dispatch(msg, response); 
        zmsg_destroy(&msg); 
        zmsg_send(&response, broker); 
  
        zmsg_destroy(&response); 
    } 
    zsocket_destroy(context, broker);  
    zctx_destroy(&context); 
    return 0; 
}

void dispatch(zmsg_t *msg, zmsg_t *response){
	assert(zmsg_size(msg) >= 1);
    char *op = zmsg_popstr(msg);
    if (strcmp(op, "search") == 0){   	
    	char *nameart = zmsg_popstr(msg);
    	char *adr = zmsg_popstr(msg);
    	string server = chooseServer();	//chooseServer deberia mandar un mensaje al servidor elegido aleatoriamente preguntandole si esta ocupado. Si lo esta, alla otro servidor aleatoriamente.
    	//send to server
    }
    if (strcmp(op, "edit") == 0){
    	char *newart = zmsg_popstr(msg);
    	//send to RS
    }
}

chooseServer