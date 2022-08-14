#ifndef _SERVER_H
#define _SERVER_H

#include "network.h"

class Server{
	Host host;
public:
	Server(unsigned short port, channel_id_t channels);
	
	NetEvent get_event(unsigned int timeout);
	
	void send_all(channel_id_t channel, string msg, bool reliable);
	
	void close();
	
	set<Peer*> get_peers();
};

#endif
