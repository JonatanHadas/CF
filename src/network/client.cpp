#include "client.h"

Client::Client(const ENetAddress& address, channel_id_t channels) : host(channels), peer(nullptr) {
	peer = host.connect(address, channels, 0);
}

void Client::send(channel_id_t channel, string msg, bool reliable){
	peer->send(channel, msg, reliable);
}

bool Client::recv(unsigned int timeout, ReceivedMsg& msg){
	NetEvent event = host.get_event(timeout);
	
	switch(event.get_type()){
	case EventType::CONNECT:
		if(peer != event.get_peer()){
			host.reset(event.get_peer());
		}
		return false;
	case EventType::DISCONNECT:
		if(peer == event.get_peer()){
			peer = nullptr;
		}
		return false;
	case EventType::RECEIVE:
		if(peer != event.get_peer()) return false;
		
		msg.channel = event.get_channel();
		msg.msg = event.get_packet();
		return true;
	default:
		return false;
	}
}

bool Client::is_connected() const{
	return peer != nullptr;
}

void Client::close(){
	if(is_connected()) peer->disconnect_soft(0);
	host.flush();
}
