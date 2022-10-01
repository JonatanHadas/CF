#ifndef _PROTOCOL_H
#define _PROTOCOL_H

#define PORT 32123

#define CHANNEL 0
#define CHANNEL_NUM 1

enum class MessageType : unsigned char{
	SETTINGS = 0,
	GAME = 1,
};

enum class SettingsObserverMessageType : unsigned char{
	INIT = 0x00,

	ADD_PLAYER = 0x10,
	REMOVE_PLAYER = 0x11,
	SET_PLAYER_INDEX = 0x12,

	SET_PLAYER_NAME = 0x21,
	SET_PLAYER_COLOR = 0x22,

	SET_TEAMS = 0x30,
	ADD_TEAM = 0x31,
	REMOVE_TEAM = 0x32,
	SET_PLAYER_TEAM = 0x33,

	SET_TEAM_NAME = 0x40,

	SET_ALLOWED_POWERUP = 0x50,

	SET_WIN_CRITERION = 0x60,
	SET_WIN_AMOUNT = 0x61,
	SET_TIE_BREAK = 0x62,

	SET_HOST_PLAYER = 0x70,
	SET_HOST = 0x71,

	PLAYER_READY = 0x80,
	RESET_ALL_READY = 0x81,
	START_COUNTDOWN = 0x82,
	START_GAME = 0x83,
};

enum class SettingsManipulatorMessageType : unsigned char{
	ADD_PLAYER = 0x00,
	REMOVE_PLAYER = 0x01,

	SET_PLAYER_NAME = 0x10,
	SET_PLAYER_COLOR = 0x11,

	SET_TEAMS = 0x20,
	ADD_TEAM = 0x21,
	REMOVE_TEAM = 0x22,
	SET_PLAYER_TEAM = 0x23,

	SET_TEAM_NAME = 0x24,

	SET_ALLOWED_POWERUP = 0x30,

	SET_WIN_CRITERION = 0x40,
	SET_WIN_AMOUNT = 0x41,
	SET_TIE_BREAK = 0x42,

	SET_READY = 0x50,
	START_COUNTDOWN = 0x51,
	START_GAME = 0x52,
};

enum class GameObserverMessageType : unsigned char {
	INIT = 0x00,
	NEW_ROUND = 0x10,
	UPDATE_SCORES = 0x11,
	SET_WINNERS = 0x12,
	UPDATE = 0x20,
	SPAWN_POWERUP = 0x30,
	ACTIVATE_POWERUP = 0x31,
};

enum class PlayerInterfaceMessageType : unsigned char {
	STEP = 0x00,
	SET_ACTIVE = 0x10,
};

#endif
