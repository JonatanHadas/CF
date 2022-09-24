#include "powerup_images.h"

map<PowerUpType, Img> powerup_images({
	{PowerUpType::INVERT, Img::INVERT},
	{PowerUpType::SPEED_UP, Img::SPEED},
	{PowerUpType::SLOW_DOWN, Img::TURTLE},
	{PowerUpType::ERASER, Img::ERASER},
	{PowerUpType::THICKEN, Img::THICKEN},
	{PowerUpType::NARROW, Img::NARROW},
	{PowerUpType::RIGHT_TURN, Img::CORNER},
	{PowerUpType::HOVER, Img::HOVER},
	{PowerUpType::SPAWN_POWERUPS, Img::SPAWN},
	{PowerUpType::NARROW_TURN, Img::NARROW_TURN},
	{PowerUpType::WIDE_TURN, Img::WIDE_TURN},
	{PowerUpType::WARP_AROUND, Img::WARP},
});
