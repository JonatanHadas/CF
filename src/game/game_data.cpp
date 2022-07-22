#include "game_data.h"

#include "../utils/serialization.h"


BoardSize::BoardSize(double w, double h) :
	w(w),
	h(h) {}

void BoardSize::serialize(ostream& output) const {
	write_raw(output, w);
	write_raw(output, h);
}

BoardSize BoardSize::deserialize(istream& input){
	auto w = read_raw<double>(input);
	auto h = read_raw<double>(input);
	
	return BoardSize(w, h);
}

#define HOVER_MASK (1 << 0)
#define ALIVE_MASK (1 << 1)
#define WARP_X_MASK (1 << 2)
#define WARP_Y_MASK (1 << 3)
#define CORNER_MASK (1 << 4)

PlayerPosition::PlayerPosition(
	double x, double y, double direction,
	int size,
	int warp_x, int warp_y,
	bool warping_x, bool warping_y,
	bool corner, bool hovering, bool alive
) :
	x(x), y(y), direction(direction),
	size(size),
	warp_x(warp_x), warp_y(warp_y),
	warping_x(warping_x), warping_y(warping_y),
	corner(corner), hovering(hovering), alive(alive) {}
	
PlayerPosition::PlayerPosition(double x, double y, double direction) :
	PlayerPosition(x, y, direction, 0, 0, 0, false, false, false, true, true) {}

void PlayerPosition::serialize(ostream& output) const {
	write_raw(output, x);
	write_raw(output, y);
	
	write_raw(output, direction);
	
	write_raw(output, size);
	
	write_raw(output, warp_x);
	write_raw(output, warp_y);
	
	unsigned char flags = 0;
	if(warping_x) flags |= WARP_X_MASK;
	if(warping_y) flags |= WARP_Y_MASK;
	if(corner) flags |= CORNER_MASK;

	if(hovering) flags |= HOVER_MASK;
	if(alive) flags |= ALIVE_MASK;
	
	write_raw(output, flags);
}

PlayerPosition PlayerPosition::deserialize(istream& input){
	auto x = read_raw<double>(input);
	auto y = read_raw<double>(input);

	auto direction = read_raw<double>(input);

	auto size = read_raw<int>(input);

	auto warp_x = read_raw<int>(input);
	auto warp_y = read_raw<int>(input);

	auto flags = read_raw<unsigned char>(input);
	
	return PlayerPosition(
		x, y, direction,
		size,
		warp_x, warp_y,
		flags & WARP_X_MASK, flags & WARP_Y_MASK,
		flags & CORNER_MASK, flags & HOVER_MASK, flags & ALIVE_MASK
	);
}


PlayerState::PlayerState(int turn_state) :
	turn_state(turn_state) {}

void PlayerState::serialize(ostream& output) const {
	write_raw(output, turn_state);
}

PlayerState PlayerState::deserialize(istream& input){
	auto turn_state = read_raw<int>(input);
	
	return PlayerState(turn_state);
}


PowerUpDescriptor::PowerUpDescriptor(PowerUpType type, PowerUpAffects affects) :
	type(type),
	affects(affects) {}
	

void PowerUpDescriptor::serialize(ostream& output) const {
	write_raw(output, type);
	write_raw(output, affects);
}

PowerUpDescriptor PowerUpDescriptor::deserialize(istream& input){
	auto type = read_raw<PowerUpType>(input);
	auto affects = read_raw<PowerUpAffects>(input);

	return PowerUpDescriptor(type, affects);
}


PowerUp::PowerUp(PowerUpDescriptor desc, double x, double y) :
	desc(desc),
	x(x),
	y(y) {}

void PowerUp::serialize(ostream& output) const {
	desc.serialize(output);
	write_raw(output, x);
	write_raw(output, y);
}

PowerUp PowerUp::deserialize(istream& input){
	auto desc = PowerUpDescriptor::deserialize(input);
	auto x = read_raw<double>(input);
	auto y = read_raw<double>(input);
	
	return PowerUp(desc, x, y);
}


PowerUpEffect::PowerUpEffect(int timer, PowerUpType type, bool affects_taker, int player) :
	timer(timer),
	type(type),
	affects_taker(affects_taker),
	player(player) {}

void PowerUpEffect::serialize(ostream& output) const {
	write_raw(output, timer);
	write_raw(output, type);
	write_raw(output, player);
	write_raw(output, affects_taker);
}

PowerUpEffect PowerUpEffect::deserialize(istream& input){
	auto timer = read_raw<int>(input);
	auto type = read_raw<PowerUpType>(input);
	auto player = read_raw<int>(input);
	auto affects_taker = read_raw<bool>(input);
	
	return PowerUpEffect(timer, type, affects_taker, player);
}
