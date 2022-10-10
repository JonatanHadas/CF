#include "game_gui.h"

#include "sounds.h"
#include "colors.h"
#include "images.h"

GameSoundManager::GameSoundManager(){}

GameSoundManager::~GameSoundManager(){
	silence();
}

void GameSoundManager::kill_player(int player){
	play(Sound::COLLISION);
}

void GameSoundManager::spawn_powerup(const PowerUp& power_up){
	play(Sound::SPAWN_POWERUP);
}

void GameSoundManager::activate_powerup(const PowerUp& power_up){
	play(Sound::TAKE_POWERUP);
}


GameMusicButton::GameMusicButton(const SDL_Rect& rect, bool music_on) :
	Button(rect, true) {

	if(music_on) music.play();
}

void GameMusicButton::draw(SDL_Renderer* renderer, const SDL_Color color){
	fill_back(renderer, clear_color);

	auto image = get_img(music.is_playing() ? Img::UNMUTED : Img::MUTED);
	
	SDL_SetTextureBlendMode(image, SDL_BLENDMODE_BLEND);
	SDL_SetTextureColorMod(image, color.r, color.g, color.b);
	SDL_SetTextureAlphaMod(image, color.a);
	
	SDL_RenderCopy(renderer, image, NULL, NULL);
}

void GameMusicButton::draw_inactive(SDL_Renderer* renderer){
	draw(renderer, text_color);
}

void GameMusicButton::draw_pressed(SDL_Renderer* renderer){
	draw(renderer, active_color);
}

void GameMusicButton::draw_released(SDL_Renderer* renderer){
	draw(renderer, text_color);
}

void GameMusicButton::on_pressed(){
	if(music.is_playing()) return music.halt();
	else music.play();
}

bool GameMusicButton::is_music_on(){
	return music.is_playing();
}


#define START_DELAY 30


GameGui::GameGui(
	GameView* view,
	GameAdvancer* advancer,
	GameEventListenerAccumulator* accumulator,
	const GameSettings& settings,
	const map<int, PlayerInterface*>& interfaces,
	const map<int, KeySet>& keysets,
	bool music_on
) :
	settings(settings),
	last_round(-1),
	starting_timer(0),
	paused(false),
	drawer(view, settings, this->keysets),
	view(view),
	advancer(advancer),
	accumulator(accumulator),
	interfaces(interfaces),
	keysets(keysets),
	music_on(music_on) {
		
	accumulator->add_listener(&sounds);
}

GameGui::~GameGui(){
	accumulator->remove_listener(&sounds);
}

bool GameGui::step(){
	if(!paused){
		if(last_round != view->get_round()){
			if(view->is_over()) return true;
			
			last_round = view->get_round();
			starting_timer = START_DELAY;
		}

		if(starting_timer) starting_timer--;
		else{
			auto keyboard_state = SDL_GetKeyboardState(NULL);
			for(auto entry: interfaces){
				int turn_state = 0;
				const auto& keyset = keysets[entry.first];
				if(keyboard_state[keyset.left]) turn_state -= 1;
				if(keyboard_state[keyset.right]) turn_state += 1;
				
				entry.second->step(view->get_round(), turn_state);
			}
		}
	}
	
	advancer->allow_step();
	advancer->advance();
	
	if(music_button.get() != nullptr) music_on = music_button->is_music_on();

	drawer.step();
	view_manager.step();
	return false;
}

bool GameGui::handle_event(const SDL_Event& event){
	if(view_manager.handle_event(event)) return false;

	switch(event.type){
	case SDL_KEYDOWN:
		switch(event.key.keysym.scancode){
		case SDL_SCANCODE_ESCAPE:
			return true;
		}
		break;
	}
	return false;
}

#define MUSIC_H 0.05
#define MUSIC_MARGIN 0.02

void GameGui::draw(SDL_Renderer* renderer){
	drawer.draw(renderer, paused || starting_timer);
	
	
	if(music_button.get() == nullptr) {
		int screen_width, screen_height;
		SDL_RenderGetLogicalSize(renderer, &screen_width, &screen_height);
		
		SDL_Rect rect;
		rect.w = rect.h = screen_height * MUSIC_H;
		int margin = screen_height * MUSIC_MARGIN;
		rect.x = screen_width - (rect.w + margin);
		rect.y = screen_height - (rect.h + margin);

		music_button = make_unique<GameMusicButton>(rect, music_on);
		view_manager.add_view(music_button.get());
	}
	
	view_manager.draw(renderer);	
}

unique_ptr<WinnerDisplay> GameGui::get_score_display(){
	if(!view->is_over()) return nullptr;
	return make_unique<WinnerDisplay>(settings, view);
}
