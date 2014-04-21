#include <czmq.h>
#include <string>
#include <stdio.h>
#include <unistd.h>
#include <assert.h>
#include <stdlib.h>
#include <iostream>
#include <time.h>
#include <list>

using namespace std;

void dispatch(zmsg_t *msg, zmsg_t *response, void *editserver);
void* chooseServer();

list<void*> listServers;
int numServers = 3;

int main(void){
    list<string> adrs;
    //servers adresses
    adrs.push_back("12352");
    adrs.push_back("12354");
    adrs.push_back("12356");
    list<string>::iterator adrsIt;
    adrsIt = adrs.begin();

	zctx_t *context = zctx_new();
    //socket broker
    void *broker = zsocket_new(context, ZMQ_REP); 
    int pn = zsocket_bind(broker, "tcp://*:12350"); 
    cout << "Port number " << pn << "\n"; 

    //conection to ES
    void *editserver = zsocket_new(context, ZMQ_REQ);
    int rc = zsocket_connect (editserver, "tcp://localhost:12351");
    assert(rc == 0);

    //connection to all servers
    for (int i = 0; i < numServers; i++){
        string adr = "tcp://*:";
        adr.append(*adrsIt);
        void *tmpsocket = zsocket_new(context, ZMQ_REQ);
        zsocket_connect (editserver, adr.c_str());
        listServers.push_back(tmpsocket);
        adrsIt++;
    }

    
    while(1){
    	zmsg_t *msg = zmsg_new();
    	msg = zmsg_recv(broker);
    	zmsg_dump(msg);
    	zmsg_t *response = zmsg_new();
    	dispatch(msg, response, editserver); 
        zmsg_destroy(&msg); 
        zmsg_send(&response, broker); 
  
        zmsg_destroy(&response); 
    }

    zsocket_destroy(context, broker);  
    zctx_destroy(&context); 
    return 0; 
}


void dispatch(zmsg_t *msg, zmsg_t *response, void *editserver){
	assert(zmsg_size(msg) >= 1);
    char *op = zmsg_popstr(msg);        //DELETES THE FRAME TOO???
    if (strcmp(op, "search") == 0){
    	void *server = chooseServer();	//chooseServer deberia mandar un mensaje al servidor elegido aleatoriamente preguntandole si esta ocupado. Si lo esta, alla otro servidor aleatoriamente.
        zmsg_send(&msg, server);        //msg contains nameart, adr
        response = zmsg_recv(server);
    }

    if (strcmp(op, "edit") == 0){
    	zmsg_send(&msg, editserver);
    	response = zmsg_recv(editserver);
    }
    free(op);
}

void* chooseServer(){
    srand(time(NULL));
    int n = rand() % numServers;
    list<void*>::iterator iter;
    iter = listServers.begin();
    for (int i = 0; i < n; i++)
        iter = iter++;
    void* chosen = *iter;
    return chosen;
}