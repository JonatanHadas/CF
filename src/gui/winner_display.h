#ifndef _WINNER_DISPLAY_H
#define _WINNER_DISPLAY_H

#include "gui.h"
#include "texts.h"

#include "../game/game_view.h"
#include "../game/game_settings.h"

#include <vector>
#include <memory>

using namespace std;

class WinnerDisplay : public Gui {
	const GameSettings settings;
	const vector<int> scores;
	
	int timer;
	
	int screen_width, screen_height;

	vector<Msg> names;
	vector<Msg> score_texts;
	
	vector<vector<Msg>> player_names;
	
	unique_ptr<Msg> title;
	
	void init(SDL_Renderer* renderer);	
public:
	WinnerDisplay(const GameSettings& settings, GameView* view);
	
	bool step();
	bool handle_event(const SDL_Event& event);
	void draw(SDL_Renderer* renderer);
};

#endif
