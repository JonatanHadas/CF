#include "player_settings.h"

#include "../utils/utils.h"

PlayerSettings::PlayerSettings() : view(nullptr), manipulator(nullptr) {}

void PlayerSettings::add_player(){
	if(is_attached()){
		manipulator->add_player();
	}
	else{
		index_ids.push_back(next_id++);
		names[index_ids.back()] = "";
	}
}

void PlayerSettings::remove_player(int id){
	int index = get_index(id);
	if(index == -1) return;

	if(is_attached()){
		manipulator->remove_player(index);
	}
	else{
		names.erase(id);
		pop_index(index_ids, index);
	}
}

int PlayerSettings::get_index(int id) const{
	if(is_attached()){
		for(int i = 0; i < view->get_my_players().size(); i++){
			if(player_ids.at(view->get_my_players().at(i)) == id) return i;
		}
	}
	else{
		for(int i = 0; i < index_ids.size(); i++){
			if(index_ids[i] == id) return i;
		}
	}
	return -1;
}

const vector<int>& PlayerSettings::get_ids(){
	if(is_attached()){
		index_ids.clear();
		for(auto player: view->get_my_players()){
			if(player_ids.count(player) == 0) player_ids[player] = next_id++;
			index_ids.push_back(player_ids[player]);
		}
	}
	return index_ids;
}

string empty_str;

const string& PlayerSettings::get_player_name(int id) const{
	int index = get_index(id);
	if(index == -1) return empty_str;

	if(is_attached()) return view->get_settings().names[view->get_my_players()[index]];
	else return names.at(id);
}

void PlayerSettings::set_player_name(int id, const string& name){
	int index = get_index(id);
	if(index == -1) return;

	if(is_attached()) manipulator->set_player_name(index, name);
	else names[id] = name;
}
int PlayerSettings::get_player_color(int id) const{
	int index = get_index(id);
	if(index == -1) return -1;

	if(is_attached()) return view->get_settings().colors[view->get_my_players()[index]];
	else return -1;
}
void PlayerSettings::set_player_color(int id, int color){
	int index = get_index(id);
	if(index == -1) return;

	if(is_attached()) manipulator->set_player_color(index, color);
}

const GameSettings& PlayerSettings::get_settings() const {
	if(is_attached()) return view->get_settings();
	
	return *(GameSettings*)nullptr;
}

bool PlayerSettings::is_attached() const{
	return view != nullptr && manipulator != nullptr;
}

void PlayerSettings::attach(GameSettingsView* view, GameSettingsManipulator* manipulator){
	this->view = view;
	this->manipulator = manipulator;
	
	for(int i = 0; i < names.size(); i++){
		manipulator->add_player();
		manipulator->set_player_name(i, names[index_ids[i]]);
	}
	names.clear();
}

void PlayerSettings::detach(){
	get_ids();
	for(auto entry: player_ids){
		names[entry.second] = view->get_settings().names[entry.first];
	}
	player_ids.clear();
	
	view = nullptr;
	manipulator = nullptr;
}
