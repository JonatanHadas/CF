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
	PlayerPosition(double x, double y, double direction);
	PlayerPosition(
		double x, double y, double direction,
		int size,
		int warp_x, int warp_y,
		bool warping_x, bool warping_y,
		bool corner, bool hovering, bool alive
	);

	double x, y;
	double direction;
	int size;
	int warp_x, warp_y;
	bool warping_x, warping_y;
	bool corner;
	bool hovering, alive;

	void serialize(ostream& output) const;
	static PlayerPosition deserialize(istream& input);
};

class PlayerState{
public:
	PlayerState(int turn_state, int counter);

	int turn_state;
	int counter;

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
	
	bool operator<(const PowerUpDescriptor& other) const;
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
	PowerUpEffect(int timer, const PowerUpDescriptor& desc, int player);

	int timer;
	PowerUpDescriptor desc;
	int player;

	void serialize(ostream& output) const;
	static PowerUpEffect deserialize(istream& input);	
};

enum class WinCriterion : char{
	BY_SCORE,
	BY_ROUND,
	NEVER,
};

class ScoreSettings{
public:
	WinCriterion criterion;
	int tie_break_threshold;
	int amount;
	
	ScoreSettings(WinCriterion criterion, int amount, int tie_break_threshold);
	
	void serialize(ostream& output) const;
	static ScoreSettings deserialize(istream& input);
};

#endif
