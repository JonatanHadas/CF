#include "game_settings_client.h"

#include "protocol.h"

#include "../utils/serialization.h"

#include <sstream>

using namespace std;

GameSettingsClient::GameSettingsClient(Client& client, GameSettingsObserver& observer) :
	client(client),
	observer(observer) {}

void GameSettingsClient::send(const string& message){
	client.send(CHANNEL, message, true);
}

bool GameSettingsClient::handle_message(const string& message){
	istringstream packet(message);
	if(read_raw<MessageType>(packet) != MessageType::SETTINGS) return false;

	switch(read_raw<SettingsObserverMessageType>(packet)){
	case SettingsObserverMessageType::INIT:
		init(observer, packet);
		break;
	case SettingsObserverMessageType::ADD_PLAYER:
		add_player(observer, packet);
		break;
	case SettingsObserverMessageType::REMOVE_PLAYER:
		remove_player(observer, packet);
		break;
	case SettingsObserverMessageType::SET_PLAYER_INDEX:
		set_player_index(observer, packet);
		break;
	case SettingsObserverMessageType::SET_PLAYER_NAME:
		set_player_name(observer, packet);
		break;
	case SettingsObserverMessageType::SET_PLAYER_COLOR:
		set_player_color(observer, packet);
		break;
	case SettingsObserverMessageType::SET_TEAMS:
		set_teams(observer, packet);
		break;
	case SettingsObserverMessageType::ADD_TEAM:
		add_team(observer, packet);
		break;
	case SettingsObserverMessageType::REMOVE_TEAM:
		remove_team(observer, packet);
		break;
	case SettingsObserverMessageType::SET_PLAYER_TEAM:
		set_player_team(observer, packet);
		break;
	case SettingsObserverMessageType::SET_TEAM_NAME:
		set_team_name(observer, packet);
		break;
	case SettingsObserverMessageType::SET_ALLOWED_POWERUP:
		set_allowed_powerup(observer, packet);
		break;
	case SettingsObserverMessageType::SET_WIN_CRITERION:
		set_win_criterion(observer, packet);
		break;
	case SettingsObserverMessageType::SET_WIN_AMOUNT:
		set_win_amount(observer, packet);
		break;
	case SettingsObserverMessageType::SET_TIE_BREAK:
		set_tie_break(observer, packet);
		break;
	case SettingsObserverMessageType::SET_HOST_PLAYER:
		set_host_player(observer, packet);
		break;
	case SettingsObserverMessageType::SET_HOST:
		set_host(observer, packet);
		break;
	case SettingsObserverMessageType::PLAYER_READY:
		player_ready(observer, packet);
		break;
	case SettingsObserverMessageType::RESET_ALL_READY:
		reset_all_ready(observer, packet);
		break;
	case SettingsObserverMessageType::START_COUNTDOWN:
		start_countdown(observer, packet);
		break;
	case SettingsObserverMessageType::START_GAME:
		start_game(observer, packet);
		return true;
	};
	return false;
}

void GameSettingsClient::add_player(){
	ostringstream packet;
	write_raw(packet, MessageType::SETTINGS);
	write_raw(packet, SettingsManipulatorMessageType::ADD_PLAYER);
	
	send(packet.str());
}

void GameSettingsClient::remove_player(int index){
	ostringstream packet;
	write_raw(packet, MessageType::SETTINGS);
	write_raw(packet, SettingsManipulatorMessageType::REMOVE_PLAYER);
	
	write_raw(packet, index);
	
	send(packet.str());
}

void GameSettingsClient::set_player_name(int index, string name){
	ostringstream packet;
	write_raw(packet, MessageType::SETTINGS);
	write_raw(packet, SettingsManipulatorMessageType::SET_PLAYER_NAME);
	
	write_raw(packet, index);
	write_string(packet, name);
	
	send(packet.str());
}

void GameSettingsClient::set_player_color(int index, int color){
	ostringstream packet;
	write_raw(packet, MessageType::SETTINGS);
	write_raw(packet, SettingsManipulatorMessageType::SET_PLAYER_COLOR);
	
	write_raw(packet, index);
	write_raw(packet, color);
	
	send(packet.str());
}

void GameSettingsClient::set_teams(bool using_teams){
	ostringstream packet;
	write_raw(packet, MessageType::SETTINGS);
	write_raw(packet, SettingsManipulatorMessageType::SET_TEAMS);
	
	write_raw(packet, using_teams);
	
	send(packet.str());
}

void GameSettingsClient::add_team(){
	ostringstream packet;
	write_raw(packet, MessageType::SETTINGS);
	write_raw(packet, SettingsManipulatorMessageType::ADD_TEAM);
	
	send(packet.str());
}

void GameSettingsClient::remove_team(int team){
	ostringstream packet;
	write_raw(packet, MessageType::SETTINGS);
	write_raw(packet, SettingsManipulatorMessageType::REMOVE_TEAM);
	
	write_raw(packet, team);
	
	send(packet.str());
}

void GameSettingsClient::set_player_team(int index, int team){
	ostringstream packet;
	write_raw(packet, MessageType::SETTINGS);
	write_raw(packet, SettingsManipulatorMessageType::SET_PLAYER_TEAM);
	
	write_raw(packet, index);
	write_raw(packet, team);
	
	send(packet.str());
}

void GameSettingsClient::set_team_name(int team, string name){
	ostringstream packet;
	write_raw(packet, MessageType::SETTINGS);
	write_raw(packet, SettingsManipulatorMessageType::SET_TEAM_NAME);
	
	write_raw(packet, team);
	write_string(packet, name);
	
	send(packet.str());
}

void GameSettingsClient::set_allowed_powerup(PowerUpDescriptor desc, bool allowed){
	ostringstream packet;
	write_raw(packet, MessageType::SETTINGS);
	write_raw(packet, SettingsManipulatorMessageType::SET_ALLOWED_POWERUP);
	
	desc.serialize(packet);
	write_raw(packet, allowed);
	
	send(packet.str());
}

void GameSettingsClient::set_win_criterion(WinCriterion criterion){
	ostringstream packet;
	write_raw(packet, MessageType::SETTINGS);
	write_raw(packet, SettingsManipulatorMessageType::SET_WIN_CRITERION);
	
	write_raw(packet, criterion);
	
	send(packet.str());
}

void GameSettingsClient::set_win_amount(int amount){
	ostringstream packet;
	write_raw(packet, MessageType::SETTINGS);
	write_raw(packet, SettingsManipulatorMessageType::SET_WIN_AMOUNT);
	
	write_raw(packet, amount);
	
	send(packet.str());
}

void GameSettingsClient::set_tie_break(int threshold){
	ostringstream packet;
	write_raw(packet, MessageType::SETTINGS);
	write_raw(packet, SettingsManipulatorMessageType::SET_TIE_BREAK);
	
	write_raw(packet, threshold);
	
	send(packet.str());
}

void GameSettingsClient::set_ready(bool is_ready){
	ostringstream packet;
	write_raw(packet, MessageType::SETTINGS);
	write_raw(packet, SettingsManipulatorMessageType::SET_READY);
	
	write_raw(packet, is_ready);
	
	send(packet.str());
}

void GameSettingsClient::start_countdown(){
	ostringstream packet;
	write_raw(packet, MessageType::SETTINGS);
	write_raw(packet, SettingsManipulatorMessageType::START_COUNTDOWN);
	
	send(packet.str());
}

void GameSettingsClient::start_game(){
	ostringstream packet;
	write_raw(packet, MessageType::SETTINGS);
	write_raw(packet, SettingsManipulatorMessageType::START_GAME);
	
	send(packet.str());
}

void GameSettingsClient::handle_messages(){
	bool idle = false;
	ReceivedMsg message;
	while(!idle){
		if(client.recv(0, message, idle)){
			if(handle_message(message.msg)) return;
		}
	}
}

void GameSettingsClient::init(GameSettingsObserver& observer, istream& input){
	auto settings = GameSettings::deserialize(input);

	observer.init(settings);
}

void GameSettingsClient::add_player(GameSettingsObserver& observer, istream& input){
	auto team = read_raw<int>(input);
	auto color = read_raw<int>(input);

	observer.add_player(team, color);
}

void GameSettingsClient::remove_player(GameSettingsObserver& observer, istream& input){
	auto player = read_raw<int>(input);

	observer.remove_player(player);
}

void GameSettingsClient::set_player_index(GameSettingsObserver& observer, istream& input){
	auto index = read_raw<int>(input);
	auto player = read_raw<int>(input);

	observer.set_player_index(index, player);
}

void GameSettingsClient::set_player_name(GameSettingsObserver& observer, istream& input){
	auto player = read_raw<int>(input);
	auto name = read_string(input);

	observer.set_player_name(player, name);
}

void GameSettingsClient::set_player_color(GameSettingsObserver& observer, istream& input){
	auto player = read_raw<int>(input);
	auto color = read_raw<int>(input);

	observer.set_player_color(player, color);
}

void GameSettingsClient::set_teams(GameSettingsObserver& observer, istream& input){
	auto using_teams = read_raw<bool>(input);

	observer.set_teams(using_teams);
}

void GameSettingsClient::add_team(GameSettingsObserver& observer, istream& input){
	observer.add_team();
}

void GameSettingsClient::remove_team(GameSettingsObserver& observer, istream& input){
	auto team_num = read_raw<int>(input);

	observer.remove_team(team_num);
}

void GameSettingsClient::set_player_team(GameSettingsObserver& observer, istream& input){
	auto player = read_raw<int>(input);
	auto team = read_raw<int>(input);

	observer.set_player_team(player, team);
}

void GameSettingsClient::set_team_name(GameSettingsObserver& observer, istream& input){
	auto team = read_raw<int>(input);
	auto name = read_string(input);

	observer.set_team_name(team, name);
}

void GameSettingsClient::set_allowed_powerup(GameSettingsObserver& observer, istream& input){
	auto desc = PowerUpDescriptor::deserialize(input);
	auto allowed = read_raw<bool>(input);

	observer.set_allowed_powerup(desc, allowed);
}

void GameSettingsClient::set_win_criterion(GameSettingsObserver& observer, istream& input){
	auto criterion = read_raw<WinCriterion>(input);

	observer.set_win_criterion(criterion);
}

void GameSettingsClient::set_win_amount(GameSettingsObserver& observer, istream& input){
	auto amount = read_raw<int>(input);

	observer.set_win_amount(amount);
}

void GameSettingsClient::set_tie_break(GameSettingsObserver& observer, istream& input){
	auto threshold = read_raw<int>(input);

	observer.set_tie_break(threshold);
}

void GameSettingsClient::set_host_player(GameSettingsObserver& observer, istream& input){
	auto player = read_raw<int>(input);

	observer.set_host_player(player);
}

void GameSettingsClient::set_host(GameSettingsObserver& observer, istream& input){
	observer.set_host();
}

void GameSettingsClient::player_ready(GameSettingsObserver& observer, istream& input){
	auto player = read_raw<int>(input);
	auto is_ready = read_raw<bool>(input);

	observer.player_ready(player, is_ready);
}

void GameSettingsClient::reset_all_ready(GameSettingsObserver& observer, istream& input){
	observer.reset_all_ready();
}

void GameSettingsClient::start_countdown(GameSettingsObserver& observer, istream& input){
	observer.start_countdown();
}

void GameSettingsClient::start_game(GameSettingsObserver& observer, istream& input){
    observer.start_game();
}
