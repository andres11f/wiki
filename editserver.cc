#include <czmq.h>
#include <string>
#include <stdio.h>
#include <unistd.h>
#include <assert.h>
#include <stdlib.h>
#include <iostream>
#include <list>

using namespace std;

list<void*> listServers;

void dispatch(zmsg_t *msg, zmsg_t *response);

int main (void){
	int numServers = 1;
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
        string adr = "tcp://localhost:";
        adr.append(*adrsIt);
        void *tmpsocket = zsocket_new(context, ZMQ_REQ);
        zsocket_connect (tmpsocket, adr.c_str());
        listServers.push_back(tmpsocket);
        adrsIt++;
        cout<<"conexion " << i << "\n";
    }

    while(1){
    	zmsg_t *msg = zmsg_new();
        cout << "waiting...\n";
    	msg = zmsg_recv(editserver);
        cout<<"from broker \n";
    	zmsg_dump(msg);
    	list<void*>::iterator it;
        zmsg_t *response = zmsg_new();
    	for (it = listServers.begin(); it != listServers.end(); it++){
    		zmsg_send(&msg, *it);
    		response = zmsg_recv(*it);
    		char *r = zmsg_popstr(response);
    		if (strcmp(r, "failure") == 0)
    			break;
            free(r);
    	}
        cout<<"to broker \n";
        zmsg_dump(response);
    	zmsg_send(&response, editserver);
        //zmsg_destroy(&response);
    	//zmsg_destroy(&msg);
        cout<<"recibido";
    }

    zsocket_destroy(context, editserver);  
    zctx_destroy(&context); 
    return 0; 
}	