#ifndef _GAME_CREATOR_H
#define _GAME_CREATOR_H

#include "game_settings_view.h"
#include "game_settings_manipulator.h"
#include "game_settings_observer_accumulator.h"

#include "game_view.h"
#include "player_interface.h"
#include "game_advancer.h"

#include <vector>
#include <memory>

using namespace std;

class CreatedGame{
public:
	virtual GameView* get_view() = 0;
	virtual const vector<PlayerInterface*>& get_interfaces() const = 0;
	virtual GameAdvancer* get_advancer() = 0;
};

class GameCreator{
public:
	virtual GameSettingsView* get_view() = 0;
	virtual GameSettingsManipulator* get_manipulator() = 0;
	virtual GameSettingsObserverAccumulator* get_accumulator() = 0;
	
	virtual bool is_multi_peer() const = 0;
	
	virtual bool update() = 0;
	
	virtual unique_ptr<CreatedGame> create_game() = 0;
};

#endif
