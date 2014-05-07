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
    articles["colombia"]="Republica unitaria de America situada en la region noroccidental de America del Sur";
    articles["casa"]="edificación construida para ser habitada";
    articles["inglaterra"]="una de las cuatro naciones constituyentes del Reino Unido";
    articles["lluvia"]="fenómeno atmosférico de tipo acuático que se inicia con la condensación del vapor de agua contenido en las nubes.";
    articles["1"]="primer número natural y también es el número entero que sigue al cero (0) y precede al dos (2)";
    articles["isaac newton"]="físico, filósofo, teólogo, inventor, alquimista y matemático inglés, autor de los Philosophiae naturalis principia mathematica, más conocidos como los Principia, donde describió la ley de la gravitación universal y estableció las bases de la mecánica clásica mediante las leyes que llevan su nombre";
    articles["musica"]="arte de organizar sensible y lógicamente una combinación coherente de sonidos y silencios utilizando los principios fundamentales de la melodía, la armonía y el ritmo, mediante la intervención de complejos procesos psico-anímicos";
    articles["13"]="número natural que sigue al doce y precede al catorce";

	zctx_t *context = zctx_new();
	//ask user for the address of the bind
	string recvadr;
	cout << "listening port:";
	cin >> recvadr;
	string adr = "tcp://*:";
	adr.append(recvadr);

	//search socket
    void *searchsocket = zsocket_new(context, ZMQ_REP); 
    int pn = zsocket_bind(searchsocket, adr.c_str()); 
    cout << "search port " << pn << "\n"; 

    //edit socket
    int n = atoi(recvadr.c_str()) + 1;
    recvadr = to_string(n);
    adr = "tcp://*:";
    adr.append(recvadr);
    void *editsocket = zsocket_new(context, ZMQ_REP);
    pn = zsocket_bind(editsocket, adr.c_str()); 
    cout << "edit port " << pn << "\n";

    zmq_pollitem_t items [] = {
		{searchsocket, 0, ZMQ_POLLIN, 0},
		{editsocket, 0, ZMQ_POLLIN, 0}
	};

    while(1){
    	zmq_poll(items, 2, -1);
    	//search
    	if  (items[0].revents & ZMQ_POLLIN){
    		zmsg_t *msg = zmsg_new();
    		zmsg_t *response = zmsg_new();
			msg = zmsg_recv(searchsocket);
            cout<<"\nfrom broker \n";
			zmsg_dump(msg);
			char *nameart = zmsg_popstr(msg);
			string name = nameart;
			unordered_map<string,string>::const_iterator got = articles.find(name);
			if (got == articles.end())
				zmsg_addstr(response, "failure");
			else
				zmsg_addstr(response, articles[name].c_str());
			zmsg_destroy(&msg);
            cout<<"\nto broker \n";
            zmsg_dump(response);
			zmsg_send(&response, searchsocket);
            zmsg_destroy(&response);
            free(nameart);
            cout<<"=======task done======= \n";
    	}
    	//edit
    	if  (items[1].revents & ZMQ_POLLIN){
    		zmsg_t *msg = zmsg_new();
    		zmsg_t *response = zmsg_new();
    		msg = zmsg_recv(editsocket);
            cout<<"\nfrom editserver \n";
    		zmsg_dump(msg);
    		char *nart = zmsg_popstr(msg);
    		char *neart = zmsg_popstr(msg);
    		string nameart = nart;
    		string newart = neart;
			articles[nameart] = newart;
            cout<<"new article "<<nameart<<": "<<articles[nameart]<<"\n";
    		zmsg_addstr(response, "success");
    		zmsg_destroy(&msg);
            cout<<"\nto editserver \n";
            zmsg_dump(response);
    		zmsg_send(&response, editsocket);
            zmsg_destroy(&response);
            free(nart);
            free(neart);
            cout<<"=======task done======= \n";
    	}
	}
	zsocket_destroy(context, searchsocket);
	zsocket_destroy(context, editsocket);
  	zctx_destroy(&context);
  	return 0;
}