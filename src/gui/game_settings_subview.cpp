#include "game_settings_subview.h"

#include "colors.h"
#include "player_texture.h"
#include "images.h"
#include "sounds.h"

#include "powerup_images.h"
#include "../game/powerups.h"

#include "../utils/utils.h"
#include "gui_utils.h"

#define OPTION_FOLLOW_SPEED 3

WinCriterionMenu::WinCriterionMenu(
	const SDL_Rect& rect, int x_margin,
	GameSettingsView* view, GameSettingsManipulator* manipulator
) : HorizontalOptionsMenu(
		rect,
		vector<string>({
			"By score",
			"By round",
			"Never",
		}),
		x_margin,
		OPTION_FOLLOW_SPEED,
		line_color, text_color, text_color,
		FontType::NRM
	),
	view(view), manipulator(manipulator) {}

void WinCriterionMenu::on_set(int option){
	play(Sound::CLICK);
	manipulator->set_win_criterion((WinCriterion)option);
}

int WinCriterionMenu::get_option(){
	return (int)(view->get_settings().scores.criterion);
}

#define NUMBER_BUTTON_W 0.5
#define NUMBER_BUTTON_H 0.2

WinAmountButton::WinAmountButton(
	const SDL_Rect& rect,
	GameSettingsView* view, GameSettingsManipulator* manipulator
) : NumberButton(
		rect,
		rect.w * NUMBER_BUTTON_W, rect.h * NUMBER_BUTTON_H,
		FontType::NRM,
		text_color, text_color,
		text_color, line_color, clear_color,
		0
	),
	view(view), manipulator(manipulator) {}

void WinAmountButton::on_set(int num){
	play(Sound::CLICK);
	manipulator->set_win_amount(num);
}

int WinAmountButton::get_value(){
	return view->get_settings().scores.amount;
}

TieBreakButton::TieBreakButton(
	const SDL_Rect& rect,
	GameSettingsView* view, GameSettingsManipulator* manipulator
) : NumberButton(
		rect,
		rect.w * NUMBER_BUTTON_W, rect.h * NUMBER_BUTTON_H,
		FontType::NRM,
		text_color, text_color,
		text_color, line_color, clear_color,
		0, 2
	),
	view(view), manipulator(manipulator) {}

void TieBreakButton::on_set(int num){
	play(Sound::CLICK);
	manipulator->set_tie_break(num);
}

int TieBreakButton::get_value(){
	return view->get_settings().scores.tie_break_threshold;
}

PowerUpButton::PowerUpButton(
	const SDL_Rect& rect,
	PowerUpDescriptor descriptor,
	GameSettingsView* view, GameSettingsManipulator* manipulator
) : Button(rect, true),
	descriptor(descriptor),
	view(view), manipulator(manipulator) {}

void PowerUpButton::draw_powerup(SDL_Renderer* renderer, bool pressed){
	SDL_Rect rect = get_rect();
	rect.x = rect.y = 1;
	rect.w -= 2;
	rect.h -= 2;
	
	if(circle.get() == nullptr){
		int size = max(rect.w, rect.h);
		
		circle = make_unique<Texture>(
			renderer,
			SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET,
			size, size
		);
		
		SDL_SetTextureBlendMode(circle->get(), SDL_BLENDMODE_BLEND);
		
		circle->do_with_texture(renderer, [&](){
			SDL_SetRenderDrawColor(renderer, 255, 255, 255, 0);
			SDL_RenderClear(renderer);
			
			SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
			draw_circle(renderer, size/2, size/2, size/2.0);
		});
	}
	
	SDL_SetRenderDrawColor(renderer, clear_color.r, clear_color.g, clear_color.b, clear_color.a);
	SDL_RenderClear(renderer);
	
	if(pressed){
		SDL_SetTextureAlphaMod(circle->get(), 255);
		SDL_SetTextureColorMod(circle->get(), line_color2.r, line_color2.g, line_color2.b);
		SDL_RenderCopy(renderer, circle->get(), NULL, NULL);
	}

	bool active = view->get_settings().allowed_powerups.count(descriptor);
	SDL_SetTextureAlphaMod(circle->get(), active ? 255 : 128);
	auto color = powerup_colors[(int)descriptor.affects];
	SDL_SetTextureColorMod(circle->get(), color.r, color.g, color.b);
	SDL_RenderCopy(renderer, circle->get(), NULL, pressed ? &rect : NULL);
	
	auto powerup_texture = get_img(powerup_images[descriptor.type]);
	SDL_RenderCopy(renderer, powerup_texture, NULL, NULL);
}

void PowerUpButton::draw_pressed(SDL_Renderer* renderer){
	draw_powerup(renderer, true);
}

void PowerUpButton::draw_released(SDL_Renderer* renderer){
	draw_powerup(renderer, false);
}

void PowerUpButton::draw_inactive(SDL_Renderer* renderer){
	draw_powerup(renderer, false);
}

void PowerUpButton::on_pressed(){
	play(Sound::CLICK);
	manipulator->set_allowed_powerup(descriptor, !view->get_settings().allowed_powerups.count(descriptor));
}

#define SETTINGS_X 0.02
#define NUMBER_BOX_W 0.025
#define NUMBER_BOX_H 0.07

#define SCORING_LABEL_Y 0.075
#define WIN_CRITERION_Y 0.11
#define WIN_CRITERION_W 0.5
#define WIN_CRITERION_H 0.05
#define WIN_CRITERION_MARGIN 0.01

#define SCORE_NUMBERS_LABEL_Y 0.195
#define SCORE_NUMBERS_Y 0.23

#define WIN_AMOUNT_X 0.05
#define TIE_BREAK_X 0.15

#define POWERUP_DIAM 0.06
#define POWERUP_INTERVAL 0.09
#define POWERUP_LABEL_Y 0.375
#define POWERUP_Y 0.425
#define POWERUP_TABLE_W 8

GameSettingsSubView::GameSettingsSubView(
	const SDL_Rect& rect,
	GameSettingsView* view, GameSettingsManipulator* manipulator
) : SubView(rect, false),
	view(view), manipulator(manipulator),
	using_score_numbers(false) {
		
	SDL_Rect menu_rect;
	
	menu_rect.x = SETTINGS_X * rect.w;
	menu_rect.y = WIN_CRITERION_Y * rect.h;
	menu_rect.w = WIN_CRITERION_W * rect.w;
	menu_rect.h = WIN_CRITERION_H * rect.h;
		
	win_criterion = make_unique<WinCriterionMenu>(
		menu_rect, WIN_CRITERION_MARGIN * rect.w,
		view, manipulator
	);
	view_manager.add_view(win_criterion.get());
	
	menu_rect.w = rect.w * NUMBER_BOX_W;
	menu_rect.h = rect.h * NUMBER_BOX_H;
	menu_rect.x = rect.w * WIN_AMOUNT_X - (menu_rect.w / 2);
	menu_rect.y = rect.h * SCORE_NUMBERS_Y;
	
	win_amount_button = make_unique<WinAmountButton>(
		menu_rect,
		view, manipulator
	);

	menu_rect.x = rect.w * TIE_BREAK_X - (menu_rect.w / 2);
	
	tie_break_button = make_unique<TieBreakButton>(
		menu_rect,
		view, manipulator
	);
	
	menu_rect.h = menu_rect.w = rect.w * POWERUP_DIAM;
	
	int dx = rect.w * POWERUP_INTERVAL;
	int i = 0;
	for(auto entry: powerup_times){
		menu_rect.x = rect.w * SETTINGS_X + dx * (i % POWERUP_TABLE_W);
		menu_rect.y = rect.h * POWERUP_Y + dx * (i / POWERUP_TABLE_W);
		
		powerups.push_back(make_unique<PowerUpButton>(menu_rect, entry.first, view, manipulator));
		view_manager.add_view(powerups.back().get());
		
		i += 1;
	}
}

void GameSettingsSubView::sync_display(){
	win_criterion->set_active(view->am_i_host());
	win_amount_button->set_active(using_score_numbers && view->am_i_host());
	tie_break_button->set_active(using_score_numbers && view->am_i_host());
	
	for(auto& powerup: powerups){
		powerup->set_active(view->am_i_host());
	}
	
	if(using_score_numbers && view->get_settings().scores.criterion == WinCriterion::NEVER){
		using_score_numbers = false;
		view_manager.remove_view(win_amount_button.get());
		view_manager.remove_view(tie_break_button.get());
	}
	else if(!using_score_numbers && view->get_settings().scores.criterion != WinCriterion::NEVER){
		using_score_numbers = true;
		view_manager.add_view(win_amount_button.get());
		view_manager.add_view(tie_break_button.get());
	}
}

bool GameSettingsSubView::on_event(const SDL_Event& event){
	sync_display();
	return view_manager.handle_event(event);
}

void GameSettingsSubView::draw_content(SDL_Renderer* renderer){
	sync_display();
	fill_back(renderer, bg_color);
	
	if(scores_label.get() == nullptr) scores_label = make_unique<Msg>(
		"victory:",
		text_color,
		FontType::NRM,
		renderer
	);
	if(score_label.get() == nullptr) score_label = make_unique<Msg>(
		"Score:",
		text_color,
		FontType::NRM,
		renderer
	);
	if(round_label.get() == nullptr) round_label = make_unique<Msg>(
		"Round:",
		text_color,
		FontType::NRM,
		renderer
	);
	if(tie_break_label.get() == nullptr) tie_break_label = make_unique<Msg>(
		"Tie break:",
		text_color,
		FontType::NRM,
		renderer
	);
	if(powerup_label.get() == nullptr) powerup_label = make_unique<Msg>(
		"Power-ups:",
		text_color,
		FontType::NRM,
		renderer
	);
	
	scores_label->render_centered(get_rect().w * SETTINGS_X, get_rect().h * SCORING_LABEL_Y, Align::LEFT);
	
	WinCriterion criterion = view->get_settings().scores.criterion;
	if(criterion == WinCriterion::BY_SCORE){
		score_label->render_centered(get_rect().w * WIN_AMOUNT_X, get_rect().h * SCORE_NUMBERS_LABEL_Y, Align::CENTER);
	}
	else if(criterion == WinCriterion::BY_ROUND){
		round_label->render_centered(get_rect().w * WIN_AMOUNT_X, get_rect().h * SCORE_NUMBERS_LABEL_Y, Align::CENTER);
	}

	if(criterion != WinCriterion::NEVER){
		tie_break_label->render_centered(get_rect().w * TIE_BREAK_X, get_rect().h * SCORE_NUMBERS_LABEL_Y, Align::CENTER);
	}

	powerup_label->render_centered(get_rect().w * SETTINGS_X, get_rect().h * POWERUP_LABEL_Y, Align::LEFT);
	
	view_manager.draw(renderer);
}

void GameSettingsSubView::lose_focus(){
	sync_display();
	view_manager.lose_focus();
}

void GameSettingsSubView::step(){
	sync_display();
	view_manager.step();
}

TeamChangeButton::TeamChangeButton(
	const SDL_Rect& rect,
	bool direction,
	PlayerView& view
) : Button(rect, false),
	direction(direction),
	view(view) {}

void TeamChangeButton::draw_arrow(SDL_Renderer* renderer, const SDL_Color& color){
	SDL_SetRenderDrawColor(renderer, clear_color.r, clear_color.g, clear_color.b, clear_color.a);
	SDL_RenderClear(renderer);
	
	SDL_Vertex vertices[3] = {
		{
			.position = {
				.x = (float)(0.5 * get_rect().w),
				.y = (float)(direction ? 0 : get_rect().h),
			},
			.color = color,
		},
		{
			.position = {
				.x = (float)(get_rect().w),
				.y = (float)(direction ? get_rect().h : 0),
			},
			.color = color,
		},
		{
			.position = {
				.x = 0.0f,
				.y = (float)(direction ? get_rect().h : 0),
			},
			.color = color,
		},
	};
	
	SDL_RenderGeometry(renderer, NULL, vertices, 3, NULL, 0);
}

void TeamChangeButton::draw_pressed(SDL_Renderer* renderer){
	draw_arrow(renderer, text_color);
}

void TeamChangeButton::draw_released(SDL_Renderer* renderer){
	draw_arrow(renderer, line_color);
}

void TeamChangeButton::draw_inactive(SDL_Renderer* renderer){
	draw_arrow(renderer, clear_color);
}

void TeamChangeButton::on_pressed(){
	play(Sound::CLICK);
	view.set_team(view.get_team() + (direction ? -1 : 1));
}

ReadyButton::ReadyButton(
	const SDL_Rect& rect,
	PlayerView& player,
	GameSettingsView* view,
	GameSettingsManipulator* manipulator
) : Button(rect, true), player(player), view(view), manipulator(manipulator) {}

bool ReadyButton::is_ready() const {
	return view->get_ready().count(player.get_player());
}

void ReadyButton::draw(SDL_Renderer* renderer, bool is_pressed){
	if(ready.get() == nullptr || pressed != is_pressed) ready = make_unique<Msg>(
		"Ready",
		is_pressed ? active_color : text_color,
		FontType::NRM,
		renderer
	);

	if(not_ready.get() == nullptr || pressed != is_pressed) not_ready = make_unique<Msg>(
		"Not Ready",
		is_pressed ? active_color : text_color,
		FontType::NRM,
		renderer
	);
	
	pressed = is_pressed;
	
	fill_back(renderer, clear_color);
	(is_ready() ? ready : not_ready)->render_centered(get_rect().w, get_rect().h / 2, Align::RIGHT);
}

void ReadyButton::draw_pressed(SDL_Renderer* renderer){
	draw(renderer, true);
}

void ReadyButton::draw_released(SDL_Renderer* renderer){
	draw(renderer, false);
}

void ReadyButton::draw_inactive(SDL_Renderer* renderer){
	draw(renderer, false);
}

void ReadyButton::on_pressed(){
	manipulator->set_ready(!view->am_i_ready());
}

int ReadyButton::get_width() const {
	auto& msg = is_ready() ? ready : not_ready;
	
	return msg.get() == nullptr ? 0 : msg->get_width();
}

#define LINE_MARGIN 0.01

#define PLAYER_NAME_X 0.04
#define TEAM_NAME_X 0.02
#define PLAYER_STATUS_X 0.95
#define PLAYER_READY_W 0.085

#define TEAM_CHANGE_H 0.4
#define TEAM_CHANGE_W 0.02
#define TEAM_CHANGE_X 0.96
#define TEAM_CHANGE_DY 0.05

PlayerView::PlayerView(
	const SDL_Rect& rect,
	int player,
	GameSettingsView* view, GameSettingsManipulator* manipulator,
	bool multi_peer
) : SubView(rect, true),
	player(player),
	view(view), manipulator(manipulator),
	using_ready_button(false),
	multi_peer(multi_peer) {
		
	SDL_Rect button_rect;
	button_rect.x = rect.w * TEAM_CHANGE_X;
	button_rect.w = rect.w * TEAM_CHANGE_W;
	button_rect.h = rect.h * TEAM_CHANGE_H;
	
	button_rect.y = rect.h * (0.5 + TEAM_CHANGE_DY);
	inc = make_unique<TeamChangeButton>(button_rect, false, *this);
	view_manager.add_view(inc.get());

	button_rect.y = rect.h * (0.5 - TEAM_CHANGE_DY) - button_rect.h;
	dec = make_unique<TeamChangeButton>(button_rect, true, *this);
	view_manager.add_view(dec.get());
	
	button_rect.h = rect.h;
	button_rect.w = rect.w * PLAYER_READY_W;
	button_rect.y = 0;
	button_rect.x = (rect.w * PLAYER_STATUS_X) - button_rect.w;
	ready_button = make_unique<ReadyButton>(
		button_rect,
		*this,
		view,
		manipulator
	);
}
	
void PlayerView::sync_display(){
	bool mine = false;
	for(auto my_player: view->get_my_players()){
		if(player == my_player) mine = true;
	}
	bool change_teams = mine && view->get_settings().using_teams;
	
	int team = view->get_settings().teams[player];
	
	inc->set_active(change_teams && team < view->get_settings().team_names.size() -1);
	dec->set_active(change_teams && team > 0);
	
	if(!multi_peer || count(view->get_host_players(), player)){
		if(using_ready_button){
			view_manager.remove_view(ready_button.get());
			using_ready_button = false;
		}
	}
	else if(!using_ready_button){
		view_manager.add_view(ready_button.get());
		using_ready_button = true;
	}
	
	ready_button->set_active(mine && using_ready_button);
}

void PlayerView::draw_content(SDL_Renderer* renderer){
	sync_display();
	if(
		name_label.get() == nullptr ||
		color != view->get_settings().colors[player] ||
		name != view->get_settings().names[player]
	){
		name = view->get_settings().names[player];
		color = view->get_settings().colors[player];
		
		char name_buffer[50];
		snprintf(name_buffer, 50, "player-%d", player);
		
		const auto& player_texture = player_textures[color];

		name_label = make_unique<Msg>(
			name.size() ? name.c_str() : name_buffer,
			player_texture.get_color(), FontType::NRM,
			renderer,
			player_texture.get_texture()
		);
	}
	
	if(host_label.get() == nullptr) host_label = make_unique<Msg>(
		"Host",
		text_color, FontType::NRM,
		renderer
	);

	fill_back(renderer, clear_color);
	
	int text_x = get_rect().w * (view->get_settings().using_teams ? PLAYER_NAME_X : TEAM_NAME_X);
	name_label->render_centered(text_x, get_rect().h / 2, Align::LEFT);
	
	int line_start_x = text_x + name_label->get_width() + (get_rect().w * LINE_MARGIN);
	
	text_x = get_rect().w * PLAYER_STATUS_X;
	
	int line_end_x = text_x - (get_rect().w * LINE_MARGIN);
	if(multi_peer){
		if(count(view->get_host_players(), player)){
			host_label->render_centered(text_x, get_rect().h / 2, Align::RIGHT);
			line_end_x -= host_label->get_width();
		} else {
			line_end_x -= ready_button->get_width();
		}
	}
	
	SDL_SetRenderDrawColor(renderer, active_color.r, active_color.g, active_color.b, active_color.a);
	SDL_RenderDrawLine(
		renderer,
		line_start_x, get_rect().h / 2,
		line_end_x, get_rect().h / 2
	);
	
	view_manager.draw(renderer);
}

bool PlayerView::on_event(const SDL_Event& event){
	sync_display();
	return view_manager.handle_event(event);
}

void PlayerView::lose_focus(){
	sync_display();
	view_manager.lose_focus();
}

void PlayerView::step(){
	sync_display();
	view_manager.step();
}

int PlayerView::get_player(){
	return player;
}

void PlayerView::set_player(int player){
	if(name.size() == 0 && this->player != player) name_label = nullptr;
	this->player = player;
}

int PlayerView::get_team(){
	return view->get_settings().teams[player];
}

void PlayerView::set_team(int team){
	for(int i = 0; i < view->get_my_players().size(); i++){
		if(view->get_my_players()[i] == player) manipulator->set_player_team(i, team);
	}
}

TeamNameBox::TeamNameBox(
	const SDL_Rect& rect, int margin,
	TeamSeparator& view
) : TextBox(
		rect, true,
		FontType::NRM,
		margin,
		text_color, {72, 72, 72, 32}
	),
	view(view) {}

void TeamNameBox::draw_back(SDL_Renderer* renderer, bool typing){
	fill_back(renderer, clear_color);
}

void TeamNameBox::on_set(const string& text){
	play(Sound::CLICK);
	view.set_name(text);
}

void TeamNameBox::on_active(){
	play(Sound::CLICK);
}

string TeamNameBox::get_default_text(){
	char name[50];
	snprintf(name, 50, "team-%d", view.get_team());
	return name;
}

#define REMOVE_CROSS_RATIO (1.0/3.0)

TeamRemoveButton::TeamRemoveButton(const SDL_Rect& rect, TeamSeparator& view) : Button(rect, true), view(view) {}

void TeamRemoveButton::init(SDL_Renderer* renderer){
	if(cross.get() == nullptr) cross = make_cross(renderer, get_rect().w, REMOVE_CROSS_RATIO);
}

void TeamRemoveButton::draw_cross(SDL_Renderer* renderer){
	fill_back(renderer, clear_color);
	SDL_RenderCopyEx(renderer, cross->get(), NULL, NULL, 45.0, NULL, SDL_FLIP_NONE);
}

void TeamRemoveButton::draw_pressed(SDL_Renderer* renderer){
	init(renderer);
	SDL_SetTextureColorMod(cross->get(), 64, 0, 0);
	draw_cross(renderer);
}

void TeamRemoveButton::draw_released(SDL_Renderer* renderer){
	init(renderer);
	SDL_SetTextureColorMod(cross->get(), 128, 0, 0);
	draw_cross(renderer);
}

void TeamRemoveButton::draw_inactive(SDL_Renderer* renderer){
	init(renderer);
	SDL_SetTextureColorMod(cross->get(), 128, 64, 64);
	draw_cross(renderer);
}

void TeamRemoveButton::on_pressed(){
	play(Sound::CLICK);
	view.remove();
}

#define TEAM_REMOVE_BUTTON_SIZE 0.5
#define TEAM_REMOVE_BUTTON_X 0.98

TeamSeparator::TeamSeparator(
	const SDL_Rect& rect,
	int team,
	GameSettingsView* view, GameSettingsManipulator* manipulator
) : SubView(rect, true),
	team(team),
	view(view), manipulator(manipulator) {

	SDL_Rect view_rect;
	view_rect.x = view_rect.y = 0;
	view_rect.h = rect.h;
	view_rect.w = rect.w * (TEAM_REMOVE_BUTTON_X - LINE_MARGIN) - rect.h / 2;
	
	name = make_unique<TeamNameBox>(view_rect, rect.w * TEAM_NAME_X, *this);
	view_manager.add_view(name.get());
}

void TeamSeparator::sync_display(){
	if(view->am_i_host() && remove_button.get() == nullptr){
		SDL_Rect rect;
		rect.w = rect.h = get_rect().h * TEAM_REMOVE_BUTTON_SIZE;
		rect.y = (get_rect().h - rect.h) / 2;
		rect.x = get_rect().w * TEAM_REMOVE_BUTTON_X - rect.w / 2;

		remove_button = make_unique<TeamRemoveButton>(rect, *this);
		view_manager.add_view(remove_button.get());			
	}
	else if(!view->am_i_host() && remove_button.get() != nullptr){
		view_manager.remove_view(remove_button.get());
		remove_button = nullptr;
	}
	
	bool is_my_team = false;
	for(auto player: view->get_my_players()){
		if(view->get_settings().teams[player] == team) is_my_team = true;
	}
	name->set_active(is_my_team);
}

void TeamSeparator::draw_content(SDL_Renderer* renderer){
	sync_display();

	fill_back(renderer, clear_color);
	view_manager.draw(renderer);

	int end_x = get_rect().w * TEAM_REMOVE_BUTTON_X;
	if(view->am_i_host()) end_x -= get_rect().h / 2;

	SDL_SetRenderDrawColor(renderer, line_color.r, line_color.g, line_color.b, line_color.a);
	SDL_RenderDrawLine(renderer,
		get_rect().w * (TEAM_NAME_X + LINE_MARGIN) + name->get_text_width(), get_rect().h / 2,
		end_x, get_rect().h / 2
	);
}

bool TeamSeparator::on_event(const SDL_Event& event){
	sync_display();
	return view_manager.handle_event(event);
}

void TeamSeparator::lose_focus(){
	sync_display();
	view_manager.lose_focus();
}

void TeamSeparator::step(){
	sync_display();
	view_manager.step();
}

int TeamSeparator::get_team(){
	return team;
}

void TeamSeparator::set_team(int team){
	if(team != this->team) name->update();
	this->team = team;
}

void TeamSeparator::set_name(string name){
	manipulator->set_team_name(team, name);
}

void TeamSeparator::remove(){
	manipulator->remove_team(team);
}

TeamsButton::TeamsButton(
	const SDL_Rect& rect,
	GameSettingsView* view, GameSettingsManipulator* manipulator
) : Button(rect, false), view(view), manipulator(manipulator) {}

void TeamsButton::draw_button(SDL_Renderer* renderer, const SDL_Color& color){
	fill_back(renderer, color);
	draw_frame(renderer, line_color);
	
	if(label.get() == nullptr || state != view->get_settings().using_teams) {
		state = view->get_settings().using_teams;
		
		label = make_unique<Msg>(
			state ? "Single player" : "Teams",
			text_color,
			FontType::NRM,
			renderer
		);
	}
	
	label->render_centered(get_rect().w / 2, get_rect().h / 2, Align::CENTER);
}

void TeamsButton::draw_pressed(SDL_Renderer* renderer){
	draw_button(renderer, active_color);
}

void TeamsButton::draw_released(SDL_Renderer* renderer){
	draw_button(renderer, bg_color);
}

void TeamsButton::draw_inactive(SDL_Renderer* renderer){
	draw_button(renderer, bg_color);
}

void TeamsButton::on_pressed(){
	play(Sound::CLICK);
	manipulator->set_teams(!view->get_settings().using_teams);
}

TeamAddButton::TeamAddButton(
	const SDL_Rect& rect,
	GameSettingsManipulator* manipulator
) : Button(rect, false), manipulator(manipulator) {}

void TeamAddButton::draw_button(SDL_Renderer* renderer, const SDL_Color& color){
	fill_back(renderer, color);
	draw_frame(renderer, line_color);
	
	if(label.get() == nullptr) label = make_unique<Msg>(
		"Add Team",
		text_color,
		FontType::NRM,
		renderer
	);
	
	label->render_centered(get_rect().w / 2, get_rect().h / 2, Align::CENTER);
}

void TeamAddButton::draw_pressed(SDL_Renderer* renderer){
	draw_button(renderer, active_color);
}

void TeamAddButton::draw_released(SDL_Renderer* renderer){
	draw_button(renderer, bg_color);
}

void TeamAddButton::draw_inactive(SDL_Renderer* renderer){
	draw_button(renderer, bg_color);
}

void TeamAddButton::on_pressed(){
	play(Sound::CLICK);
	manipulator->add_team();
}

#define PLAYER_VIEW_H 0.07
#define TEAM_VIEW_H 0.05

#define PLAYERS_SPACE 0.03

#define TEAMS_BUTTON_MARGIN 0.02
#define TEAMS_BUTTON_H 0.07

PlayersView::PlayersView(
	const SDL_Rect& rect,
	GameSettingsView* view, GameSettingsManipulator* manipulator,
	bool multi_peer
) : SubView(rect, false),
	view(view), manipulator(manipulator),
	multi_peer(multi_peer) {

	SDL_Rect button_rect;
	button_rect.x = rect.w * TEAMS_BUTTON_MARGIN;
	button_rect.w = rect.w - button_rect.x * 2;
	button_rect.h = rect.h * TEAMS_BUTTON_H;
	button_rect.y = -button_rect.h;

	using_teams_button = make_unique<TeamsButton>(button_rect, view, manipulator);
	view_manager.add_view(using_teams_button.get());
	
	button_rect.x = rect.w * TEAMS_BUTTON_MARGIN;
	button_rect.w = rect.w - button_rect.x * 2;
	button_rect.h = rect.h * TEAMS_BUTTON_H;
	button_rect.y = rect.h;

	add_team_button = make_unique<TeamAddButton>(button_rect, manipulator);
	view_manager.add_view(add_team_button.get());
}

void PlayersView::sync_displays(){
	SDL_Rect rect;
	rect.x = 0;
	rect.w = get_rect().w;
	rect.y = get_rect().h;
	
	rect.h = get_rect().h * PLAYER_VIEW_H;
	for(int i = 0; i < players.size(); i++){
		players[i]->set_player(i);
	}
	while(players.size() < view->get_settings().teams.size()){
		players.push_back(make_unique<PlayerView>(
			rect,
			players.size(),
			view, manipulator,
			multi_peer
		));
		view_manager.add_view(players.back().get());
	}

	if(view->get_settings().using_teams){
		rect.h = get_rect().h * TEAM_VIEW_H;
		for(int i = 0; i < teams.size(); i++){
			teams[i]->set_team(i);
		}
		while(teams.size() < view->get_settings().team_names.size()){
			teams.push_back(make_unique<TeamSeparator>(
				rect,
				teams.size(),
				view, manipulator
			));
			view_manager.add_view(teams.back().get());
		}
	}
	else{
		for(auto& team: teams) view_manager.remove_view(team.get());
		teams.clear();
	}
	
	using_teams_button->set_active(view->am_i_host());
	
	add_team_button->set_active(view->am_i_host() && view->get_settings().using_teams);
}

bool PlayersView::on_event(const SDL_Event& event){
	sync_displays();
	return view_manager.handle_event(event);
}

void PlayersView::draw_content(SDL_Renderer* renderer){
	sync_displays();
	fill_back(renderer, bg_color);

	view_manager.draw(renderer);
}

void PlayersView::lose_focus(){
	sync_displays();
	view_manager.lose_focus();
}

#define PLAYER_FOLLOW_SPEED 5

void PlayersView::step(){
	sync_displays();

	removed_players.clear();
	removed_teams.clear();

	vector<SubView*> views;
	
	if(view->get_settings().using_teams){
		vector<vector<int>> team_players(teams.size());
		
		for(int i = 0; i < view->get_settings().teams.size(); i++){
			team_players[view->get_settings().teams[i]].push_back(i);
		}
		
		for(int i = 0; i < teams.size(); i++){
			views.push_back(teams[i].get());
			for(auto player: team_players[i]){
				views.push_back(players[player].get());
			}
		}
	}
	else{
		for(auto& view: players) views.push_back(view.get());
	}
	
	int y = get_rect().h * PLAYERS_SPACE;
	
	int using_teams_button_y = -using_teams_button->get_rect().h;

	if(view->am_i_host()){
		using_teams_button_y = y;
		y += get_rect().h * PLAYERS_SPACE + using_teams_button->get_rect().h;
	}
	
	using_teams_button->move(using_teams_button->get_rect().x, follow(
		using_teams_button->get_rect().y,
		using_teams_button_y,
		PLAYER_FOLLOW_SPEED
	));

	for(auto view: views){
		view->move(0, follow(
			view->get_rect().y,
			y,
			PLAYER_FOLLOW_SPEED
		));

		y += view->get_rect().h;
	}
	
	int add_team_button_y = get_rect().h;
	
	if(view->am_i_host() && view->get_settings().using_teams){
		y += get_rect().h * PLAYERS_SPACE;
		
		add_team_button_y = y;
	}

	add_team_button->move(add_team_button->get_rect().x, follow(
		add_team_button->get_rect().y,
		add_team_button_y,
		PLAYER_FOLLOW_SPEED
	));

	view_manager.step();
}

void PlayersView::remove_player(int player_num){
	removed_players.push_back(pop_index(players, player_num));
	view_manager.remove_view(removed_players.back().get());
	
	sync_displays();
}


void PlayersView::remove_team(int team_num) {
	if(team_num < teams.size()) {
		removed_teams.push_back(pop_index(teams, team_num));
		view_manager.remove_view(removed_teams.back().get());
	}
	sync_displays();
}

void PlayersView::init(const GameSettings& settings){};
void PlayersView::add_player(int team, int color){}
void PlayersView::add_team() {}
void PlayersView::set_player_index(int index, int player) {}
void PlayersView::set_player_name(int player, string name) {}
void PlayersView::set_player_color(int player, int color) {}
void PlayersView::set_teams(bool using_teams) {}
void PlayersView::set_player_team(int player, int team) {}
void PlayersView::set_team_name(int team, string name) {}
void PlayersView::set_allowed_powerup(PowerUpDescriptor desc, bool allowed) {}
void PlayersView::set_win_criterion(WinCriterion criterion) {}
void PlayersView::set_win_amount(int amount) {}
void PlayersView::set_tie_break(int threshold) {}
void PlayersView::set_host_player(int player) {}
void PlayersView::set_host() {}
void PlayersView::player_ready(int player, bool is_ready) {}
void PlayersView::reset_all_ready() {}
void PlayersView::start_countdown() {}
void PlayersView::start_game() {}

#define MENU_H 0.05

GameSettingsMenu::GameSettingsMenu(
	const SDL_Rect& rect,
	GameSettingsView* view, GameSettingsManipulator* manipulator, GameSettingsObserverAccumulator* accumulator,
	bool multi_peer
) : view(view), manipulator(manipulator), accumulator(accumulator), multi_peer(multi_peer),
	TabView(
		rect, rect.h * MENU_H, false,
		FontType::NRM, ::text_color, 20
	) {}
	
GameSettingsMenu::~GameSettingsMenu(){
	if(players.get() != nullptr){
		accumulator->remove_observer(players.get());
	}
}

vector<TabView::ViewDescriptor> GameSettingsMenu::init_subviews(const SDL_Rect& rect){
	settings = make_unique<GameSettingsSubView>(rect, view, manipulator);
	players = make_unique<PlayersView>(rect, view, manipulator, multi_peer);
	accumulator->add_observer(players.get());

	return vector<TabView::ViewDescriptor>({
		{
			.view = players.get(),
			.name = "Players",
		},
		{
			.view = settings.get(),
			.name = "Settings",
		},
	});
}

void GameSettingsMenu::on_changed(){
	play(Sound::CLICK);
}

void GameSettingsMenu::draw_button_back(SDL_Renderer* renderer, const SubView& view, TabView::State state){
	view.fill_back(renderer, state == TabView::State::PRESSED ? active_color : bg_color);

	SDL_SetRenderDrawColor(renderer, line_color.r, line_color.g, line_color.b, line_color.a);
	SDL_Rect rect = view.get_rect();

	switch(state){
	case TabView::State::CURRENT:
		rect.h += 1;
	case TabView::State::PRESSED:
	case TabView::State::RELEASED:
	case TabView::State::BACK:
		rect.x = rect.y = 0;
		
		SDL_RenderDrawRect(renderer, &rect);
		break;
	}
}


