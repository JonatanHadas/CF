#include "local_game_creator.h"

#include "default_settings.h"
#include "game_logic.h"

LocalGame::LocalGame(
	const GameSettings& settings,
	const vector<int>& players
) : game(
		get_board_size(settings.teams.size()),
		settings.scores,
		settings.get_team_num(), settings.get_teams(),
		settings.allowed_powerups,
		set<GameObserver*>()
	) {
	
	for(auto player: players){
		interfaces.push_back(&game.get_player_interface(player));
	}
}

GameView* LocalGame::get_view(){
	return &game;
}

const vector<PlayerInterface*>& LocalGame::get_interfaces() const {
	return interfaces;
}

GameAdvancer* LocalGame::get_advancer(){
	return &game;
}


LocalGameCreator::LocalGameCreator() :
	manager(default_settings){

	peer = manager.create_peer();
}

GameSettingsView* LocalGameCreator::get_view(){
	return peer;
}

GameSettingsManipulator* LocalGameCreator::get_manipulator(){
	return peer;
}

GameSettingsObserverAccumulator* LocalGameCreator::get_accumulator(){
	return peer;
}

bool LocalGameCreator::is_multi_peer() const{
	return false;
}

bool LocalGameCreator::update(){
	if(manager.get_all_starting()) manager.start_all();
	return true;
}

unique_ptr<CreatedGame> LocalGameCreator::create_game(){
	return make_unique<LocalGame>(manager.get_settings(), peer->get_my_players());
}
