#include "default_settings.h"

const set<PowerUpDescriptor> allowed_powerups({
	PowerUpDescriptor(PowerUpType::INVERT, PowerUpAffects::OTHERS),
	PowerUpDescriptor(PowerUpType::SPEED_UP, PowerUpAffects::YOU),
	PowerUpDescriptor(PowerUpType::SPEED_UP, PowerUpAffects::OTHERS),
	PowerUpDescriptor(PowerUpType::SLOW_DOWN, PowerUpAffects::YOU),
	PowerUpDescriptor(PowerUpType::SLOW_DOWN, PowerUpAffects::OTHERS),
	PowerUpDescriptor(PowerUpType::NARROW, PowerUpAffects::YOU),
	PowerUpDescriptor(PowerUpType::THICKEN, PowerUpAffects::OTHERS),
	PowerUpDescriptor(PowerUpType::NARROW_TURN, PowerUpAffects::YOU),
	PowerUpDescriptor(PowerUpType::WIDE_TURN, PowerUpAffects::OTHERS),
//	PowerUpDescriptor(PowerUpType::ERASER, PowerUpAffects::YOU),
	PowerUpDescriptor(PowerUpType::ERASER, PowerUpAffects::ALL),
	PowerUpDescriptor(PowerUpType::RIGHT_TURN, PowerUpAffects::YOU),
	PowerUpDescriptor(PowerUpType::HOVER, PowerUpAffects::YOU),
	PowerUpDescriptor(PowerUpType::SPAWN_POWERUPS, PowerUpAffects::ALL),
//	PowerUpDescriptor(PowerUpType::WARP_AROUND, PowerUpAffects::YOU),
	PowerUpDescriptor(PowerUpType::WARP_AROUND, PowerUpAffects::ALL),
});
	
const ScoreSettings score_settings(WinCriterion::BY_SCORE, 10, 2);
	
const GameSettings default_settings(
	score_settings,
	false,
	vector<string>(),
	vector<int>(),
	allowed_powerups,
	vector<string>(),
	vector<int>()
);
