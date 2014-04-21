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

zmsg_t* dispatch(zmsg_t *msg, void *server, void *editserver);
void* chooseServer();

list<void*> listServers;
int numServers = 1;

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
        string adr = "tcp://localhost:";
        adr.append(*adrsIt);
        void *tmpsocket = zsocket_new(context, ZMQ_REQ);
        zsocket_connect (tmpsocket, adr.c_str());
        listServers.push_back(tmpsocket);
        adrsIt++;
        cout << "conexion " << adr << "\n";
    }

    while(1){
    	zmsg_t *msg = zmsg_new();
        cout<<"\nwaiting...\n";
    	msg = zmsg_recv(broker);
        cout<<"\nfrom client \n";
    	zmsg_dump(msg);
        zmsg_t *response = zmsg_new();
        void *server = chooseServer();
    	response = dispatch(msg, server, editserver);
        cout<<"\nto client \n";
        zmsg_dump(response);
        zmsg_send(&response, broker);
        //zmsg_destroy(&msg);
        zmsg_destroy(&response);
    }

    for (list<void*>::iterator it = listServers.begin(); it != listServers.end(); it++)
        zsocket_destroy(context, *it);
    zsocket_destroy(context, broker);
    zsocket_destroy(context, editserver);
    zctx_destroy(&context); 
    return 0; 
}


zmsg_t* dispatch(zmsg_t *msg, void *server, void *editserver){
	assert(zmsg_size(msg) >= 1);
    char *op = zmsg_popstr(msg);    
    if (strcmp(op, "search") == 0){
        zmsg_send(&msg, server);        
        free(op);
        return zmsg_recv(server);
    }

    if (strcmp(op, "edit") == 0){
    	zmsg_send(&msg, editserver);
        free(op);
    	return zmsg_recv(editserver);
    }
}

void* chooseServer(){
    /*cout<<"paso por aca 2";
    srand(time(NULL));
    int n = rand() % numServers;*/
    int n=1;
    list<void*>::iterator iter;
    iter = listServers.begin();
    for (int i = 0; i < n; i++)
        iter = iter++;
    void* chosen = *iter;
    return chosen;
}