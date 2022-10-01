#include "game_client.h"

#include "protocol.h"

#include "../utils/serialization.h"

#include <sstream>

GameClientPlayer::GameClientPlayer(Client& client, int index) : client(client), index(index) {}

void GameClientPlayer::send(const string& message){
	client.send(CHANNEL, message, true);
}

void GameClientPlayer::step(int round, int turn_state){
	ostringstream packet;
	write_raw(packet, MessageType::GAME);
	write_raw(packet, index);
	write_raw(packet, PlayerInterfaceMessageType::STEP);
	
	write_raw(packet, round);
	write_raw(packet, turn_state);

	send(packet.str());
}

void GameClientPlayer::set_active(bool is_active){
	ostringstream packet;
	write_raw(packet, MessageType::GAME);
	write_raw(packet, index);
	write_raw(packet, PlayerInterfaceMessageType::SET_ACTIVE);
	
	write_raw(packet, is_active);

	send(packet.str());
}


GameClient::GameClient(
	Client& client,
	GameObserver& observer,
	int player_num,
	function<void(const string&)> handle_settings_message
) :
	client(client),
	observer(observer),
	handle_settings_message(handle_settings_message) {

	for(int i = 0; i < player_num; i++){
		players.push_back(GameClientPlayer(client, i));
	}
	for(auto& player: players){
		player_ptrs.push_back(&player);
	}
}

const vector<PlayerInterface*>&	GameClient::get_players(){
	return player_ptrs;
}

bool GameClient::handle_messages(){
	bool idle = false;
	ReceivedMsg message;
	while(!idle){
		if(client.recv(0, message, idle)){
			if(handle_message(message.msg)) return true;
		}
	}
	return false;
}

bool GameClient::handle_message(const string& message){
	istringstream packet(message);

	switch(read_raw<MessageType>(packet)){
	case MessageType::GAME:
		break;
	case MessageType::SETTINGS:
		handle_settings_message(message);
	default:
		return false;
	}

	switch(read_raw<GameObserverMessageType>(packet)){
	case GameObserverMessageType::INIT:
		init(observer, packet);
		break;
	case GameObserverMessageType::NEW_ROUND:
		new_round(observer, packet);
		return true;
	case GameObserverMessageType::UPDATE_SCORES:
		update_scores(observer, packet);
		break;
	case GameObserverMessageType::UPDATE:
		update(observer, packet);
		break;
	case GameObserverMessageType::SPAWN_POWERUP:
		spawn_powerup(observer, packet);
		break;
	case GameObserverMessageType::ACTIVATE_POWERUP:
		activate_powerup(observer, packet);
		break;
	};
	return false;
}

void GameClient::init(GameObserver& observer, istream& input){
	auto histories = read_vector<vector<PlayerPosition>>(input, [](istream& input){
		return read_vector<PlayerPosition>(input, [](istream& input){
			return PlayerPosition::deserialize(input);
		});
	});
	auto round_timer = read_raw<int>(input);
	
	observer.init(histories, round_timer);
}

void GameClient::new_round(GameObserver& observer, istream& input){
	auto round = read_raw<int>(input);
	
	observer.new_round(round);
}

void GameClient::update_scores(GameObserver& observer, istream& input){
	auto addition = read_vector<int>(input, read_raw<int>);
	
	observer.update_scores(addition);
}

void GameClient::update(GameObserver& observer, istream& input){
	auto positions = read_vector<PlayerPosition>(input, [](istream& input){
		return PlayerPosition::deserialize(input);
	});
	auto states = read_vector<PlayerState>(input, [](istream& input){
		return PlayerState::deserialize(input);
	});
	
	observer.update(positions, states);
}

void GameClient::spawn_powerup(GameObserver& observer, istream& input){
	auto id = read_raw<int>(input);
	auto power_up = PowerUp::deserialize(input);

	observer.spawn_powerup(id, power_up);
}

void GameClient::activate_powerup(GameObserver& observer, istream& input){
	auto id = read_raw<int>(input);
	auto effect = PowerUpEffect::deserialize(input);

	observer.activate_powerup(id, effect);
}
