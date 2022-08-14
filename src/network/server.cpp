#include "server.h"

ENetAddress get_address(unsigned short port){
	ENetAddress address;
	address.host = ENET_HOST_ANY;
	address.port = port;
	
	return address;
}

Server::Server(unsigned short port, channel_id_t channels) :
	host(get_address(port), channels) {}

NetEvent Server::get_event(unsigned int timeout){
	return host.get_event(timeout);
}

void Server::send_all(channel_id_t channel, string msg, bool reliable){
	for(auto peer: host.get_peers()){
		peer->send(channel, msg, reliable);
	}
}

void Server::close(){
	for(auto peer: host.get_peers()){
		peer->disconnect_soft(0);
	}
	
	while(host.get_peer_num()){
		auto event = host.get_event(3000);
		switch(event.get_type()){
		case EventType::CONNECT:
			host.reset(event.get_peer());
			break;
		}
	}
}

set<Peer*> Server::get_peers(){
	return host.get_peers();
}