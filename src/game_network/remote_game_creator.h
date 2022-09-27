#ifndef _REMOTE_GAME_CREATOR_H
#define _REMOTE_GAME_CREATOR_H

#include "game_settings_client.h"

#include "../game/game_creator.h"
#include "../game/observed_settings_view.h"

#include "../network/client.h"

class RemoteGameCreator : public GameCreator {
	Client client;
	GameSettingsClient settings_client;
	ObservedSettingsView settings;
public:
	RemoteGameCreator(Client&& client);

	GameSettingsView* get_view();
	GameSettingsManipulator* get_manipulator();
	GameSettingsObserverAccumulator* get_accumulator();
	
	bool is_multi_peer() const;
	
	bool update();
	
	unique_ptr<CreatedGame> create_game();
};

#endif
