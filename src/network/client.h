#ifndef _CLIENT_H
#define _CLIENT_H

#include "network.h"

struct ReceivedMsg{
	string msg;
	channel_id_t channel;
};

class Client{
	Host host;
	Peer* peer;
public:
	Client(const ENetAddress& address, channel_id_t channels);
	
	void send(channel_id_t channel, string msg, bool reliable);
	bool recv(unsigned int timeout, ReceivedMsg& msg);
	
	bool is_connected() const;
	
	void close();
};

#endif
