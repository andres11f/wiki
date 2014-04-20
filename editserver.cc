#include <czmq.h>
#include <string>
#include <stdio.h>
#include <unistd.h>
#include <assert.h>
#include <stdlib.h>
#include <iostream>

int main (void){
	zctx_t *context = zctx_new();
    void *editserver = zsocket_new(context, ZMQ_REP); 
    int pn = zsocket_bind(editserver, "tcp://*:12351"); 
    cout << "Port number " << pn << "\n"; 


	return 0;
}