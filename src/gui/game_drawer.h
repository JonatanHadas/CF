#ifndef _GAME_DRAWER
#define _GAME_DRAWER

#include "gui_utils.h"
#include "../game/game_view.h"
#include "../game/game_settings.h"

#include "texts.h"

#include <memory>
#include <SDL.h>

using namespace std;

class BoardDrawer{
	GameView* view;
	const GameSettings& settings;
	const BoardSize board;
	
	unique_ptr<Texture> texture;

	void init(SDL_Renderer* renderer);
public:
	BoardDrawer(GameView* view, const GameSettings& settings);
	
	void draw(SDL_Renderer* renderer);
	
	Texture& get_texture() const;
};

class ScoreDrawer{
	GameView* view;
	const GameSettings& settings;
	
	unique_ptr<Msg> title, threshold;
	bool tie_break;
	int amount;
	
	vector<Msg> names;
	
	vector<int> scores;
	vector<Msg> score_texts;
	
	vector<vector<Msg>> player_names;
	
	vector<int> order;
	vector<int> ys;
	
	int x, w, h;
	
	void init(SDL_Renderer* renderer);
public:
	ScoreDrawer(
		GameView* view,
		const GameSettings& settings,
		int x, int w, int h
	);
	
	void draw(SDL_Renderer* renderer);
	void step();
};

class WinnerDrawer{
	GameView* view;
	const GameSettings& settings;
	
	vector<int> winners;
	unique_ptr<Msg> msg;
	unique_ptr<Texture> texture;
	
	int y, h, w;
	
	vector<string> get_names();
	
	void init(SDL_Renderer* renderer);
	void draw_msg(SDL_Renderer* renderer);
public:
	WinnerDrawer(
		GameView* view,
		const GameSettings& settings,
		int y, int h, int w
	);

	void draw(SDL_Renderer* renderer);
	void step();
};

class GameDrawer{
	GameView* view;
	const BoardSize board;
	const GameSettings& settings;
	
	BoardDrawer board_drawer;
	unique_ptr<ScoreDrawer> score_drawer;
	unique_ptr<WinnerDrawer> winner_drawer;

	bool is_initialized;
	int screen_width, screen_height;
	
	void init(SDL_Renderer* renderer);
public:
	GameDrawer(GameView* view, const GameSettings& settings);
	
	void draw(SDL_Renderer* renderer);
	void step();
};

#endif
