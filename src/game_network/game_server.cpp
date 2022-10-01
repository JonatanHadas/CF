#include "game_server.h"

#include "protocol.h"

#include "../game/game_logic.h"

#include "../utils/serialization.h"

#include <sstream>

GameServerObserver::GameServerObserver(Peer& peer) : peer(peer) {}

void GameServerObserver::send(string message){
	peer.send(CHANNEL, message, true);
}

void GameServerObserver::init(const vector<vector<PlayerPosition>>& histories, int round_timer){
	ostringstream packet;
	write_raw(packet, MessageType::GAME);
	write_raw(packet, GameObserverMessageType::INIT);
	
	write_vector<vector<PlayerPosition>>(packet, histories, [](ostream& output, const vector<PlayerPosition>& history){
		write_vector<PlayerPosition>(output, history, [](ostream& output, const PlayerPosition& position){
			position.serialize(output);
		});
	});
	
	write_raw(packet, round_timer);
	
	send(packet.str());
}

void GameServerObserver::new_round(int round){
	ostringstream packet;
	write_raw(packet, MessageType::GAME);
	write_raw(packet, GameObserverMessageType::NEW_ROUND);
	
	write_raw(packet, round);
	
	send(packet.str());
}

void GameServerObserver::update_scores(const vector<int>& addition){
	ostringstream packet;
	write_raw(packet, MessageType::GAME);
	write_raw(packet, GameObserverMessageType::UPDATE_SCORES);
	
	write_vector<int>(packet, addition, write_raw<int>);
	
	send(packet.str());
}

void GameServerObserver::update(const vector<PlayerPosition>& positions, const vector<PlayerState>& states){
	ostringstream packet;
	write_raw(packet, MessageType::GAME);
	write_raw(packet, GameObserverMessageType::UPDATE);
	
	write_vector<PlayerPosition>(packet, positions, [](ostream& output, const PlayerPosition& position){
		position.serialize(output);
	});

	write_vector<PlayerState>(packet, states, [](ostream& output, const PlayerState& state){
		state.serialize(output);
	});

	send(packet.str());
}

void GameServerObserver::spawn_powerup(int id, const PowerUp& power_up){
	ostringstream packet;
	write_raw(packet, MessageType::GAME);
	write_raw(packet, GameObserverMessageType::SPAWN_POWERUP);
	
	write_raw(packet, id);
	power_up.serialize(packet);
	
	send(packet.str());
}

void GameServerObserver::activate_powerup(int id, const PowerUpEffect& effect){
	ostringstream packet;
	write_raw(packet, MessageType::GAME);
	write_raw(packet, GameObserverMessageType::ACTIVATE_POWERUP);
	
	write_raw(packet, id);
	effect.serialize(packet);
	
	send(packet.str());
}

GameServer::GameServer(
	const GameSettings& settings,
	Server& server,
	const map<Peer*, GameSettingsView*>& peers,
	function<void(const NetEvent&)> on_connect,
	function<void(const NetEvent&)> on_disconnect
) : 
	game(
		get_board_size(settings.teams.size()),
		settings.scores,
		settings.get_team_num(), settings.get_teams(),
		settings.allowed_powerups,
		set<GameObserver*>()
	),
	on_connect(on_connect), on_disconnect(on_disconnect),
	server(server) {
	
	for(const auto& entry: peers){
		peer_players[entry.first] = vector<PlayerInterface*>();
		
		for(auto player: entry.second->get_my_players()){
			peer_players[entry.first].push_back(&game.get_player_interface(player));
		}
		
		peer_observers[entry.first] = make_unique<GameServerObserver>(*entry.first);
		game.add_observer(peer_observers[entry.first].get());
	}
}

void GameServer::handle_connection(const NetEvent& event){
	on_connect(event);

	peer_players[event.get_peer()] = vector<PlayerInterface*>();

	peer_observers[event.get_peer()] = make_unique<GameServerObserver>(*event.get_peer());
	game.add_observer(peer_observers[event.get_peer()].get());
}

void GameServer::handle_disconnection(const NetEvent& event){
	game.remove_observer(peer_observers[event.get_peer()].get());
	peer_observers.erase(event.get_peer());
	
	for(auto player: peer_players[event.get_peer()]) player->set_active(false);
	peer_players.erase(event.get_peer());

	on_disconnect(event);
}

void GameServer::handle_message(const NetEvent& event){
	istringstream packet(event.get_packet());
	
	if(read_raw<MessageType>(packet) != MessageType::GAME) return;

	auto index = read_raw<int>(packet);

	if(peer_players[event.get_peer()].size() <= index) return;
	
	switch(read_raw<PlayerInterfaceMessageType>(packet)){
	case PlayerInterfaceMessageType::STEP:
		step(peer_players[event.get_peer()][index], packet);
		break;
	case PlayerInterfaceMessageType::SET_ACTIVE:
		set_active(peer_players[event.get_peer()][index], packet);
		break;
	}
}

void GameServer::step(PlayerInterface* player, istream& input){
	auto round = read_raw<int>(input);
	auto turn_state = read_raw<int>(input);
	
	player->step(round, turn_state);
}

void GameServer::set_active(PlayerInterface* player, istream& input){
	auto active = read_raw<bool>(input);
	
	player->set_active(active);
}

void GameServer::serve(int interval){	
	cout << this << " " << &game << endl;
	while(true){
		auto event = server.get_event(interval);
		switch(event.get_type()){
		case EventType::NONE:
			break;
		case EventType::CONNECT:
			handle_connection(event);
			break;
		case EventType::DISCONNECT:
			handle_disconnection(event);
			break;
		case EventType::RECEIVE:
			handle_message(event);
			break;
		case EventType::NET_ERROR:
			return;
		}
		
		game.advance();
		if(game.is_over()) return;
		
	}
}
