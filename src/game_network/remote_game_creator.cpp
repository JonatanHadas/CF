#include "remote_game_creator.h"

RemoteGameCreator::RemoteGameCreator(Client&& client) :
	client(std::move(client)),
	settings_client(this->client, settings) {}
	

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
	return nullptr;  // TODO
}
