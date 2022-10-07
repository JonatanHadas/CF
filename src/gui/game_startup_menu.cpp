#include "game_startup_menu.h"

#include "../game/local_game_creator.h"

#include "../game_network/protocol.h"
#include "../game_network/remote_game_creator.h"

#include "colors.h"

#include "sounds.h"

#include <enet/enet.h>

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
		"Go!",
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

#define SECOND_LENGTH 60

#define CONNECTION_TIMEOUT (3*SECOND_LENGTH)

ConnectionState::ConnectionState(int text_x, int text_y) :
	text_x(text_x), text_y(text_y),
	has_error(false) {}

void ConnectionState::connect(const string& hostname){
	clear_error();
	
	ENetAddress address;
	if(enet_address_set_host(&address, hostname.c_str()) < 0){
		set_error("Could not resolve host");
		return;
	}
	
	address.port = PORT;
	
	client = make_unique<Client>(address, CHANNEL_NUM);
	
	counter = CONNECTION_TIMEOUT;
}

bool ConnectionState::is_active(){
	return client.get() != nullptr && client->is_open();
}

bool ConnectionState::is_connected(){
	return client.get() != nullptr && client->is_connected();
}

void ConnectionState::draw(SDL_Renderer* renderer){
	if(connecting_msg.get() == nullptr) connecting_msg = make_unique<Msg>(
		"Connecting...",
		text_color,
		FontType::NRM,
		renderer
	);
	
	if(error.get() != nullptr){
		error_msg = make_unique<Msg>(
			error->c_str(),
			SDL_Color({128, 0, 0, 255}),
			FontType::NRM,
			renderer
		);
		
		error = nullptr;
	}
	
	if(has_error){
		error_msg->render_centered(text_x, text_y, Align::LEFT);
	}
	if(is_active() && !is_connected()) connecting_msg->render_centered(text_x, text_y, Align::LEFT);
}

void ConnectionState::step() {
	ReceivedMsg msg;
	bool idle;
	while(is_active() && !is_connected() && !idle) client->recv(0, msg, idle);
	
	if(is_active() && !is_connected()){
		if(counter) counter--;
		else{
			set_error("Connection timed out");
			client = nullptr;
		}
	}
	
	if(client.get() != nullptr && !is_active()) {
		set_error("Could not connect");
		client = nullptr;
	}
}

void ConnectionState::set_error(const string& message){
	error = make_unique<string>(message);
	has_error = true;
}

void ConnectionState::clear_error(){
	has_error = false;
}

unique_ptr<Client>&& ConnectionState::get_client(){
	return std::move(client);
}

HostTextBox::HostTextBox(const SDL_Rect& rect, int margin, ConnectionState& connection) :
	TextBox(
		rect, false,
		FontType::NRM,
		margin,
		text_color, {72, 72, 72, 32}
	),
	connection(connection) {}

void HostTextBox::draw_back(SDL_Renderer* renderer, bool typing){
	fill_back(renderer, typing ? active_color : bg_color);
	draw_frame(renderer, line_color);
}

void HostTextBox::on_set(const string& text){
	connection.connect(text);
	set_active(false);
}

string HostTextBox::get_default_text(){
	return "Enter host name";
}

#define HOST_X 0.02
#define HOST_Y 0.1
#define HOST_W 0.96
#define HOST_H 0.2
#define HOST_MARGIN 0.02

#define ERROR_X 0.15
#define ERROR_Y 0.65

#define BUTTON_Y 0.4
#define BUTTON_H 0.5
#define BUTTON_W 0.1

#define LOCAL_BUTTON_X 0.88
#define LEAVE_BUTTON_X 0.88
#define READY_BUTTON_X 0.02

GameStartupMenu::GameStartupMenu(const SDL_Rect& rect) : SubView(rect, false),
	connection(rect.w * ERROR_X, rect.h * ERROR_Y) {
	SDL_Rect button_rect;
	button_rect.w = rect.w * BUTTON_W;
	button_rect.h = rect.h * BUTTON_H;
	button_rect.y = rect.h * BUTTON_Y;
	
	button_rect.x = rect.w * LOCAL_BUTTON_X;
	local_game = make_unique<LocalGameButton>(button_rect, *this);
	view_manager.add_view(local_game.get());

	button_rect.x = rect.w * LOCAL_BUTTON_X;
	leave_game = make_unique<LeaveGameButton>(button_rect, *this);
	
	button_rect.x = rect.w * HOST_X;
	button_rect.y = rect.h * HOST_Y;
	button_rect.w = rect.w * HOST_W;
	button_rect.h = rect.h * HOST_H;
	host_box = make_unique<HostTextBox>(button_rect, rect.w * HOST_MARGIN, connection);
	view_manager.add_view(host_box.get());
}

void GameStartupMenu::sync_display(){
	if(connection.is_connected()){
		auto client = std::move(connection.get_client());
		
		set_creator(make_shared<RemoteGameCreator>(
			std::move(*client)
		));
	}
	
	if(game_creator.get() != nullptr) connection.clear_error();
	
	host_box->set_active(game_creator.get() == nullptr && !connection.is_active());
	
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
	
	connection.draw(renderer);
	
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
	connection.step();
	
	if(game_creator.get() == nullptr || !game_creator->get_view()->is_counting_down()){
		countdown = -1;
	}
	else if(countdown == -1){
		countdown = SECOND_LENGTH * COUNTDOWN_LENGTH;
	}
	else if(countdown > 0){
		if(countdown % SECOND_LENGTH == 0){
			play(Sound::DING_LOW);
		}
		countdown--;
	}
	else{
		play(Sound::DING_HIGH);
		game_creator->get_manipulator()->start_game();
	}
	
	sync_display();
	
	view_manager.step();
}
