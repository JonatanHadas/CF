#include "composite_player_interface.h"

CompositePlayerInterface::CompositePlayerInterface(set<PlayerInterface*>&& interfaces) :
	interfaces(std::move(interfaces)) {}

void CompositePlayerInterface::step(int round, int turn_state){
	for(auto interface: interfaces) interface->step(round, turn_state);
}

void CompositePlayerInterface::set_active(bool is_active){
	for(auto interface: interfaces) interface->set_active(is_active);
}
