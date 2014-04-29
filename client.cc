#include <czmq.h>
#include <string>
#include <stdio.h>
#include <unistd.h>
#include <assert.h>
#include <stdlib.h>
#include <iostream>

using namespace std;

string search (string nameart, void* broker);
void edit (string nameart, string newart, void* broker);


int main (void){
	zctx_t *context = zctx_new();
	//socket broker
	void *broker = zsocket_new(context, ZMQ_REQ);
	int rc = zsocket_connect (broker, "tcp://localhost:12350");
	assert(rc == 0);

	string art = "";
	string nameart = "";
	while(1){
		string input = "";
		if (art == "")
			cout << "1. Search Article. \n";
		else{
			cout << "1. Search Article. \n";
			cout << "2. Edit Article. \n";
		}
		cout << "0. Exit. \n";
		cout << "Option: ";
		cout.flush();
		cin >> input;

		//search
		if (input == "1"){
			cout << "Name of article: ";
			cout.flush();
			cin.clear(); 
			cin.ignore(INT_MAX,'\n');
			getline(cin,nameart);
			art = search(nameart, broker);
			if (art != "")
				cout << "=======================================\n"<< art << "\n=======================================\n";
			else
				cout << "Error finding article \n";
		}
		//Edit
		else if ((input == "2") && (art != "")){
			string newart = "";
			cout << "New article: ";
			cout.flush();
			cin.clear(); 
			cin.ignore(INT_MAX,'\n');
			getline(cin,newart);
			edit(nameart, newart, broker);
			art = "";
		}
		else if (input == "0"){
			zsocket_destroy(context, broker);
			zctx_destroy(&context);
			return 0;
		}
	}
}


string search (string nameart, void* broker){
	zmsg_t *msg = zmsg_new();
	assert(msg);
	string op = "search";
	zmsg_addstr(msg, op.c_str());
	zmsg_addstr(msg, nameart.c_str());
	zmsg_send(&msg, broker);

	assert(msg == NULL);
	msg = zmsg_recv(broker);

	char *result = zmsg_popstr(msg);
	string art = result;
	if (art == "failure")
		art = "";
	free(result);
	zmsg_destroy(&msg);
	return art;
}


void edit (string nameart, string newart, void* broker){
	zmsg_t *msg = zmsg_new();
	assert(msg);
	string op = "edit";
	zmsg_addstr(msg, op.c_str());
	zmsg_addstr(msg, nameart.c_str());
	zmsg_addstr(msg, newart.c_str());
	zmsg_send(&msg, broker);

	assert(msg == NULL);
	msg = zmsg_recv(broker);

	char *result = zmsg_popstr(msg);
	cout << result << "\n";

	free(result);
	zmsg_destroy(&msg);
}