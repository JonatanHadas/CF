#include "game_settings_subview.h"

#include "colors.h"
#include "images.h"

#include "powerup_images.h"
#include "../game/powerups.h"

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
		line_color, text_color, text_color,
		FontType::NRM
	),
	view(view), manipulator(manipulator) {}

void WinCriterionMenu::on_set(int option){
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

PlayersView::PlayersView(
	const SDL_Rect& rect,
	GameSettingsView* view, GameSettingsManipulator* manipulator,
	bool multi_peer
) : SubView(rect, false),
	view(view), manipulator(manipulator),
	multi_peer(multi_peer) {}

bool PlayersView::on_event(const SDL_Event& event){
	return false;
}

void PlayersView::draw_content(SDL_Renderer* renderer){
	fill_back(renderer, bg_color);
}

void PlayersView::lose_focus(){}

void PlayersView::step(){}

#define MENU_H 0.05

GameSettingsMenu::GameSettingsMenu(
	const SDL_Rect& rect,
	GameSettingsView* view, GameSettingsManipulator* manipulator,
	bool multi_peer
) : view(view), manipulator(manipulator), multi_peer(multi_peer),
	TabView(
		rect, rect.h * MENU_H, false,
		FontType::NRM, ::text_color, 20
	) {}

vector<TabView::ViewDescriptor> GameSettingsMenu::init_subviews(const SDL_Rect& rect){
	settings = make_unique<GameSettingsSubView>(rect, view, manipulator);
	players = make_unique<PlayersView>(rect, view, manipulator, multi_peer);

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
