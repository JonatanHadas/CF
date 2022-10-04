#include "game_event_listener_accumulator.h"

void GameEventListenerAccumulator::add_listener(GameEventListener* listener){
	listeners.insert(listener);
}

void GameEventListenerAccumulator::remove_listener(GameEventListener* listener){
	listeners.erase(listener);
}

void GameEventListenerAccumulator::clear_listeners(){
	listeners.clear();
}

const set<GameEventListener*>& GameEventListenerAccumulator::get_listeners() const {
	return listeners;
}
