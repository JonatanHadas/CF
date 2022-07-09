#ifndef _PLAYER_INTERFACE_H
#define _PLAYER_INTERFACE_H

class PlayerInterface{
public:
	virtual void step(int round, int turn_state) = 0;
	virtual void set_active(bool active) = 0;
};

#endif
