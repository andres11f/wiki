#include <czmq.h>
#include <string>
#include <stdio.h>
#include <unistd.h>
#include <assert.h>
#include <stdlib.h>
#include <iostream>


int main (void){
	zctx_t *context = zctx_new();
	//ask user for the address of the bind
	//search socket
    void *searchsocket = zsocket_new(context, ZMQ_REP); 
    int pn = zsocket_bind(searchsocket, "tcp://*:12352"); 
    cout << "Port number " << pn << "\n"; 

    //edit socket
    void *editsocket = zsocket_new(context, ZMQ_REP); 
    int pn = zsocket_bind(editsocket, "tcp://*:12353"); 
    cout << "Port number " << pn << "\n";

	return 0;
}