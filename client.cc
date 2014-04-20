#include <czmq.h>
#include <string>
#include <stdio.h>
#include <unistd.h>
#include <assert.h>
#include <stdlib.h>
#include <iostream>

using namespace std;

string search (string nameart, string adr, void* broker, void* recart);
void edit (string newart, void* broker);


int main (void){
	zctx_t *context = zctx_new();
	//socket broker
	void *broker = zsocket_new(context, ZMQ_REQ);
	int rc = zsocket_connect (broker, "tcp://localhost:12350");
	assert(rc == 0);

	//socket recart
	string listenAdr;
	cout << "port:";
	cin >> listenAdr;
	string adr = "tcp://*:";
	adr.append(listenAdr);
	void *recart = zsocket_new(context, ZMQ_REP);
	rc = zsocket_bind (recart, adr.c_str());

	string art = "";
	while(1){
		if (art == ""){
			cout << "1. Search Article. \n";
		}
		else{
			cout << "1. Search Article. \n";
			cout << "2. Edit Article. \n";
		cout << "0. Exit. \n";

		zmq_poll(items, 3, -1);

		//broker
		string input = "";
		cin >> input;

		//search
		if (input == "1"){
			cout << "Name of article: ";
				cin >> nameart;
			art = search(nameart, adr, broker, recart);
			cout << art;
		}
		//Edit
		if ((input == "2") && (art == "")){
			string newart;
			cout << "New article: ";
			cin >> newart;
			edit(newart, broker);
		}
	}
}

string search (string nameart, string adr, void* broker, void* recart){
	zmsg_t *msg = zmsg_new();
	assert(msg);
	op = "search";
	zmsg_addstr(msg, op.c_str());
	zmsg_addstr(msg, nameart.c_str());
	zmsg_addstr(msg, adr.c_str());
	zmsg_send(&msg, broker);

	assert(msg == NULL);
	msg = zmsg_recv(broker);

	char *result = zmsg_popstr(msg);
	char *explanation = zmsg_popstr(msg);
	printf("%s. %s\n", result, explanation);

	if (strcmp(result,"success") == 0){
		string art = zmsg_recv(recart);
		zmsg_t *msgaux = zmsg_new();
		if (art != "")
			zmsg_addstr(msgaux, "success");
		else
			zmsg_addstr(msgaux, "failure");
		zmsg_send(&msgaux, recart);
		free(result);
		free(explanation);
		zmsg_destroy(&msg);
		zmsg_destroy(&msgaux);
		return art;
	}
	else{
		free(result);
		free(explanation);
		zmsg_destroy(&msg);
		return art;
	}
}

void edit (string newart, void* broker){
	zmsg_t *msg = zmsg_new();
	assert(msg);
	op = "edit";
	zmsg_addstr(msg, op.c_str());
	zmsg_addstr(msg, newart.c_str());
	zmsg_send(&msg, broker);

	assert(msg == NULL);
	msg = zmsg_recv(broker);

	char *result = zmsg_popstr(msg);
	char *explanation = zmsg_popstr(msg);
	printf("%s. %s\n", result, explanation);

	free(result);
	free(explanation);
	zmsg_destroy(&msg);
}