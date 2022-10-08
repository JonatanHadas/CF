#include "game_settings_manager.h"

#include "../utils/utils.h"

GameSettingsManager::Peer::Peer(GameSettingsManager& manager) :
	manager(manager),
	ready(false), starting(false) {}

GameSettingsManager::Peer::~Peer(){
	clear_observers();
	
	while(players.size()){
		int player = players.back();
		players.pop_back();
		manager.remove_player(player);
	}
}

// Manipulator
void GameSettingsManager::Peer::add_player(){
	set_ready(false);
	
	int player = manager.add_player();
	int index = players.size();
	
	players.push_back(player);
	
	manager.do_with_observers([&](GameSettingsObserver& observer){
		observer.player_ready(player, ready);
	});

	if(manager.host == this) manager.do_with_observers([&](GameSettingsObserver& observer){
		observer.set_host_player(player);
	});

	for(auto observer: get_observers()){
		observer->set_player_index(index, player);
	}
}

void GameSettingsManager::Peer::remove_player(int index){
	if(index < players.size()){
		set_ready(false);
		
		int player = pop_index(players, index);
		manager.remove_player(player);
	}
}

void GameSettingsManager::Peer::set_player_name(int index, string name){
	if(index < players.size()){
		set_ready(false);
		
		manager.set_player_name(players[index], name);
	}
}

void GameSettingsManager::Peer::set_player_color(int index, int color){
	if(index < players.size()){
		set_ready(false);
		
		manager.set_player_color(players[index], color);
	}
}

void GameSettingsManager::Peer::set_teams(bool using_teams){
	if(manager.host == this){
		manager.set_teams(using_teams);
	}
}

void GameSettingsManager::Peer::add_team(){
	if(manager.host == this){
		manager.add_team();
	}
}

void GameSettingsManager::Peer::remove_team(int team){
	if(manager.host == this){
		manager.remove_team(team);
	}
}

void GameSettingsManager::Peer::set_player_team(int index, int team){
	if(index < players.size()){
		set_ready(false);
		
		manager.set_player_team(players[index], team);
	}
}

void GameSettingsManager::Peer::set_team_name(int team, string name){
	for(auto player: players){
		if(manager.settings.teams[player] == team){
			manager.set_team_name(team, name);
			return;
		}
	}
}

void GameSettingsManager::Peer::set_allowed_powerup(PowerUpDescriptor desc, bool allowed){
	if(manager.host == this){
		manager.set_allowed_powerup(desc, allowed);
	}
}

void GameSettingsManager::Peer::set_win_criterion(WinCriterion criterion){
	if(manager.host == this){
		manager.set_win_criterion(criterion);
	}
}

void GameSettingsManager::Peer::set_win_amount(int amount){
	if(manager.host == this){
		manager.set_win_amount(amount);
	}
}

void GameSettingsManager::Peer::set_tie_break(int threshold){
	if(manager.host == this){
		manager.set_tie_break(threshold);
	}
}

void GameSettingsManager::Peer::set_ready(bool is_ready){
	ready = is_ready;
	if(!ready) {
		for(auto& peer: manager.peers) peer->starting = false;
		manager.counting_down = false;
	}
	
	for(auto player: players){
		manager.set_player_ready(player, is_ready);
	}
}

void GameSettingsManager::Peer::start_countdown(){
	if(manager.host == this){
		manager.start_countdown();
	}
}

void GameSettingsManager::Peer::start_game(){
	starting = true;
}

// View
const GameSettings& GameSettingsManager::Peer::get_settings() const{
	return manager.settings;
}

const vector<int>& GameSettingsManager::Peer::get_my_players() const{
	return players;
}

const vector<int>& GameSettingsManager::Peer::get_host_players() const{
	return manager.host->players;
}

set<int> GameSettingsManager::Peer::get_ready() const{
	set<int> players;

	for(auto& peer: manager.peers){
		if(peer.get() != manager.host && !peer->ready) continue;
		for(auto player: peer->players){
			players.insert(player);
		}
	}
	
	return players;
}

bool GameSettingsManager::Peer::am_i_host() const{
	return manager.host == this;
}

bool GameSettingsManager::Peer::am_i_ready() const{
	return ready || am_i_host();
}

bool GameSettingsManager::Peer::is_counting_down() const{
	return manager.counting_down;
}

void GameSettingsManager::Peer::init_observer(GameSettingsObserver* observer){
	observer->init(manager.settings);
	
	for(int i = 0; i < players.size(); i++){
		observer->set_player_index(i, players[i]);
	}
	
	for(auto& peer: manager.peers){
		for(auto player: peer->players){
			observer->player_ready(player, peer->ready);
		}
	}
	for(auto player: manager.host->players){
		observer->set_host_player(player);
	}
	
	if(manager.host == this) observer->set_host();
	if(manager.counting_down) observer->start_countdown();
}


GameSettingsManager::GameSettingsManager(const GameSettings& settings) :
	settings(settings),
	counting_down(false),
	host(nullptr) {}

GameSettingsManager::Peer* GameSettingsManager::create_peer(){
	auto peer = make_unique<GameSettingsManager::Peer>(*this);
	auto ptr = peer.get();
	
	if(host == nullptr) {
		host = ptr;
		peer->ready = true;
	}
	
	peers.insert(move(peer));
	return ptr;
}

void GameSettingsManager::remove_peer(GameSettingsManager::Peer* peer){
	for(auto it = peers.begin(); it != peers.end(); it++){
		if(it->get() == peer){
			if(host == it->get()) host = nullptr;
			peers.erase(it);
			
			if(host == nullptr && peers.size()){
				host = peers.begin()->get();
				host->ready = true;
				
				for(auto observer: host->get_observers()) observer->set_host();
				
				for(auto player: host->players){
					do_with_observers([&](GameSettingsObserver& observer){
						observer.set_host_player(player);
					});
				}
			}
			
			return;
		}
	}
}

void GameSettingsManager::do_with_observers(function<void(GameSettingsObserver&)> todo){
	for(auto& peer: peers){
		for(auto observer: peer->get_observers()){
			todo(*observer);
		}
	}
}

int get_free_color(const vector<int>& colors){
	set<int> color_set(colors.begin(), colors.end());
	for(int i = 0; i <= colors.size(); i++){
		if(!color_set.count(i)) return i;
	}
	return -1;  // Should never happen
}

int GameSettingsManager::add_player(){
	int player = settings.teams.size();
	int color = get_free_color(settings.colors);
	
	if(!settings.team_names.size()) settings.team_names.push_back("");
	int team = settings.team_names.size() - 1;
	
	settings.teams.push_back(team);
	settings.names.push_back("");
	settings.colors.push_back(color);
	
	do_with_observers([&](GameSettingsObserver& observer){
		observer.add_player(team, color);
	});
	
	return player;
}

void GameSettingsManager::remove_player(int player){
	counting_down = false;
	
	for(auto& peer: peers){
		for(auto& peer_player: peer->players){
			if(peer_player > player) peer_player--;
		}
	}

	remove_index(settings.teams, player);
	remove_index(settings.names, player);
	remove_index(settings.colors, player);

	do_with_observers([&](GameSettingsObserver& observer){
		observer.remove_player(player);
	});
}

void GameSettingsManager::set_player_name(int player, string name){
	settings.names[player] = name;
	
	do_with_observers([&](GameSettingsObserver& observer){
		observer.set_player_name(player, name);
	});
}

void GameSettingsManager::set_player_color(int player, int color){
	if(count(settings.colors, color)) return;

	settings.colors[player] = color;
	
	do_with_observers([&](GameSettingsObserver& observer){
		observer.set_player_color(player, color);
	});
}

void GameSettingsManager::set_teams(bool using_teams){
	settings.using_teams = using_teams;
	
	do_with_observers([&](GameSettingsObserver& observer){
		observer.set_teams(using_teams);
	});
}

void GameSettingsManager::add_team(){
	settings.team_names.push_back("");
	
	do_with_observers([&](GameSettingsObserver& observer){
		observer.add_team();
	});
}

void GameSettingsManager::remove_team(int team){
	if(settings.team_names.size() <= 1) return;

	remove_index(settings.team_names, team);
	for(auto& player_team: settings.teams){
		if(player_team >= team) player_team--;
	}
	
	do_with_observers([&](GameSettingsObserver& observer){
		observer.remove_team(team);
	});
}

void GameSettingsManager::set_player_team(int player, int team){
	settings.teams[player] = team;
	
	do_with_observers([&](GameSettingsObserver& observer){
		observer.set_player_team(player, team);
	});
}

void GameSettingsManager::set_team_name(int team, string name){
	settings.team_names[team] = name;
	
	do_with_observers([&](GameSettingsObserver& observer){
		observer.set_team_name(team, name);
	});
}

void GameSettingsManager::set_allowed_powerup(PowerUpDescriptor desc, bool allowed){
	if(allowed) settings.allowed_powerups.insert(desc);
	else settings.allowed_powerups.erase(desc);
	
	do_with_observers([&](GameSettingsObserver& observer){
		observer.set_allowed_powerup(desc, allowed);
	});
}

void GameSettingsManager::set_win_criterion(WinCriterion criterion){
	reset_all_ready();
	settings.scores.criterion = criterion;
	
	do_with_observers([&](GameSettingsObserver& observer){
		observer.set_win_criterion(criterion);
	});
}

void GameSettingsManager::set_win_amount(int amount){
	reset_all_ready();
	if(amount < 0) return;
	settings.scores.amount = amount;
	
	do_with_observers([&](GameSettingsObserver& observer){
		observer.set_win_amount(amount);
	});
}

void GameSettingsManager::set_tie_break(int threshold){
	reset_all_ready();
	if(threshold < 0) return;
	settings.scores.tie_break_threshold = threshold;
	
	do_with_observers([&](GameSettingsObserver& observer){
		observer.set_tie_break(threshold);
	});
}

void GameSettingsManager::reset_all_ready(){
	counting_down = false;
	
	for(auto& peer: peers){
		peer->ready = false;
		peer->starting = false;
	}
	
	do_with_observers([&](GameSettingsObserver& observer){
		observer.reset_all_ready();
	});
}

void GameSettingsManager::set_player_ready(int player, bool is_ready){
	do_with_observers([&](GameSettingsObserver& observer){
		observer.player_ready(player, is_ready);
	});
}

void GameSettingsManager::start_countdown(){
	if(check_ready()){
		counting_down = true;
		do_with_observers([&](GameSettingsObserver& observer){
			observer.start_countdown();
		});
	}
}

bool GameSettingsManager::check_ready(){
	for(auto& peer: peers){
		if(peer->players.size() && !(peer->ready || peer.get() == host)) return false;
	}
	return true;
}

bool GameSettingsManager::get_all_starting() const{
	for(auto& peer: peers){
		if(peer->players.size() && !peer->starting) return false;
	}
	return counting_down;
}

void GameSettingsManager::start_all(){
	counting_down = false;
	reset_all_ready();
	
	do_with_observers([&](GameSettingsObserver& observer){
		observer.start_game();
	});
}

const GameSettings& GameSettingsManager::get_settings() const{
	return settings;
}
