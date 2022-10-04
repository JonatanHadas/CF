#ifndef _GAME_EVENT_LISTENER_ACCUMULATOR_H
#define _GAME_EVENT_LISTENER_ACCUMULATOR_H

#include "game_event_listener.h"

#include <set>

using namespace std;

class GameEventListenerAccumulator{
	set<GameEventListener*> listeners;
public:
	void add_listener(GameEventListener* listener);
	void remove_listener(GameEventListener* listener);
	void clear_listeners();
	
	const set<GameEventListener*>& get_listeners() const;
};

#endif