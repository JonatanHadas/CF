#include "players_subview.h"

#include "colors.h"
#include "player_texture.h"
#include "sounds.h"
#include "../utils/utils.h"
#include "gui_utils.h"

#define REMOVE_CROSS_RATIO (1.0/3.0)

PlayerRemoveButton::PlayerRemoveButton(const SDL_Rect& rect, PlayerSubView& view) :
	Button(rect, true),
	view(view) {}

void PlayerRemoveButton::init(SDL_Renderer* renderer){
	cross = make_cross(renderer, get_rect().w, REMOVE_CROSS_RATIO);
}
void PlayerRemoveButton::draw_button(SDL_Renderer* renderer){
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
	SDL_RenderClear(renderer);
	
	SDL_RenderCopyEx(renderer, cross->get(), NULL, NULL, 45.0, NULL, SDL_FLIP_NONE);
}

void PlayerRemoveButton::draw_pressed(SDL_Renderer* renderer){
	if(cross.get() == nullptr) init(renderer);
	SDL_SetTextureColorMod(cross->get(), 64, 0, 0);
	draw_button(renderer);
}

void PlayerRemoveButton::draw_released(SDL_Renderer* renderer){
	if(cross.get() == nullptr) init(renderer);
	SDL_SetTextureColorMod(cross->get(), 128, 0, 0);
	draw_button(renderer);
}

void PlayerRemoveButton::draw_inactive(SDL_Renderer* renderer){
	if(cross.get() == nullptr) init(renderer);
	SDL_SetTextureColorMod(cross->get(), 128, 64, 64);
	draw_button(renderer);
}

void PlayerRemoveButton::on_pressed(){
	play(Sound::CLICK);
	view.remove();
}

PlayerNameBox::PlayerNameBox(
	const SDL_Rect& rect, int margin,
	TextCompleter& completer,
	PlayerSubView& view
) : TextBox(
	rect, false,
	FontType::NRM,
	margin,
	text_color, text_completion_color,
	completer
	),
	view(view) {}

void PlayerNameBox::draw_back(SDL_Renderer* renderer, bool typing){
	fill_back(renderer, typing ? active_color : bg_color);
	draw_frame(renderer, line_color);
}

void PlayerNameBox::on_set(const string& text){
	play(Sound::CLICK);
	view.set_name(text);
	add_last_as_suggestion();
}

void PlayerNameBox::on_active(){
	play(Sound::CLICK);
}

string PlayerNameBox::get_default_text(){
	return "player name";
}

SDL_Scancode choose_key_const(const KeySet& keyset, function<SDL_Scancode&(KeySet&)> key_choice){
	KeySet keys = keyset;
	return key_choice(keys);
}

PlayerKeyButton::PlayerKeyButton(
	const SDL_Rect& rect, PlayerSubView& view,
	function<SDL_Scancode&(KeySet&)> key_choice
) :
	KeyChoiceButton(
		rect, false,
		FontType::NRM, text_color,
		choose_key_const(view.get_keys(), key_choice)
	),
	key_choice(key_choice),
	view(view) {}

bool PlayerKeyButton::on_set(SDL_Scancode key){
	play(Sound::CLICK);
	KeySet keys = view.get_keys();
	key_choice(keys) = key;
	view.set_keys(keys);
	return true;
}

void PlayerKeyButton::on_active(){
	play(Sound::CLICK);
}

void PlayerKeyButton::draw_back(SDL_Renderer* renderer, bool active, bool empty){
	if(active) fill_back(renderer, active_color);
	else fill_back(renderer, empty ? SDL_Color({64, 0, 0, 255}) : bg_color);
	draw_frame(renderer, line_color);
}

PlayerColorButton::PlayerColorButton(const SDL_Rect& rect, PlayerSubView& view) :
	Button(rect, true),
	view(view) {}

void PlayerColorButton::draw_back(SDL_Renderer* renderer){
	int color = view.get_color();
	if(color == -1) fill_back(renderer, clear_color);
	else{
		const auto& player_texture = player_textures[color];
		if(NULL == player_texture.get_texture()) fill_back(renderer, player_texture.get_color());
		else{
			SDL_Color old_color_mod;
			SDL_GetTextureColorMod(player_texture.get_texture(), &old_color_mod.r, &old_color_mod.g, &old_color_mod.b);
			SDL_GetTextureAlphaMod(player_texture.get_texture(), &old_color_mod.a);
			
			SDL_SetTextureColorMod(player_texture.get_texture(), player_texture.get_color().r, player_texture.get_color().g, player_texture.get_color().b);
			SDL_SetTextureAlphaMod(player_texture.get_texture(), player_texture.get_color().a);
			
			SDL_SetTextureBlendMode(player_texture.get_texture(), SDL_BLENDMODE_NONE);
			SDL_RenderCopy(renderer, player_texture.get_texture(), NULL, NULL);

			SDL_SetTextureColorMod(player_texture.get_texture(), old_color_mod.r, old_color_mod.g, old_color_mod.b);
			SDL_SetTextureAlphaMod(player_texture.get_texture(), old_color_mod.a);
		}
	}
	
}

void PlayerColorButton::draw_pressed(SDL_Renderer* renderer){
	draw_back(renderer);
	draw_frame(renderer, line_color2);
}

void PlayerColorButton::draw_released(SDL_Renderer* renderer){
	draw_back(renderer);
	draw_frame(renderer, line_color);
}

void PlayerColorButton::draw_inactive(SDL_Renderer* renderer){
	fill_back(renderer, clear_color);
}

void PlayerColorButton::on_pressed(){
	play(Sound::CLICK);
	view.set_color_menu_open(true);
}

ColorButton::ColorButton(const SDL_Rect& rect, int color, PlayerSubView& view) :
	Button(rect, false),
	color(color),
	view(view) {}
	
#define COLOR_CROSS_RATIO (1.0/3.0)

void ColorButton::init(SDL_Renderer* renderer){
	if(cross.get() == nullptr){
		cross = make_cross(renderer, min(get_rect().w, get_rect().h), COLOR_CROSS_RATIO);
		SDL_SetTextureColorMod(cross->get(), 0, 0, 0);
	}
}

void ColorButton::draw_back(SDL_Renderer* renderer){
	const auto& player_texture = player_textures[color];
	if(NULL == player_texture.get_texture()) fill_back(renderer, player_texture.get_color());
	else{
		SDL_Color old_color_mod;
		SDL_GetTextureColorMod(player_texture.get_texture(), &old_color_mod.r, &old_color_mod.g, &old_color_mod.b);
		SDL_GetTextureAlphaMod(player_texture.get_texture(), &old_color_mod.a);
		
		SDL_SetTextureColorMod(player_texture.get_texture(), player_texture.get_color().r, player_texture.get_color().g, player_texture.get_color().b);
		SDL_SetTextureAlphaMod(player_texture.get_texture(), player_texture.get_color().a);
		
		SDL_SetTextureBlendMode(player_texture.get_texture(), SDL_BLENDMODE_NONE);
		SDL_RenderCopy(renderer, player_texture.get_texture(), NULL, NULL);

		SDL_SetTextureColorMod(player_texture.get_texture(), old_color_mod.r, old_color_mod.g, old_color_mod.b);
		SDL_SetTextureAlphaMod(player_texture.get_texture(), old_color_mod.a);
	}
}

void ColorButton::draw_pressed(SDL_Renderer* renderer){
	draw_back(renderer);
	draw_frame(renderer, line_color);
}

void ColorButton::draw_released(SDL_Renderer* renderer){
	draw_back(renderer);
}

void ColorButton::draw_inactive(SDL_Renderer* renderer){
	init(renderer);
	draw_back(renderer);
	
	SDL_RenderCopyEx(renderer, cross->get(), NULL, NULL, 45.0, NULL, SDL_FLIP_NONE);	
}

void ColorButton::on_pressed(){
	play(Sound::CLICK);
	view.set_color(color);
}

ColorMenu::ColorMenu(const SDL_Rect& first_rect, int width, PlayerSubView& view) :
	SubView({
		.x = first_rect.x,
		.y = first_rect.y,
		.w = first_rect.w * width,
		.h = first_rect.h * (((int)player_textures.size() + width - 1)/width)
	}, true),
	view(view) {
		
	SDL_Rect button_rect = first_rect;
	
	for(int i = 0; i < player_textures.size(); i++){
		button_rect.x = (i % width) * first_rect.w;
		button_rect.y = (i / width) * first_rect.h;
		buttons.push_back(make_unique<ColorButton>(button_rect, i, view));
		
		manager.add_view(buttons.back().get());
	}
}

bool ColorMenu::on_event(const SDL_Event& event){
	return manager.handle_event(event);
}

void ColorMenu::draw_content(SDL_Renderer* renderer){
	fill_back(renderer, clear_color);
	
	manager.draw(renderer);
}

void ColorMenu::lose_focus(){
	manager.lose_focus();

	view.set_color_menu_open(false);
}

void ColorMenu::step(){	
	for(int i = 0; i < buttons.size(); i++){
		buttons[i]->set_active(view.get_settings().get_colors().count(i) == 0);
	}

	manager.step();
}

#define PLAYER_VIEW_MARGIN_RATIO 0.05
#define PLAYER_NAME_WIDTH 0.85
#define PLAYER_NAME_HEIGHT 0.2

#define PLAYER_KEY_HEIGHT 0.2
#define PLAYER_KEY_Y 0.65
#define PLAYER_KEY_LABEL_Y 0.5

#define PLAYER_COLOR_HEIGHT 0.17
#define PLAYER_COLOR_X 0.3
#define PLAYER_COLOR_Y 0.4
#define COLOR_MENU_W 6

PlayerSubView::PlayerSubView(
	const SDL_Rect& rect,
	int id, PlayerSettings& settings,
	TextCompleter& name_completer,
	KeySetManager& key_manager
) :
	SubView(rect, false),
	id(id),
	keys_id(key_manager.get_new()),
	key_manager(key_manager),
	showing_color_menu(false),
	settings(settings) {
		
	SDL_Rect button_rect;
	button_rect.w = button_rect.h = rect.w * PLAYER_VIEW_MARGIN_RATIO;
	button_rect.y = button_rect.h / 2;
	button_rect.x = rect.w - (3 * button_rect.w / 2);
	
	remove_button = make_unique<PlayerRemoveButton>(button_rect, *this);
	view_manager.add_view(remove_button.get());
	
	button_rect.y = button_rect.x = rect.w * PLAYER_VIEW_MARGIN_RATIO;
	button_rect.w = rect.w * PLAYER_NAME_WIDTH;
	button_rect.h = rect.h * PLAYER_NAME_HEIGHT;
	
	name_box = make_unique<PlayerNameBox>(
		button_rect, button_rect.x,
		name_completer,
		*this
	);
	view_manager.add_view(name_box.get());
	
	button_rect.x = rect.w * PLAYER_VIEW_MARGIN_RATIO;
	button_rect.y = rect.h * PLAYER_KEY_Y;
	button_rect.w = button_rect.h = rect.h * PLAYER_KEY_HEIGHT;
	
	left_key = make_unique<PlayerKeyButton>(button_rect, *this, [](KeySet& keys) -> SDL_Scancode& { return keys.left; });
	view_manager.add_view(left_key.get());
	
	button_rect.x += button_rect.w + (rect.w * PLAYER_VIEW_MARGIN_RATIO);
	
	right_key = make_unique<PlayerKeyButton>(button_rect, *this, [](KeySet& keys) -> SDL_Scancode& { return keys.right; });
	view_manager.add_view(right_key.get());
	
	button_rect.x = rect.w * PLAYER_COLOR_X;
	button_rect.y = rect.h * PLAYER_COLOR_Y;
	button_rect.w = button_rect.h = rect.h * PLAYER_COLOR_HEIGHT;
	
	color_button = make_unique<PlayerColorButton>(button_rect, *this);
	view_manager.add_view(color_button.get());	

	button_rect.x += button_rect.w;
	color_menu = make_unique<ColorMenu>(button_rect, COLOR_MENU_W, *this);
}

PlayerSubView::~PlayerSubView(){
	key_manager.remove(keys_id);
}

bool PlayerSubView::on_event(const SDL_Event& event){
	if(event.type == SDL_MOUSEBUTTONDOWN && !color_menu->check_inside(event.motion.x, event.motion.y)){
		set_color_menu_open(false);
	}
	
	return view_manager.handle_event(event);
}

void PlayerSubView::draw_content(SDL_Renderer* renderer){
	fill_back(renderer, bg_color);
	draw_frame(renderer, line_color);
	
	if(keys_label.get() == nullptr) keys_label = make_unique<Msg>(
		"Keys:",
		text_color,
		FontType::NRM,
		renderer
	);
	
	int keys_label_x = (left_key->get_rect().x + right_key->get_rect().x + right_key->get_rect().w) / 2;
	keys_label->render_centered(
		keys_label_x,
		get_rect().h * PLAYER_KEY_LABEL_Y,
		Align::CENTER
	);
	
	name_box->set_text(settings.get_player_name(id));

	view_manager.draw(renderer);
}

void PlayerSubView::lose_focus(){
	view_manager.lose_focus();
	
	set_color_menu_open(false);
}

void PlayerSubView::step(){	
	view_manager.step();
}

void PlayerSubView::remove(){
	settings.remove_player(id);
}

int PlayerSubView::get_id() const{
	return id;
}

int PlayerSubView::get_index() const{
	return settings.get_index(id);
}

const string& PlayerSubView::get_name() const{
	return settings.get_player_name(id);
}

int PlayerSubView::get_color() const{
	return settings.get_player_color(id);
}

const KeySet& PlayerSubView::get_keys() const {
	return key_manager.get_keys(keys_id);
}

void PlayerSubView::set_name(const string& new_name){
	settings.set_player_name(id, new_name);
}

void PlayerSubView::set_color(int color){
	settings.set_player_color(id, color);
	
	set_color_menu_open(false);
}

void PlayerSubView::set_keys(const KeySet& keys){
	key_manager.set_keys(keys_id, keys);
}

const PlayerSettings& PlayerSubView::get_settings() const {
	return settings;
}

void PlayerSubView::set_color_menu_open(bool open){
	if(open && !showing_color_menu) view_manager.add_view(color_menu.get());
	if(!open && showing_color_menu) view_manager.remove_view(color_menu.get());
	
	showing_color_menu = open;
}


#define ADD_CROSS_SIZE_RATIO 0.5
#define ADD_CROSS_RATIO (1.0/6.0)

PlayerAddButton::PlayerAddButton(const SDL_Rect& rect, PlayersSubView& owner) :
	Button(rect, false),
	owner(owner),
	cross(nullptr) {}
	
void PlayerAddButton::init(SDL_Renderer* renderer){
	cross = make_cross(
		renderer,
		get_rect().h * ADD_CROSS_SIZE_RATIO,
		ADD_CROSS_RATIO
	);
}

void PlayerAddButton::draw_button(SDL_Renderer* renderer, SDL_Color color){
	fill_back(renderer, color);
	draw_frame(renderer, line_color);
	
	SDL_Rect rect = get_rect();
	
	int size = rect.h * ADD_CROSS_SIZE_RATIO;
	rect.x = (rect.w - size) / 2;
	rect.y = (rect.h - size) / 2;
	rect.w = rect.h = size;
	
	SDL_RenderCopy(renderer, cross->get(), NULL, &rect);
}

void PlayerAddButton::draw_pressed(SDL_Renderer* renderer){
	if(cross.get() == nullptr) init(renderer);
	SDL_SetTextureColorMod(cross->get(), 0, 128, 0);
	draw_button(renderer, active_color);
}

void PlayerAddButton::draw_released(SDL_Renderer* renderer){
	if(cross.get() == nullptr) init(renderer);
	SDL_SetTextureColorMod(cross->get(), 0, 128, 0);
	draw_button(renderer, bg_color);
}

void PlayerAddButton::draw_inactive(SDL_Renderer* renderer){
	if(cross.get() == nullptr) init(renderer);
	SDL_SetTextureColorMod(cross->get(), 64, 128, 64);
	draw_button(renderer, bg_color);
}

void PlayerAddButton::on_pressed(){
	play(Sound::CLICK);
	owner.add_player();
}

#define MARGIN 0.05
#define BUTTON_HEIGHT 0.2

PlayersSubView::PlayersSubView(const SDL_Rect& rect, TextCompleter& name_completer, KeySetManager& key_manager) :
	SubView(rect, false),
	name_completer(name_completer),
	key_manager(key_manager){
	SDL_Rect button_rect;
	button_rect.y = button_rect.x = rect.w * MARGIN;
	button_rect.w = rect.w - (button_rect.x * 2);
	button_rect.h = (rect.h * BUTTON_HEIGHT) - (button_rect.x * 2);
	
	add_button = make_unique<PlayerAddButton>(button_rect, *this);
	view_manager.add_view(add_button.get());
}

PlayersSubView::~PlayersSubView(){
	while(views.size()) views.pop_back();
}

void PlayersSubView::reorganize_views(){
	vector<int> ids = settings.get_ids();
	
	map<int, int> index_by_id;
	for(int i = 0; i < views.size(); i++){
		index_by_id[views[i]->get_id()] = i;
	}
	
	auto old_views = std::move(views);
	views.clear();
	
	for(auto id: ids){
		if(index_by_id.count(id)){
			views.push_back(std::move(old_views[index_by_id[id]]));
			old_views[index_by_id[id]] = nullptr;
		}
		else{
			views.push_back(nullptr);
		}
	}
	
	for(int i = old_views.size() - 1; i >= 0; i--){
		if(old_views[i].get() != nullptr) view_manager.remove_view(old_views[i].get());
		old_views[i] = nullptr;
	}

	old_views.clear();

	int margin = get_rect().w * MARGIN;
	int dy = (get_rect().h * BUTTON_HEIGHT) - margin;

	SDL_Rect rect;
	rect.y = rect.x = margin;
	rect.w = get_rect().w - (margin * 2);
	rect.h = (get_rect().h * BUTTON_HEIGHT) - (margin * 2);

	for(int i = 0; i < ids.size(); i++, rect.y += dy){
		if(views[i].get() == nullptr){
			views[i] = make_unique<PlayerSubView>(
				rect,
				ids[i], settings,
				name_completer,
				key_manager
			);
			view_manager.add_view(views[i].get());
		}
	}
}

bool PlayersSubView::on_event(const SDL_Event& event){
	return view_manager.handle_event(event);
}

void PlayersSubView::draw_content(SDL_Renderer* renderer){
	reorganize_views();

	fill_back(renderer, bg_color);
	draw_frame(renderer, line_color);
	
	view_manager.draw(renderer);
}

void PlayersSubView::lose_focus(){
	view_manager.lose_focus();
}

#define VIEW_FOLLOW_SPEED 5

void PlayersSubView::step(){
	reorganize_views();
	
	view_manager.step();
	
	int margin = get_rect().w * MARGIN;
	int dy = (get_rect().h * BUTTON_HEIGHT) - margin;
	
	int target_y = margin;
	for(int i = 0; i < views.size(); i++, target_y += dy){
		int x = views[i]->get_rect().x;
		int y = views[i]->get_rect().y;
		
		views[i]->move(x, follow(y, target_y, VIEW_FOLLOW_SPEED));
	}
	
	int x = add_button->get_rect().x;
	int y = add_button->get_rect().y;
	add_button->move(x, follow(y, target_y, VIEW_FOLLOW_SPEED));
}

void PlayersSubView::attach_settings(GameSettingsView* view, GameSettingsManipulator* manipulator){
	settings.attach(view, manipulator);
}

void PlayersSubView::detach_settings(){
	settings.detach();
}

void PlayersSubView::add_player(){
	settings.add_player();
}

vector<KeySet> PlayersSubView::get_keys() const {
	vector<KeySet> keys;
	
	for(auto& view: views){
		keys.push_back(view->get_keys());
	}
	
	return keys;
}
