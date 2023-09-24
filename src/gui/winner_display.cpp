#include "winner_display.h"

#include "colors.h"
#include "player_texture.h"

#include "sounds.h"

#include <algorithm>

#define MIN_TIME 60

vector<bool> get_active(GameView* view){
	vector<bool> active;

	for(const auto& state: view->get_states()) active.push_back(state.active);

	return active;
}

WinnerDisplay::WinnerDisplay(const GameSettings& settings, GameView* view) :
	settings(settings),
	scores(view->get_scores()),
	active(get_active(view)),
	start(true),
	timer(MIN_TIME) {}
	
string default_name(const char* type, int index){
	char name[50];
	snprintf(name, 50, "%s-%d", type, index);
	return name;
}

#define INACTIVE_MULTIPLIER 0.2

void WinnerDisplay::init(SDL_Renderer* renderer){
	SDL_GetRendererOutputSize(renderer, &screen_width, &screen_height);
	
	vector<int> order;
	for(int i = 0; i < scores.size(); i++) order.push_back(i);
	
	sort(order.begin(), order.end(), [&](int first, int second){
		return scores[first] > scores[second];
	});
	
	int winner_amount = 1;
	for(; winner_amount < order.size() && scores[order[winner_amount]] == scores[order[0]]; winner_amount++);
	
	vector<string> name_texts;
	
	if(settings.using_teams){
		vector<int> inverse_order(order.size());

		for(int i = 0; i < order.size(); i++){
			auto team = order[i];
			
			inverse_order[team] = i;
			
			name_texts.push_back(settings.team_names[team].size() ? settings.team_names[team] : default_name("team", team));
			
			names.push_back(Msg(
				name_texts.back().c_str(),
				text_color,
				FontType::MID,
				renderer
			));
			
			score_texts.push_back(Msg(
				to_string(scores[team]).c_str(),
				text_color,
				FontType::MID,
				renderer
			));
		}

		player_names = vector<vector<Msg>>(settings.team_names.size());
		
		for(int player = 0; player < settings.teams.size(); player++){
			const auto& player_texture = player_textures[settings.colors[player]];
			
			SDL_Color color = player_texture.get_color();
			if(!active[player]) color.a *= INACTIVE_MULTIPLIER;
			
			player_names[inverse_order[settings.teams[player]]].push_back(Msg(
				(settings.names[player].size() ? settings.names[player] : default_name("player", player)).c_str(),
				color, FontType::MID,
				renderer,
				player_texture.get_texture()
			));
		}
	}
	else{
		player_names = vector<vector<Msg>>(settings.teams.size());
		for(auto player: order){
			name_texts.push_back(settings.names[player].size() ? settings.names[player] : default_name("player", player));
			
			const auto& player_texture = player_textures[settings.colors[player]];

			SDL_Color color = player_texture.get_color();
			if(!active[player]) color.a *= INACTIVE_MULTIPLIER;
			
			names.push_back(Msg(
				name_texts.back().c_str(),
				color, FontType::MID,
				renderer,
				player_texture.get_texture()
			));
			
			score_texts.push_back(Msg(
				to_string(scores[player]).c_str(),
				color, FontType::MID,
				renderer,
				player_texture.get_texture()
			));
		}
	}
	
	string text = "";
	SDL_Color color = text_color;
	SDL_Texture* text_texture = NULL;
	if(winner_amount == 1){
		text = settings.using_teams ? "Team " : "";
		text += name_texts[0];
		if(!settings.using_teams){
			const auto& player_texture = player_textures[settings.colors[order[0]]];
			color = player_texture.get_color();
			text_texture = player_texture.get_texture();
		}
	}
	else{
		text = settings.using_teams ? "Teams " : "";
		for(int i = 0; i < winner_amount; i++){
			if(i == winner_amount - 1) text += " and ";
			else if(i > 0) text += ", ";
			
			text += name_texts[i];
		}
	}
	text += " won the game";
	
	title = make_unique<Msg>(
		text.c_str(),
		color,
		FontType::BIG,
		renderer,
		text_texture
	);
}

bool WinnerDisplay::step(){
	if(start){
		play(Sound::GAME_END);
		start = false;
	}
	if(timer) timer--;
	return false;
}

bool WinnerDisplay::handle_event(const SDL_Event& event){
	switch(event.type){
	case SDL_MOUSEBUTTONDOWN:
	case SDL_KEYDOWN:
		if(!timer) return true;
	}
	return false;
}

#define TITLE_Y 0.1
#define SCORES_Y 0.21
#define TEXT_X 0.05
#define PLAYER_NAME_X 0.07
#define SCORES_X 0.95
#define SCORES_DY 0.06

void WinnerDisplay::draw(SDL_Renderer* renderer){
	if(title.get() == nullptr) init(renderer);
	
	SDL_SetRenderDrawColor(renderer, bg_color.r, bg_color.g, bg_color.b, bg_color.a);
	SDL_RenderClear(renderer);
	
	title->render_centered(screen_width * TEXT_X, screen_height * TITLE_Y, Align::LEFT);
	
	int y = screen_height * SCORES_Y;
	for(int i = 0; i < names.size(); i++){
		names[i].render_centered(screen_width * TEXT_X, y, Align::LEFT);
		score_texts[i].render_centered(screen_width * SCORES_X, y, Align::RIGHT);

		y += screen_height * SCORES_DY;
		
		for(auto& name: player_names[i]){
			name.render_centered(screen_width * PLAYER_NAME_X, y, Align::LEFT);
			
			y += screen_height * SCORES_DY;			
		}	
	}
}
