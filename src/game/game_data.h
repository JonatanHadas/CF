#ifndef _GAME_DATA_H
#define _GAME_DATA_H

#include <ostream>
#include <istream>

using namespace std;

class BoardSize{
public:
	BoardSize(double w, double h);

	double w, h;
	
	void serialize(ostream& output) const;
	static BoardSize deserialize(istream& input);
};

class PlayerPosition{	
public:
	PlayerPosition(double x, double y, int size, bool hovering, bool alive);

	double x, y;
	int size;
	bool hovering, alive;

	void serialize(ostream& output) const;
	static PlayerPosition deserialize(istream& input);
};

class PlayerState{
public:
	PlayerState(double direction, int turn_state);

	double direction;
	int turn_state;

	void serialize(ostream& output) const;
	static PlayerState deserialize(istream& input);	
};

enum class PowerUpType : unsigned char{
	SPEED_UP = 0,
	SLOW_DOWN = 1,
	THICKEN = 2,
	NARROW = 3,
	WIDE_TURN = 4,
	NARROW_TURN = 5,
	RIGHT_TURN = 6,
	INVERT = 7,
	WARP_AROUND = 8,
	SPAWN_POWERUPS = 9,
	ERASER = 10,
	HOVER = 11
};

enum class PowerUpAffects : unsigned char{
	ALL = 0,
	YOU = 1,
	OTHERS = 2,
};

class PowerUpDescriptor{
public:
	PowerUpDescriptor(PowerUpType type, PowerUpAffects affects);
	
	PowerUpType type;
	PowerUpAffects affects;
	
	void serialize(ostream& output) const;
	static PowerUpDescriptor deserialize(istream& input);
};

class PowerUp{
public:
	PowerUp(PowerUpDescriptor desc, double x, double y);
	
	PowerUpDescriptor desc;

	double x,y;

	void serialize(ostream& output) const;
	static PowerUp deserialize(istream& input);	
};

class PowerUpEffect{
public:
	PowerUpEffect(int timer, PowerUpType type, bool affects_taker, int player);

	int timer;
	PowerUpType type;
	
	bool affects_taker;
	int player;

	void serialize(ostream& output) const;
	static PowerUpEffect deserialize(istream& input);	
};

#endif
