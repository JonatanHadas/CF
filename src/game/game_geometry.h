#ifndef _GAME_GEOMETRY_H
#define _GAME_GEOMETRY_H

#include "game_data.h"

#include <vector>

using namespace std;

bool check_curve_collision(  // Return whether further checking is possible.
	const BoardSize& board,
	const PlayerPosition& last_position,
	PlayerPosition& position,
	const vector<PlayerPosition>& history,
	bool is_self
);

bool check_border_collision(  // Return whether further checking is possible.
	const BoardSize& board,
	const PlayerPosition& last_position,
	PlayerPosition& position	
);

#endif
