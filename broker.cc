#include <czmq.h>
#include <string>
#include <stdio.h>
#include <unistd.h>
#include <assert.h>
#include <stdlib.h>
#include <iostream>

using namespace std;

int main(void){
	list<string> listServers;
	zctx_t *context = zctx_new(); 
    void *responder = zsocket_new(context, ZMQ_REP); 
    int pn = zsocket_bind(responder, "tcp://*:12350"); 
    cout << "Port number " << pn << "\n"; 

    /*
    connect to ES
    */

    /*
    connect to all server
    */

    while(1){
    	zmsg_t *msg = zmsg_new();
    	msg = zmsg_recv(responder);
    	zmsg_dump();
    	zmsg_t *response = zmsg_new();
    	dispatch(msg, response); 
        zmsg_destroy(&msg); 
        zmsg_send(&response, responder); 
  
        zmsg_destroy(&response); 
    } 
    zsocket_destroy(context, responder);  
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
    	//conectar RS
    	//send to RS	
    }
}