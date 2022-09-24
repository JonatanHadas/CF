#include "game_settings_observer_accumulator.h"

void GameSettingsObserverAccumulator::add_observer(GameSettingsObserver* observer){
	init_observer(observer);
	
	observers.insert(observer);
}

void GameSettingsObserverAccumulator::remove_observer(GameSettingsObserver* observer){
	observers.erase(observer);
}

void GameSettingsObserverAccumulator::clear_observers(){
	observers.clear();
}

const set<GameSettingsObserver*> GameSettingsObserverAccumulator::get_observers() const{
	return observers;
}
