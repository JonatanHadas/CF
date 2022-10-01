#include "game_settings_server.h"

#include "game_server.h"

#include "protocol.h"

#include "../utils/serialization.h"

#include <sstream>

ServerSettingsObserver::ServerSettingsObserver(Peer& peer) : peer(peer) {}

void ServerSettingsObserver::init(const GameSettings& settings){
	ostringstream packet;
	write_raw(packet, MessageType::SETTINGS);
	write_raw(packet, SettingsObserverMessageType::INIT);
	
	settings.serialize(packet);
	
	send(packet.str());
}

void ServerSettingsObserver::add_player(int team, int color){
	ostringstream packet;
	write_raw(packet, MessageType::SETTINGS);
	write_raw(packet, SettingsObserverMessageType::ADD_PLAYER);
	
	write_raw(packet, team);
	write_raw(packet, color);
	
	send(packet.str());
}

void ServerSettingsObserver::remove_player(int player){
	ostringstream packet;
	write_raw(packet, MessageType::SETTINGS);
	write_raw(packet, SettingsObserverMessageType::REMOVE_PLAYER);
	
	write_raw(packet, player);
	
	send(packet.str());
}

void ServerSettingsObserver::set_player_index(int index, int player){
	ostringstream packet;
	write_raw(packet, MessageType::SETTINGS);
	write_raw(packet, SettingsObserverMessageType::SET_PLAYER_INDEX);
	
	write_raw(packet, index);
	write_raw(packet, player);
	
	send(packet.str());
}

void ServerSettingsObserver::set_player_name(int player, string name){
	ostringstream packet;
	write_raw(packet, MessageType::SETTINGS);
	write_raw(packet, SettingsObserverMessageType::SET_PLAYER_NAME);
	
	write_raw(packet, player);
	write_string(packet, name);
	
	send(packet.str());
}

void ServerSettingsObserver::set_player_color(int player, int color){
	ostringstream packet;
	write_raw(packet, MessageType::SETTINGS);
	write_raw(packet, SettingsObserverMessageType::SET_PLAYER_COLOR);

	write_raw(packet, player);
	write_raw(packet, color);
	
	send(packet.str());
}

void ServerSettingsObserver::set_teams(bool using_teams){
	ostringstream packet;
	write_raw(packet, MessageType::SETTINGS);
	write_raw(packet, SettingsObserverMessageType::SET_TEAMS);
	
	write_raw(packet, using_teams);
	
	send(packet.str());
}

void ServerSettingsObserver::add_team(){
	ostringstream packet;
	write_raw(packet, MessageType::SETTINGS);
	write_raw(packet, SettingsObserverMessageType::ADD_TEAM);
	
	send(packet.str());
}

void ServerSettingsObserver::remove_team(int team_num){
	ostringstream packet;
	write_raw(packet, MessageType::SETTINGS);
	write_raw(packet, SettingsObserverMessageType::REMOVE_TEAM);
	
	write_raw(packet, team_num);
	
	send(packet.str());
}

void ServerSettingsObserver::set_player_team(int player, int team){
	ostringstream packet;
	write_raw(packet, MessageType::SETTINGS);
	write_raw(packet, SettingsObserverMessageType::SET_PLAYER_TEAM);
	
	write_raw(packet, player);
	write_raw(packet, team);
	
	send(packet.str());
}

void ServerSettingsObserver::set_team_name(int team, string name){
	ostringstream packet;
	write_raw(packet, MessageType::SETTINGS);
	write_raw(packet, SettingsObserverMessageType::SET_TEAM_NAME);
	
	write_raw(packet, team);
	write_string(packet, name);
	
	send(packet.str());
}

void ServerSettingsObserver::set_allowed_powerup(PowerUpDescriptor desc, bool allowed){
	ostringstream packet;
	write_raw(packet, MessageType::SETTINGS);
	write_raw(packet, SettingsObserverMessageType::SET_ALLOWED_POWERUP);
	
	desc.serialize(packet);
	write_raw(packet, allowed);
	
	send(packet.str());
}

void ServerSettingsObserver::set_win_criterion(WinCriterion criterion){
	ostringstream packet;
	write_raw(packet, MessageType::SETTINGS);
	write_raw(packet, SettingsObserverMessageType::SET_WIN_CRITERION);
	
	write_raw(packet, criterion);
	
	send(packet.str());
}

void ServerSettingsObserver::set_win_amount(int amount){
	ostringstream packet;
	write_raw(packet, MessageType::SETTINGS);
	write_raw(packet, SettingsObserverMessageType::SET_WIN_AMOUNT);
	
	write_raw(packet, amount);
	
	send(packet.str());
}

void ServerSettingsObserver::set_tie_break(int threshold){
	ostringstream packet;
	write_raw(packet, MessageType::SETTINGS);
	write_raw(packet, SettingsObserverMessageType::SET_TIE_BREAK);
	
	write_raw(packet, threshold);
	
	send(packet.str());
}

void ServerSettingsObserver::set_host_player(int player){
	ostringstream packet;
	write_raw(packet, MessageType::SETTINGS);
	write_raw(packet, SettingsObserverMessageType::SET_HOST_PLAYER);
	
	write_raw(packet, player);
	
	send(packet.str());
}

void ServerSettingsObserver::set_host(){
	ostringstream packet;
	write_raw(packet, MessageType::SETTINGS);
	write_raw(packet, SettingsObserverMessageType::SET_HOST);
	
	send(packet.str());
}

void ServerSettingsObserver::player_ready(int player, bool is_ready){
	ostringstream packet;
	write_raw(packet, MessageType::SETTINGS);
	write_raw(packet, SettingsObserverMessageType::PLAYER_READY);
	
	write_raw(packet, player);
	write_raw(packet, is_ready);
	
	send(packet.str());
}

void ServerSettingsObserver::reset_all_ready(){
	ostringstream packet;
	write_raw(packet, MessageType::SETTINGS);
	write_raw(packet, SettingsObserverMessageType::RESET_ALL_READY);
	
	send(packet.str());
}
void ServerSettingsObserver::start_countdown(){
	ostringstream packet;
	write_raw(packet, MessageType::SETTINGS);
	write_raw(packet, SettingsObserverMessageType::START_COUNTDOWN);
	
	send(packet.str());
}

void ServerSettingsObserver::start_game(){
	ostringstream packet;
	write_raw(packet, MessageType::SETTINGS);
	write_raw(packet, SettingsObserverMessageType::START_GAME);
	
	send(packet.str());
}

void ServerSettingsObserver::send(string message){
	peer.send(CHANNEL, message, true);
}


GameSettingsServer::GameSettingsServer(const GameSettings& settings) :
	server(PORT, CHANNEL_NUM),
	settings(settings) {}

void GameSettingsServer::handle_connection(const NetEvent& event){
	peers[event.get_peer()] = settings.create_peer();
	peer_observers[event.get_peer()] = make_unique<ServerSettingsObserver>(*event.get_peer());
	
	peers[event.get_peer()]->add_observer(peer_observers[event.get_peer()].get());
}

void GameSettingsServer::handle_disconnection(const NetEvent& event){
	peers[event.get_peer()]->remove_observer(peer_observers[event.get_peer()].get());
	settings.remove_peer(peers[event.get_peer()]);
	
	peer_observers.erase(event.get_peer());
	peers.erase(event.get_peer());
}

void GameSettingsServer::handle_message(const NetEvent& event){
	istringstream packet(event.get_packet());
	GameSettingsManipulator* manipulator = peers[event.get_peer()];
	
	if(read_raw<MessageType>(packet) != MessageType::SETTINGS) return;
	
	switch(read_raw<SettingsManipulatorMessageType>(packet)){
	case SettingsManipulatorMessageType::ADD_PLAYER:
		add_player(manipulator, packet);
		break;
	case SettingsManipulatorMessageType::REMOVE_PLAYER:
		remove_player(manipulator, packet);
		break;
	case SettingsManipulatorMessageType::SET_PLAYER_NAME:
		set_player_name(manipulator, packet);
		break;
	case SettingsManipulatorMessageType::SET_PLAYER_COLOR:
		set_player_color(manipulator, packet);
		break;
	case SettingsManipulatorMessageType::SET_TEAMS:
		set_teams(manipulator, packet);
		break;
	case SettingsManipulatorMessageType::ADD_TEAM:
		add_team(manipulator, packet);
		break;
	case SettingsManipulatorMessageType::REMOVE_TEAM:
		remove_team(manipulator, packet);
		break;
	case SettingsManipulatorMessageType::SET_PLAYER_TEAM:
		set_player_team(manipulator, packet);
		break;
	case SettingsManipulatorMessageType::SET_TEAM_NAME:
		set_team_name(manipulator, packet);
		break;
	case SettingsManipulatorMessageType::SET_ALLOWED_POWERUP:
		set_allowed_powerup(manipulator, packet);
		break;
	case SettingsManipulatorMessageType::SET_WIN_CRITERION:
		set_win_criterion(manipulator, packet);
		break;
	case SettingsManipulatorMessageType::SET_WIN_AMOUNT:
		set_win_amount(manipulator, packet);
		break;
	case SettingsManipulatorMessageType::SET_TIE_BREAK:
		set_tie_break(manipulator, packet);
		break;
	case SettingsManipulatorMessageType::SET_READY:
		set_ready(manipulator, packet);
		break;
	case SettingsManipulatorMessageType::START_COUNTDOWN:
		start_countdown(manipulator, packet);
		break;
	case SettingsManipulatorMessageType::START_GAME:
		start_game(manipulator, packet);
		break;
	}
}

void GameSettingsServer::add_player(GameSettingsManipulator* manipulator, istream& input){
	manipulator->add_player();
}
void GameSettingsServer::remove_player(GameSettingsManipulator* manipulator, istream& input){
	auto index = read_raw<int>(input);
	manipulator->remove_player(index);
}

void GameSettingsServer::set_player_name(GameSettingsManipulator* manipulator, istream& input){
	auto index = read_raw<int>(input);
	auto name = read_string(input);
	manipulator->set_player_name(index, name);
}
void GameSettingsServer::set_player_color(GameSettingsManipulator* manipulator, istream& input){
	auto index = read_raw<int>(input);
	auto color = read_raw<int>(input);
	manipulator->set_player_color(index, color);
}

void GameSettingsServer::set_teams(GameSettingsManipulator* manipulator, istream& input){
	auto using_teams = read_raw<bool>(input);
	manipulator->set_teams(using_teams);
}
void GameSettingsServer::add_team(GameSettingsManipulator* manipulator, istream& input){
	manipulator->add_team();
}
void GameSettingsServer::remove_team(GameSettingsManipulator* manipulator, istream& input){
	auto team = read_raw<int>(input);
	manipulator->remove_team(team);
}
void GameSettingsServer::set_player_team(GameSettingsManipulator* manipulator, istream& input){
	auto index = read_raw<int>(input);
	auto team = read_raw<int>(input);
	manipulator->set_player_team(index, team);
}

void GameSettingsServer::set_team_name(GameSettingsManipulator* manipulator, istream& input){
	auto team = read_raw<int>(input);
	auto name = read_string(input);
	manipulator->set_team_name(team, name);
}

void GameSettingsServer::set_allowed_powerup(GameSettingsManipulator* manipulator, istream& input){
	auto desc = PowerUpDescriptor::deserialize(input);
	auto allowed = read_raw<bool>(input);
	manipulator->set_allowed_powerup(desc, allowed);
}

void GameSettingsServer::set_win_criterion(GameSettingsManipulator* manipulator, istream& input){
	auto criterion = read_raw<WinCriterion>(input);
	manipulator->set_win_criterion(criterion);
}
void GameSettingsServer::set_win_amount(GameSettingsManipulator* manipulator, istream& input){
	auto amount = read_raw<int>(input);
	manipulator->set_win_amount(amount);
}
void GameSettingsServer::set_tie_break(GameSettingsManipulator* manipulator, istream& input){
	auto threshold = read_raw<int>(input);
	manipulator->set_tie_break(threshold);
}

void GameSettingsServer::set_ready(GameSettingsManipulator* manipulator, istream& input){
	auto is_ready = read_raw<bool>(input);
	manipulator->set_ready(is_ready);
}
void GameSettingsServer::start_countdown(GameSettingsManipulator* manipulator, istream& input){
	manipulator->start_countdown();
}
void GameSettingsServer::start_game(GameSettingsManipulator* manipulator, istream& input){
	manipulator->start_game();
}

void GameSettingsServer::serve(int interval){
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
		
		if(settings.get_all_starting()){
			map<Peer*, GameSettingsView*> views;
			for(auto entry: peers) views[entry.first] = entry.second;
			settings.start_all();
			GameServer(
				settings.get_settings(),
				server,
				views,
				[&](const NetEvent& event){ 
					handle_connection(event);
					
					peer_observers[event.get_peer()]->start_game();
				},
				[&](const NetEvent& event){
					handle_disconnection(event);
				}
			).serve(interval);
		}
	}
}
