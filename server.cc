#include <czmq.h>
#include <string>
#include <stdio.h>
#include <unistd.h>
#include <assert.h>
#include <stdlib.h>
#include <iostream>
#include <unordered_map>

using namespace std;

unordered_map<string, string> articles;

int main (void){
	zctx_t *context = zctx_new();
	//ask user for the address of the bind
	string recvadr;
	cout << "port:";
	cin >> recvadr;
	string adr = "tcp://*:";
	adr.append(recvadr);
	//search socket
    void *searchsocket = zsocket_new(context, ZMQ_REP); 
    int pn = zsocket_bind(searchsocket, adr.c_str()); 
    cout << "Port number " << pn << "\n"; 

    //edit socket
    int n = atoi(recvadr.c_str()) + 1;
    recvadr = to_string(n);
    adr.append(recvadr);
    void *editsocket = zsocket_new(context, ZMQ_REP); 
    pn = zsocket_bind(editsocket, adr.c_str()); 
    cout << "Port number " << pn << "\n";

    zmq_pollitem_t items [] = {
		{searchsocket, 0, ZMQ_POLLIN, 0},
		{editsocket, 0, ZMQ_POLLIN, 0}
	};

    while(1){
    	zmq_poll(items, 3, -1);
    	//search
    	if  (items[0].revents & ZMQ_POLLIN){
    		zmsg_t *msg = zmsg_new();
    		zmsg_t *response = zmsg_new();
			msg = zmsg_recv(searchsocket);
			zmsg_dump(msg);
			char *nameart = zmsg_popstr(msg);
			string name = nameart;
			unordered_map<string,string>::const_iterator got = articles.find(name);
			if (got == articles.end())
				zmsg_addstr(response, "");
			else
				zmsg_addstr(response, articles[name].c_str());
			zmsg_destroy(&msg);
			zmsg_send(&response, searchsocket);
            zmsg_destroy(&response);
            free(nameart);
    	}
    	//edit
    	if  (items[1].revents & ZMQ_POLLIN){
    		zmsg_t *msg = zmsg_new();
    		zmsg_t *response = zmsg_new();
    		msg = zmsg_recv(editsocket);
    		zmsg_dump(msg);
    		char *nart = zmsg_popstr(msg);
    		char *neart = zmsg_popstr(msg);
    		string nameart = nart;
    		string newart = neart;
    		unordered_map<string,string>::const_iterator got = articles.find(nameart);
			if (got == articles.end())
				zmsg_addstr(response, "failure");
			articles[nameart] = newart;
    		zmsg_addstr(response, "success");
    		zmsg_destroy(&msg);
    		zmsg_send(&response, editsocket);
            zmsg_destroy(&response);
            free(nart);
            free(neart);
    	}
	}
	zsocket_destroy(context, searchsocket);
	zsocket_destroy(context, editsocket);
  	zctx_destroy(&context);
  	return 0;
}