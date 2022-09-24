#include "game_startup_menu.h"

#include "../game/local_game_creator.h"

#include "colors.h"

SimpleButton::SimpleButton(const SDL_Rect& rect, string text) : Button(rect, false), text(text) {}

void SimpleButton::draw_button(SDL_Renderer* renderer, const SDL_Color& color){
	fill_back(renderer, color);
	draw_frame(renderer, line_color);
	
	if(label.get() == nullptr) label = make_unique<Msg>(
		text.c_str(),
		text_color,
		FontType::NRM,
		renderer
	);
	
	label->render_centered(get_rect().w / 2, get_rect().h / 2, Align::CENTER);
}

void SimpleButton::draw_pressed(SDL_Renderer* renderer){
	draw_button(renderer, active_color);
}

void SimpleButton::draw_released(SDL_Renderer* renderer){
	draw_button(renderer, bg_color);
}

void SimpleButton::draw_inactive(SDL_Renderer* renderer){
	draw_button(renderer, bg_color);
}

LocalGameButton::LocalGameButton(const SDL_Rect& rect, GameStartupMenu& menu) :
	SimpleButton(rect, "Play"), menu(menu) {}

void LocalGameButton::on_pressed(){
	menu.set_creator(make_shared<LocalGameCreator>());
}

LeaveGameButton::LeaveGameButton(const SDL_Rect& rect, GameStartupMenu& menu) :
	SimpleButton(rect, "Leave"), menu(menu) {}

void LeaveGameButton::on_pressed(){
	menu.set_creator(nullptr);
}

ReadyButton::ReadyButton(
	const SDL_Rect& rect,
	GameSettingsView* view,
	GameSettingsManipulator* manipulator,
	function<int()> get_countdown
) : Button(rect, false),
	view(view), manipulator(manipulator),
	get_countdown(get_countdown) {}

void ReadyButton::draw_button(SDL_Renderer* renderer, const SDL_Color& color){
	fill_back(renderer, color);
	draw_frame(renderer, line_color);
	
	if(ready.get() == nullptr) ready = make_unique<Msg>(
		"Ready",
		text_color,
		FontType::NRM,
		renderer
	);

	if(not_ready.get() == nullptr) not_ready = make_unique<Msg>(
		"Not Ready",
		text_color,
		FontType::NRM,
		renderer
	);

	if(start.get() == nullptr) start = make_unique<Msg>(
		"Start",
		text_color,
		FontType::NRM,
		renderer
	);

	if(waiting.get() == nullptr) waiting = make_unique<Msg>(
		"Waiting",
		text_color,
		FontType::NRM,
		renderer
	);
	
	if(countdown[0].get() == nullptr) countdown[0] = make_unique<Msg>(
		"Ready",
		text_color,
		FontType::NRM,
		renderer
	);
	
	for(int i = 1; i < COUNTDOWN_LENGTH; i++){
		if(countdown[i].get() == nullptr) countdown[i] = make_unique<Msg>(
			to_string(i).c_str(),
			text_color,
			FontType::NRM,
			renderer
		);
	}
	
	Msg* msg;
	if(view->is_counting_down()) msg = countdown[get_countdown()].get();
	else if(view->am_i_host()) msg = (view->get_settings().teams.size() == view->get_ready().size() ? start : waiting).get();
	else msg = (view->am_i_ready() ? ready : not_ready).get();
	
	msg->render_centered(get_rect().w / 2, get_rect().h / 2, Align::CENTER);
}

void ReadyButton::draw_pressed(SDL_Renderer* renderer){
	draw_button(renderer, active_color);
}

void ReadyButton::draw_released(SDL_Renderer* renderer){
	draw_button(renderer, bg_color);
}

void ReadyButton::draw_inactive(SDL_Renderer* renderer){
	draw_button(renderer, bg_color);
}

void ReadyButton::on_pressed(){
	if(view->am_i_host()){
		if(view->get_settings().teams.size() == view->get_ready().size()) manipulator->start_countdown();
	}
	else{
		manipulator->set_ready(!view->am_i_ready());
	}
}

#define BUTTON_Y 0.4
#define BUTTON_H 0.5
#define BUTTON_W 0.1

#define LOCAL_BUTTON_X 0.88
#define LEAVE_BUTTON_X 0.88
#define READY_BUTTON_X 0.02

#define SECOND_LENGTH 60

GameStartupMenu::GameStartupMenu(const SDL_Rect& rect) : SubView(rect, false) {
	SDL_Rect button_rect;
	button_rect.w = rect.w * BUTTON_W;
	button_rect.h = rect.h * BUTTON_H;
	button_rect.y = rect.h * BUTTON_Y;
	
	button_rect.x = rect.w * LOCAL_BUTTON_X;
	local_game = make_unique<LocalGameButton>(button_rect, *this);
	view_manager.add_view(local_game.get());

	button_rect.x = rect.w * LOCAL_BUTTON_X;
	leave_game = make_unique<LeaveGameButton>(button_rect, *this);
}

void GameStartupMenu::sync_display(){
	if(game_creator.get() == nullptr){
		if(ready.get() != nullptr){
			view_manager.remove_view(ready.get());
			ready = nullptr;
			
			view_manager.remove_view(leave_game.get());
			view_manager.add_view(local_game.get());
		}
	}
	else if(ready.get() == nullptr){
		SDL_Rect rect;
		rect.w = get_rect().w * BUTTON_W;
		rect.h = get_rect().h * BUTTON_H;
		rect.x = get_rect().w * READY_BUTTON_X;
		rect.y = get_rect().h * BUTTON_Y;
		
		ready = make_unique<ReadyButton>(
			rect,
			game_creator->get_view(),
			game_creator->get_manipulator(),
			[&](){ return countdown / SECOND_LENGTH; }
		);
		view_manager.add_view(ready.get());
		
		view_manager.add_view(leave_game.get());
		view_manager.remove_view(local_game.get());	
	}
	
	if(ready.get() != nullptr) ready->set_active(
		!game_creator->get_view()->am_i_host() ||
		game_creator->get_view()->get_settings().teams.size() == game_creator->get_view()->get_ready().size()
	);
}

void GameStartupMenu::draw_content(SDL_Renderer* renderer){
	sync_display();
	
	fill_back(renderer, bg_color);
	draw_frame(renderer, line_color);
	
	view_manager.draw(renderer);
}

bool GameStartupMenu::on_event(const SDL_Event& event){
	sync_display();
	
	return view_manager.handle_event(event);
}

shared_ptr<GameCreator> GameStartupMenu::get_creator(){
	return game_creator;
}

void GameStartupMenu::set_creator(shared_ptr<GameCreator> creator){
	game_creator = creator;
}

void GameStartupMenu::lose_focus(){
	sync_display();
	
	view_manager.lose_focus();
}

void GameStartupMenu::step(){
	if(game_creator.get() == nullptr || !game_creator->get_view()->is_counting_down()){
		countdown = -1;
	}
	else if(countdown == -1){
		countdown = SECOND_LENGTH * COUNTDOWN_LENGTH - 1;
	}
	else if(countdown > 0){
		countdown--;
	}
	else{
		game_creator->get_manipulator()->start_game();
	}
	
	sync_display();
	
	view_manager.step();
}
