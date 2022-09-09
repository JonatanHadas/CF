#ifndef _GAME_SETTINGS_OBSERVER_ACCUMULATOR_H
#define _GAME_SETTINGS_OBSERVER_ACCUMULATOR_H

#include "game_settings_observer.h"

#include <set>

using namespace std;

class GameSettingsObserverAccumulator{
	set<GameSettingsObserver*> observers;
protected:
	virtual void init_observer(GameSettingsObserver* observer) = 0;
public:
	void add_observer(GameSettingsObserver* observer);
	void remove_observer(GameSettingsObserver* observer);
	void clear_observers();
	
	const set<GameSettingsObserver*> get_observers() const;
};

#endif
