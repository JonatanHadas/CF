#include "remote_game_creator.h"

#include "../game/game_logic.h"

#include "../utils/utils.h"

RemoteGameAdvancer::RemoteGameAdvancer(
	GameClient& client,
	GameView& game,
	GameAdvancer& advancer
) : 
	client(client),
	game(game),
	advancer(advancer) {}

void RemoteGameAdvancer::advance(){
	while(!game.is_over()){
		bool should_continue = client.handle_messages();
		advancer.advance();
		if(!should_continue) return;
	}
}

void RemoteGameAdvancer::allow_step(){
	advancer.allow_step();
}


RemoteGame::RemoteGame(
	Client& client,
	const GameSettingsView& settings,
	GameSettingsClient& settings_client
) :
	game(
		get_board_size(settings.get_settings().teams.size()),
		settings.get_settings().scores,
		settings.get_settings().teams, settings.get_settings().get_team_num()
	),
	client(
		client, game,
		settings.get_my_players().size(),
		[&](const string& message){
			settings_client.handle_message(message);
		}
	),
	advancer(this->client, game, game) {
		
	for(int player = 0; player < settings.get_settings().teams.size(); player++){
		if(!count(settings.get_my_players(), player)){
			game.get_player_interface(player).set_active(false);
		}
	}

	for(int i = 0; i < this->client.get_players().size(); i++){		
		interfaces.push_back(CompositePlayerInterface({
			this->client.get_players()[i],
			&game.get_player_interface(settings.get_my_players()[i])
		}));
	}
	
	for(auto& player_interface: interfaces) interface_ptrs.push_back(&player_interface);
	
	for(int player; player < settings.get_settings().teams.size(); player++){
		if(!count(settings.get_my_players(), player)) game.get_player_interface(player).set_active(false);
	}
}

GameView* RemoteGame::get_view() {
	return &game;
}

const vector<PlayerInterface*>& RemoteGame::get_interfaces() const {
	return interface_ptrs;
}

GameAdvancer* RemoteGame::get_advancer(){
	return &advancer;
}

GameEventListenerAccumulator* RemoteGame::get_accumulator(){
	return &game;
}


RemoteGameCreator::RemoteGameCreator(Client&& client) :
	client(std::move(client)),
	settings_client(this->client, settings) {}
	
RemoteGameCreator::~RemoteGameCreator(){
	client.close();
}

GameSettingsView* RemoteGameCreator::get_view(){
	return &settings;
}

GameSettingsManipulator* RemoteGameCreator::get_manipulator(){
	return &settings_client;
}

GameSettingsObserverAccumulator* RemoteGameCreator::get_accumulator(){
	return &settings;
}

bool RemoteGameCreator::is_multi_peer() const{
	return true;
}

bool RemoteGameCreator::update(){
	settings_client.handle_messages();
	
	return client.is_open();
}

unique_ptr<CreatedGame> RemoteGameCreator::create_game(){
	return make_unique<RemoteGame>(
		client,
		*get_view(),
		settings_client
	);
}
