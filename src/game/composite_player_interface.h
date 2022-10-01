#ifndef _COMPOSITE_PLAYER_INTERFACE_H
#define _COMPOSITE_PLAYER_INTERFACE_H

#include "player_interface.h"

#include <set>

using namespace std;

class CompositePlayerInterface : public PlayerInterface {
	set<PlayerInterface*> interfaces;
public:
	CompositePlayerInterface(set<PlayerInterface*>&& interfaces);
	
	void step(int round, int turn_state);
	void set_active(bool is_active);
};

#endif
