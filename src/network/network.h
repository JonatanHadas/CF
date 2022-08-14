#ifndef _NETWORK_H
#define _NETWORK_H

#include <enet/enet.h>

#include <string>
#include <map>
#include <set>
#include <memory>

using namespace std;

class Host;

typedef unsigned char channel_id_t;

class Peer{
	ENetPeer* peer;

	Host* host;

	friend Host;

	void disconnect_now(unsigned int data);
	void clear();
public:
	Peer(ENetPeer* peer, Host* host);

	Peer(const Peer&) = delete;
	Peer(Peer&& src);

	~Peer();

	Peer& operator=(const Peer&) = delete;
	Peer& operator=(Peer&& src);

	void send(channel_id_t channel, string msg, bool reliable);

	void disconnect(unsigned int data);
	void disconnect_soft(unsigned int data);

	bool is_closed() const;

	const ENetAddress& get_address() const;
};

enum class EventType{
	NONE,
	NET_ERROR,
	CONNECT,
	RECEIVE,
	DISCONNECT,
};

class NetEvent{
	EventType type;
	channel_id_t channel;
	unsigned int data;
	unique_ptr<string> packet;
	unique_ptr<Peer> saved_peer;
	Peer* peer;

	NetEvent(
		EventType type,
		channel_id_t channel,
		unsigned int data,
		unique_ptr<string>&& packet,
		unique_ptr<Peer>&& saved_peer,
		Peer* peer
	);
public:
	NetEvent(
		EventType type,
		channel_id_t channel,
		unsigned int data,
		string&& packet,
		Peer* peer
	);

	NetEvent(
		EventType type,
		unsigned int data,
		Peer* peer
	);

	NetEvent(
		EventType type,
		unsigned int data,
		unique_ptr<Peer>&& peer
	);

	NetEvent();

	EventType get_type() const;
	channel_id_t get_channel() const;
	unsigned int get_data() const;
	const string& get_packet() const;
	Peer* get_peer() const;
};

class Host{
	ENetHost* host;

	map<ENetPeer*, unique_ptr<Peer>> peers;

	void destory();

	Host(const ENetAddress* address, channel_id_t channels);
public:
	Host(const ENetAddress& address, channel_id_t channels);
	Host(channel_id_t channels);

	Host(const Host&) = delete;
	Host(Host&& src);

	~Host();

	Host& operator=(const Host&) = delete;
	Host& operator=(Host&& src);

	void flush();
	NetEvent get_event(unsigned int timeout);
	Peer* connect(const ENetAddress& address, channel_id_t channels, unsigned int data);

	set<Peer*> get_peers();
	size_t get_peer_num();

	void reset(Peer* peer);
};

#endif
