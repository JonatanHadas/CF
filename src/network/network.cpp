#include "network.h"

void Peer::disconnect_now(unsigned int data){
	if(peer != nullptr) enet_peer_disconnect_now(peer, data);
	peer = nullptr;
}
void Peer::clear(){
	peer = nullptr;
}

Peer::Peer(ENetPeer* peer, Host* host) : peer(peer), host(host) {}

Peer::Peer(Peer&& src) : peer(src.peer){
	src.peer = nullptr;
}

Peer::~Peer(){
	disconnect_now(0);
	host->flush();
}

Peer& Peer::operator=(Peer&& src){
	disconnect_now(0);

	peer = src.peer;
	src.peer = nullptr;

	return *this;
}

void Peer::send(channel_id_t channel, string msg, bool reliable){
	if(peer != nullptr) enet_peer_send(peer, channel, enet_packet_create(msg.data(), msg.size(), reliable ? ENET_PACKET_FLAG_RELIABLE : 0));
	host->flush();
}

void Peer::disconnect(unsigned int data){
	if(peer != nullptr) enet_peer_disconnect(peer, data);
	peer = nullptr;
}
void Peer::disconnect_soft(unsigned int data){
	if(peer != nullptr) enet_peer_disconnect_later(peer, data);
	peer = nullptr;
}

bool Peer::is_closed() const{
	return nullptr == peer;
}

const ENetAddress& Peer::get_address() const{
	return peer->address;
}

NetEvent::NetEvent(
	EventType type,
	channel_id_t channel,
	unsigned int data,
	unique_ptr<string>&& packet,
	unique_ptr<Peer>&& saved_peer,
	Peer* peer
) : type(type), channel(channel), data(data), packet(move(packet)), saved_peer(move(saved_peer)), peer(peer) {}

NetEvent::NetEvent(
	EventType type,
	channel_id_t channel,
	unsigned int data,
	string&& packet,
	Peer* peer
) : NetEvent(type, channel, data, make_unique<string>(packet), nullptr, peer) {}

NetEvent::NetEvent(
	EventType type,
	unsigned int data,
	Peer* peer
) : NetEvent(type, 0, data, nullptr, nullptr, peer) {}

NetEvent::NetEvent(
	EventType type,
	unsigned int data,
	unique_ptr<Peer>&& peer
) : NetEvent(type, 0, data, nullptr, move(peer), peer.get()) {}

NetEvent::NetEvent() : NetEvent(EventType::NONE, 0, nullptr) {}

EventType NetEvent::get_type() const{
	return type;
}

channel_id_t NetEvent::get_channel() const{
	return channel;
}

unsigned int NetEvent::get_data() const{
	return data;
}

const string& NetEvent::get_packet() const{
	return *packet;
}

Peer* NetEvent::get_peer() const{
	return peer;
}


void Host::destory(){
	peers.clear();
	connecting_peers.clear();

	if(host != nullptr) enet_host_destroy(host);
	host = nullptr;
}

#define PEER_NUM 32

Host::Host(const ENetAddress* address, channel_id_t channels) :
	host(enet_host_create(address, PEER_NUM, channels, 0, 0)) {}

Host::Host(const ENetAddress& address, channel_id_t channels) : Host(&address, channels) {}

Host::Host(channel_id_t channels) : Host(nullptr, channels) {}

Host::Host(Host&& src) : host(src.host){
	for(auto& entry: src.peers){
		peers[entry.first] = std::move(entry.second);
	}
	src.peers.clear();

	for(auto& entry: src.connecting_peers){
		connecting_peers[entry.first] = std::move(entry.second);
	}
	src.connecting_peers.clear();

	src.host = nullptr;
}

Host::~Host(){
	destory();
}

Host& Host::operator=(Host&& src){
	destory();

	for(auto& entry: src.peers){
		peers[entry.first] = std::move(entry.second);
	}
	src.peers.clear();

	for(auto& entry: src.connecting_peers){
		connecting_peers[entry.first] = std::move(entry.second);
	}
	src.connecting_peers.clear();

	host = src.host;
	src.host = nullptr;

	return *this;
}

void Host::flush(){
	enet_host_flush(host);
}
NetEvent Host::get_event(unsigned int timeout){
	ENetEvent event;
	int code = enet_host_service(host, &event, timeout);
	
	if(code < 0){
		return NetEvent(EventType::NET_ERROR, 0, nullptr);
	}
	if(code == 0){
		return NetEvent();
	}

	switch(event.type){
	case ENET_EVENT_TYPE_CONNECT:
		{
			unique_ptr<Peer> peer;
			if(connecting_peers.count(event.peer)){
				peer = std::move(connecting_peers[event.peer]);
				connecting_peers.erase(event.peer);
			}
			else peer = make_unique<Peer>(event.peer, this);
			
			peers.insert({event.peer, std::move(peer)});
			return NetEvent(EventType::CONNECT, event.data, peers[event.peer].get());
		}
	case ENET_EVENT_TYPE_DISCONNECT:
		{
			auto peer = move(peers[event.peer]);
			peers.erase(event.peer);

			peer->clear();
			return NetEvent(EventType::DISCONNECT, event.data, move(peer));
		}
	case ENET_EVENT_TYPE_RECEIVE:
		{
			string msg((char*)event.packet->data, event.packet->dataLength);

			enet_packet_destroy(event.packet);

			return NetEvent(EventType::RECEIVE, event.channelID, event.data, move(msg), peers[event.peer].get());
		}
	default:
		return NetEvent();
	}
}

Peer* Host::connect(const ENetAddress& address, channel_id_t channels, unsigned int data){
	ENetPeer* peer = enet_host_connect(host, &address, channels, data);

	if(nullptr == peer) return nullptr;

	connecting_peers.insert({peer, make_unique<Peer>(peer, this)});
	
	return connecting_peers[peer].get();
}

set<Peer*> Host::get_peers(){
	set<Peer*> peer_ptrs;
	for(auto& entry: peers){
		peer_ptrs.insert(entry.second.get());
	}
	return peer_ptrs;
}

size_t Host::get_peer_num(){
	return peers.size();
}

void Host::reset(Peer* peer){
	enet_peer_reset(peer->peer);
	peers.erase(peer->peer);
}