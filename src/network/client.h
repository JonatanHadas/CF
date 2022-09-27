#ifndef _CLIENT_H
#define _CLIENT_H

#include "network.h"

#include <memory>

using namespace std;

struct ReceivedMsg{
	string msg;
	channel_id_t channel;
};

class Client{
	unique_ptr<Host> host;
	Peer* peer;
	
	bool connected;
public:
	Client(const ENetAddress& address, channel_id_t channels);
	
	void send(channel_id_t channel, string msg, bool reliable);
	bool recv(unsigned int timeout, ReceivedMsg& msg, bool& idle);
	
	bool is_connected() const;
	bool is_open() const;
	
	void close();
};

#endif
