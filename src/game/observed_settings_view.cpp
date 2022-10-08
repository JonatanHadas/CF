#include "observed_settings_view.h"

#include "default_settings.h"

#include "../utils/utils.h"

ObservedSettingsView::ObservedSettingsView() :
	settings(default_settings),
	host(false),
	ready(false),
	counting_down(false) {}
	
void ObservedSettingsView::init_observer(GameSettingsObserver* observer){
	observer->init(settings);
	for(int i = 0; i < my_players.size(); i++) observer->set_player_index(i, my_players[i]);
	
	for(auto player: ready_players) observer->player_ready(player, true);
	for(auto player: host_players) observer->set_host_player(player);
	
	if(host) observer->set_host();
	if(counting_down) observer->start_countdown();
}

void ObservedSettingsView::init(const GameSettings& settings){
	this->settings = settings;
	
	for(auto observer: get_observers()) observer->init(settings);
}

void ObservedSettingsView::add_player(int team, int color){
	if(!settings.team_names.size()) settings.team_names.push_back("");
	
	settings.teams.push_back(team);
	settings.colors.push_back(color);
	settings.names.push_back("");
	
	for(auto observer: get_observers()) observer->add_player(team, color);
}

void ObservedSettingsView::remove_player(int player){
	erase(my_players, player);
	erase(host_players, player);
	erase(ready_players, player);
	
	for(auto& my_player: my_players){
		if(my_player > player) my_player--;
	}
	for(auto& host_player: host_players){
		if(host_player > player) host_player--;
	}
	for(auto& ready_player: ready_players){
		if(ready_player > player) ready_player--;
	}
	
	remove_index(settings.teams, player);
	remove_index(settings.colors, player);
	remove_index(settings.names, player);
	
	for(auto observer: get_observers()) observer->remove_player(player);
}

void ObservedSettingsView::set_player_index(int index, int player){
	while(my_players.size() <= index) my_players.push_back(-1);
	my_players[index] = player;
	
	for(auto observer: get_observers()) observer->set_player_index(index, player);
}

void ObservedSettingsView::set_player_name(int player, string name){
	settings.names[player] = name;
	
	for(auto observer: get_observers()) observer->set_player_name(player, name);
}

void ObservedSettingsView::set_player_color(int player, int color){
	settings.colors[player] = color;
	
	for(auto observer: get_observers()) observer->set_player_color(player, color);
}

void ObservedSettingsView::set_teams(bool using_teams){
	settings.using_teams = using_teams;
	
	for(auto observer: get_observers()) observer->set_teams(using_teams);
}

void ObservedSettingsView::add_team(){
	settings.team_names.push_back("");
	
	for(auto observer: get_observers()) observer->add_team();
}

void ObservedSettingsView::remove_team(int team_num){
	remove_index(settings.team_names, team_num);
	
	for(auto& team: settings.teams){
		if(team >= team_num) team--;
	}
	
	for(auto observer: get_observers()) observer->remove_team(team_num);
}

void ObservedSettingsView::set_player_team(int player, int team){
	settings.teams[player] = team;
	
	for(auto observer: get_observers()) observer->set_player_team(player, team);
}

void ObservedSettingsView::set_team_name(int team, string name){
	settings.team_names[team] = name;
	
	for(auto observer: get_observers()) observer->set_team_name(team, name);
}

void ObservedSettingsView::set_allowed_powerup(PowerUpDescriptor desc, bool allowed){
	if(allowed) settings.allowed_powerups.insert(desc);
	else settings.allowed_powerups.erase(desc);
	
	for(auto observer: get_observers()) observer->set_allowed_powerup(desc, allowed);
}

void ObservedSettingsView::set_win_criterion(WinCriterion criterion){
	settings.scores.criterion = criterion;
	
	for(auto observer: get_observers()) observer->set_win_criterion(criterion);
}

void ObservedSettingsView::set_win_amount(int amount){
	settings.scores.amount = amount;
	
	for(auto observer: get_observers()) observer->set_win_amount(amount);
}

void ObservedSettingsView::set_tie_break(int threshold){
	settings.scores.tie_break_threshold = threshold;
	
	for(auto observer: get_observers()) observer->set_tie_break(threshold);
}

void ObservedSettingsView::set_host_player(int player){
	host_players.push_back(player);
	
	for(auto observer: get_observers()) observer->set_host_player(player);
}

void ObservedSettingsView::set_host(){
	host = true;
	
	for(auto observer: get_observers()) observer->set_host();
}

void ObservedSettingsView::player_ready(int player, bool is_ready){
	if(count(my_players, player)) ready = is_ready || host;

	if(is_ready) ready_players.push_back(player);
	else{
		erase(ready_players, player);
		counting_down = false;
	}
	
	for(auto observer: get_observers()) observer->player_ready(player, is_ready);
}

void ObservedSettingsView::reset_all_ready(){
	counting_down = false;
	ready = host;
	ready_players.clear();
	
	for(auto observer: get_observers()) observer->reset_all_ready();
}

void ObservedSettingsView::start_countdown(){
	counting_down = true;
	
	for(auto observer: get_observers()) observer->start_countdown();
}

void ObservedSettingsView::start_game() {
	counting_down = false;
	for(auto observer: get_observers()) observer->start_game();
}

const GameSettings& ObservedSettingsView::get_settings() const {
	return settings;
}

const vector<int>& ObservedSettingsView::get_my_players() const {
	return my_players;
}

const vector<int>& ObservedSettingsView::get_host_players() const {
	return host_players;
}

set<int> ObservedSettingsView::get_ready() const {
	set<int> ready;
	for(auto player: ready_players) ready.insert(player);
	for(auto player: host_players) ready.insert(player);
	
	return ready;
}

bool ObservedSettingsView::am_i_host() const {
	return host;
}

bool ObservedSettingsView::am_i_ready() const {
	return ready;
}

bool ObservedSettingsView::is_counting_down() const {
	return counting_down;
}
