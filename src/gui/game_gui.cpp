#include "game_gui.h"

#include "sounds.h"

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


#define START_DELAY 30

GameGui::GameGui(
	GameView* view,
	GameAdvancer* advancer,
	GameEventListenerAccumulator* accumulator,
	const GameSettings& settings,
	const map<int, PlayerInterface*>& interfaces,
	const map<int, KeySet>& keysets
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
	keysets(keysets) {
		
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

	drawer.step();
	return false;
}

bool GameGui::handle_event(const SDL_Event& event){
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

void GameGui::draw(SDL_Renderer* renderer){
	drawer.draw(renderer, paused || starting_timer);
}

unique_ptr<WinnerDisplay> GameGui::get_score_display(){
	if(!view->is_over()) return nullptr;
	return make_unique<WinnerDisplay>(settings, view);
}
