#include <czmq.h>
#include <string>
#include <stdio.h>
#include <unistd.h>
#include <assert.h>
#include <stdlib.h>
#include <iostream>

list<void*> listServers;

void dispatch(zmsg_t *msg, zmsg_t *response);

int main (void){
	int numServers = 3;
	list<string> adrs;
    //servers adresses for edition
    adrs.push_back("12353");
    adrs.push_back("12355");
    adrs.push_back("12357");
    list<string>::iterator adrsIt;
    adrsIt = adrs.begin();

	zctx_t *context = zctx_new();
	//socket editserver
    void *editserver = zsocket_new(context, ZMQ_REP); 
    int pn = zsocket_bind(editserver, "tcp://*:12351"); 
    cout << "Port number " << pn << "\n";

    //connection to all servers
    for (int i = 0; i < numServers; i++){
        string adr = "tcp://*:";
        adr.append(adrsIt);
        void *tmpsocket = zsocket_new(context, ZMQ_REQ);
        zsocket_connect (editserver, adr.c_str());
        listServers.push_back(tmpsocket);
        adrsIt++;
    }

    while(1){
    	zmsg_t *msg = zmsg_new();
    	msg = zmsg_recv(editserver);
    	zmsg_dump();
    	list<void*>::iterator it;
    	for (it = listServers.begin(); it != listServers.end(); it++){
    		zmsg_t *response = zmsg_new();
    		zmsg_send(&msg, it);
    		response = zmsg_recv(it);
        	zmsg_send(&response, editserver); 
        	zmsg_destroy(&response);
    	}
    	zmsg_destroy(&msg);
    }

    zsocket_destroy(context, editserver);  
    zctx_destroy(&context); 
    return 0; 
}