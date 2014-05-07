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

list<void*> listServers;    //sockets list

zmsg_t* dispatch(zmsg_t *msg, void *editserver, int numServers);
void* chooseServer(int numServers);

int main(void){
    list<string> adrs;
    list<string>::iterator adrsIt;
    string adr;

	zctx_t *context = zctx_new();
    //socket broker
    string recvadr;
    cout << "port for client:";
    cin >> recvadr;
    adr = "tcp://*:";
    adr.append(recvadr);
    void *broker = zsocket_new(context, ZMQ_REP); 
    int pn = zsocket_bind(broker, adr.c_str()); 
    //cout << "broker port " << pn << "\n"; 

    //conection to ES
    void *editserver = zsocket_new(context, ZMQ_REQ);
    int rc = zsocket_connect (editserver, "tcp://localhost:12350");
    assert(rc == 0);

    //ask for servers
    string a;
    int numServers=0;
    cout << "number of servers: ";
    cin >> numServers;
    for (int i=0; i < numServers; i++){
        cout << "search port server "<<i+1<<": ";
        cin >> a;
        adrs.push_back(a);
    }

    //connection to all servers
    adr="";
    adrsIt = adrs.begin();
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
    	response = dispatch(msg, editserver, numServers);
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


zmsg_t* dispatch(zmsg_t *msg, void *editserver, int numServers){
	assert(zmsg_size(msg) >= 1);
    char *op = zmsg_popstr(msg);    
    if (strcmp(op, "search") == 0){
        void *server = chooseServer(numServers);
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

void* chooseServer(int numServers){
    srand(time(NULL));
    int n = rand() % numServers;
    cout << "chosen server: "<<n+1<<"\n";
    list<void*>::iterator iter;
    iter = listServers.begin();
    for (int i = 0; i < n; i++)
        iter++;
    return *iter;
}