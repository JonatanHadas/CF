#include "game_drawer.h"

#include "colors.h"
#include "player_texture.h"
#include "texts.h"
#include "images.h"

#include "../utils/geometry.h"
#include "../game/game_logic.h"
#include "../game/powerups.h"

#include "powerup_images.h"

#include <algorithm>

BoardDrawer::BoardDrawer(GameView* view, const GameSettings& settings, const map<int, KeySet>& keysets) :
	board(view->get_board_size()),
	view(view),
	settings(settings),
	keysets(keysets),
	texture(nullptr) {}

#define DRAW_SCALE 8.0

void BoardDrawer::init(SDL_Renderer* renderer){
	if(nullptr == texture.get()){
		texture = make_unique<Texture>(renderer,
			SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET,
			board.w * DRAW_SCALE, board.h * DRAW_SCALE
		);
	}
}

#define CIRCLE_RAD 50

#define ARROW_SIZE (DRAW_SCALE * 12.)
#define ARROW_CENTER_X (ARROW_SIZE * -0.1);

#define ARROW_CANVAS_SIZE 256
#define ARROW_TEXT_Y 80
#define ARROW_TEXT_X 8


#define CURVE_WIDTH_RATIO 1.2

#define RING_RATIO (2.0 * CURVE_WIDTH_RATIO)
#define RING_WIDTH 0.4
#define RING_INTERVAL 0.8
#define RING_PART 0.01

#define CIRCLE_PARTS 100

void BoardDrawer::draw(SDL_Renderer* renderer, bool paused){
	init(renderer);

	Texture circle_texture(renderer,
		SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET,
		CIRCLE_RAD * 2, CIRCLE_RAD * 2
	);
	Texture textured_circle(renderer,
		SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET,
		CIRCLE_RAD * 2, CIRCLE_RAD * 2
	);
	
	Texture textured_arrows(renderer,
		SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET,
		ARROW_CANVAS_SIZE, ARROW_CANVAS_SIZE
	);
	Texture arrows(renderer,
		SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET,
		ARROW_CANVAS_SIZE, ARROW_CANVAS_SIZE
	);

	circle_texture.do_with_texture(renderer, [&](){
		SDL_SetRenderDrawColor(renderer, 255, 255, 255, 0);
		SDL_RenderClear(renderer);

		SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
		draw_circle(renderer, CIRCLE_RAD, CIRCLE_RAD, CIRCLE_RAD);
	});
	SDL_SetTextureBlendMode(circle_texture.get(), SDL_BLENDMODE_BLEND);
	SDL_SetTextureBlendMode(textured_circle.get(), SDL_BLENDMODE_BLEND);

	SDL_SetTextureBlendMode(textured_arrows.get(), SDL_BLENDMODE_BLEND);
	SDL_SetTextureBlendMode(arrows.get(), SDL_BLENDMODE_NONE);
	SDL_SetTextureBlendMode(get_img(Img::ARROWS), SDL_BLENDMODE_NONE);

	SDL_Rect dst;
	
	auto texture_circle = [&](const PlayerTexture& player_texture, int counter){
		textured_circle.do_with_texture(renderer, [&](){
			if(player_texture.get_texture() == NULL) SDL_SetTextureColorMod(
				circle_texture.get(),
				player_texture.get_color().r,
				player_texture.get_color().g,
				player_texture.get_color().b
			);
			else SDL_SetTextureColorMod(circle_texture.get(), 255, 255, 255);

			SDL_RenderCopy(renderer, circle_texture.get(), NULL, NULL);
			
			if(player_texture.get_texture() != NULL){
				SDL_Vertex vertices[4] = {
					{
						.position = {
							.x = 0.0f,
							.y = 0.0f,
						},
						.color = player_texture.get_color(),
						.tex_coord = {
							.x = 1.0f * counter / player_texture.get_length(),
							.y = 1.0f,
						},
					},
					{
						.position = {
							.x = 0.0f,
							.y = CIRCLE_RAD * 2.0f,
						},
						.color = player_texture.get_color(),
						.tex_coord = {
							.x = 1.0f * counter / player_texture.get_length(),
							.y = 0.0f,
						},
					},
					{
						.position = {
							.x = CIRCLE_RAD * 2.0f,
							.y = 0.0f,
						},
						.color = player_texture.get_color(),
						.tex_coord = {
							.x = 1.0f * (counter + 1) / player_texture.get_length(),
							.y = 1.0f,
						},
					},
					{
						.position = {
							.x = CIRCLE_RAD * 2.0f,
							.y = CIRCLE_RAD * 2.0f,
						},
						.color = player_texture.get_color(),
						.tex_coord = {
							.x = 1.0f * (counter + 1) / player_texture.get_length(),
							.y = 0.0f,
						},
					},
				};
				
				int indices[6] = {0, 1, 2, 1, 2, 3};
				
				SDL_RenderGeometry(renderer, player_texture.get_texture(), vertices, 4, indices, 6);
			}
		});
	};
	
	auto circular_texture = [&](const PlayerTexture& player_texture){
		textured_circle.do_with_texture(renderer, [&](){
			if(player_texture.get_texture() == NULL) SDL_SetTextureColorMod(
				circle_texture.get(),
				player_texture.get_color().r,
				player_texture.get_color().g,
				player_texture.get_color().b
			);
			else SDL_SetTextureColorMod(circle_texture.get(), 255, 255, 255);

			SDL_RenderCopy(renderer, circle_texture.get(), NULL, NULL);
			
			if(player_texture.get_texture() != NULL){
				vector<SDL_Vertex> vertices;
				vector<int> indices;

				vertices.push_back({
					.position = {
						.x = 2.01f * CIRCLE_RAD,
						.y = 1.0f * CIRCLE_RAD,
					},
					.color = player_texture.get_color(),
					.tex_coord = {
						.x = 0.0f,
						.y = 0.0f,
					},
				});

				for(int i = 1; i <= CIRCLE_PARTS; i++){
					double angle = M_PI * 2 * i / CIRCLE_PARTS;
					
					indices.push_back(vertices.size() + 1);
					indices.push_back(vertices.size());
					indices.push_back(vertices.size() - 1);
					vertices.push_back({
						.position = {
							.x = 1.0f * CIRCLE_RAD,
							.y = 1.0f * CIRCLE_RAD,
						},
						.color = player_texture.get_color(),
						.tex_coord = {
							.x = 1.0f * (i - 0.5f) / CIRCLE_PARTS,
							.y = 1.0f,
						},
					});
					vertices.push_back({
						.position = {
							.x = (float)(1.0 + 1.01 * cos(angle)) * CIRCLE_RAD,
							.y = (float)(1.0 + 1.01 * sin(angle)) * CIRCLE_RAD,
						},
						.color = player_texture.get_color(),
						.tex_coord = {
							.x = 1.0f * i / CIRCLE_PARTS,
							.y = 0.0f,
						},
					});
				}
				
				if(!player_texture.is_lateral()){
					for(auto& vertex: vertices){
						swap(vertex.tex_coord.x, vertex.tex_coord.y);
					}
				}
				
				SDL_RenderGeometry(renderer, player_texture.get_texture(), vertices.data(), vertices.size(), indices.data(), indices.size());
			}
		});
	};

	texture->do_with_texture(renderer, [&](){
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
		SDL_RenderClear(renderer);

		for(auto entry: view->get_powerups()){
			SDL_Color color = powerup_colors[(int)entry.second.desc.affects];
			SDL_SetTextureColorMod(circle_texture.get(),
				color.r,
				color.g,
				color.b
			);
			dst.x = DRAW_SCALE * entry.second.x;
			dst.y = DRAW_SCALE * entry.second.y;			
			dst.w = dst.h = 2 * DRAW_SCALE * POWERUP_RADIUS;
			dst.x -= dst.w/2; dst.y -= dst.h/2;

			SDL_RenderCopy(renderer, circle_texture.get(), NULL, &dst);
			SDL_RenderCopy(renderer, get_img(powerup_images[entry.second.desc.type]), NULL, &dst);			
		}

		SDL_SetTextureBlendMode(circle_texture.get(), SDL_BLENDMODE_NONE);
		SDL_SetTextureColorMod(circle_texture.get(), 255, 255, 255);

		for(int i = 0; i < view->get_histories().size(); i++){
			auto player_history = view->get_histories()[i];
			const auto& player_state = view->get_states()[i];
			const auto& player_texture = player_textures[settings.colors[i]];
			
			if(player_history.empty()) continue;

			
			SDL_SetTextureBlendMode(player_texture.get_texture(), SDL_BLENDMODE_MOD);
			
			vector<SDL_Vertex> vertices;
			vector<int> indices;
			SDL_Vertex vertex = {
				.color = player_texture.get_color(),
			};

			for(int j = 1; j < player_history.size(); j++){
				int counter = (j + player_state.counter - player_history.size()) % player_texture.get_length();
				
				if(player_history[j].hovering) continue;

				double width = get_player_size(player_history[j].size) * CURVE_WIDTH_RATIO;
				double prev_width = min(width, get_player_size(player_history[j].size) * CURVE_WIDTH_RATIO);

				double prev_x = player_history[j-1].x;
				if(player_history[j].warping_x) prev_x -= board.w * player_history[j].warp_x;
				double prev_y = player_history[j-1].y;
				if(player_history[j].warping_y) prev_y -= board.h * player_history[j].warp_y;

				double prev_direction = player_history[j - 1].direction;
				if(player_history[j].corner) {
					double direction = (prev_direction + player_history[j].direction) / 2;
					
					prev_direction = player_history[j].direction;
					
					texture_circle(player_texture, counter);

					// Draw circle
					dst.w = dst.h = DRAW_SCALE * prev_width;
					dst.x = DRAW_SCALE * prev_x - dst.w / 2.0;
					dst.y = DRAW_SCALE * prev_y - dst.h / 2.0;
					SDL_RenderCopyEx(renderer, textured_circle.get(), NULL, &dst, RAD2DEG(prev_direction), NULL, SDL_FLIP_NONE);
				}

				for(int warp_x = min(player_history[j].warp_x, 0); warp_x <= max(player_history[j].warp_x, 0); warp_x += 1){
					for(int warp_y = min(player_history[j].warp_y, 0); warp_y <= max(player_history[j].warp_y, 0); warp_y += 1){
						int index = vertices.size();
						vertex.position = {
							.x = (float)(DRAW_SCALE * (prev_x - sin(prev_direction)*(width/2) + warp_x * board.w)),
							.y = (float)(DRAW_SCALE * (prev_y + cos(prev_direction)*(width/2) + warp_y * board.h)),
						};
						vertex.tex_coord = {
							.x = 1.0f * counter / player_texture.get_length(),
							.y = 0.0f,
						};
						vertices.push_back(vertex);
						vertex.position = {
							.x = (float)(DRAW_SCALE * (prev_x + sin(prev_direction)*(width/2) + warp_x * board.w)),
							.y = (float)(DRAW_SCALE * (prev_y - cos(prev_direction)*(width/2) + warp_y * board.h)),
						};
						vertex.tex_coord = {
							.x = 1.0f * counter / player_texture.get_length(),
							.y = 1.0f,
						};
						vertices.push_back(vertex);
						vertex.position = {
							.x = (float)(DRAW_SCALE * (player_history[j].x - sin(player_history[j].direction)*(width/2) + warp_x * board.w)),
							.y = (float)(DRAW_SCALE * (player_history[j].y + cos(player_history[j].direction)*(width/2) + warp_y * board.h)),
						};
						vertex.tex_coord = {
							.x = 1.0f * (counter + 1) / player_texture.get_length(),
							.y = 0.0f,
						};
						vertices.push_back(vertex);
						vertex.position = {
							.x = (float)(DRAW_SCALE * (player_history[j].x + sin(player_history[j].direction)*(width/2) + warp_x * board.w)),
							.y = (float)(DRAW_SCALE * (player_history[j].y - cos(player_history[j].direction)*(width/2) + warp_y * board.h)),
						};
						vertex.tex_coord = {
							.x = 1.0f * (counter + 1) / player_texture.get_length(),
							.y = 1.0f,
						};
						vertices.push_back(vertex);

						indices.push_back(index);
						indices.push_back(index+1);
						indices.push_back(index+2);
						indices.push_back(index+1);
						indices.push_back(index+2);
						indices.push_back(index+3);
					}
				}
			}

			SDL_SetTextureBlendMode(player_texture.get_texture(), SDL_BLENDMODE_BLEND);
			SDL_RenderGeometry(renderer, player_texture.get_texture(), vertices.data(), vertices.size(), indices.data(), indices.size());

			SDL_SetTextureBlendMode(player_texture.get_texture(), SDL_BLENDMODE_MOD);
			circular_texture(player_texture);
			
			if(paused && keysets.count(i)){
				arrows.do_with_texture(renderer, [&](){
					SDL_RenderCopy(renderer, get_img(Img::ARROWS), NULL, NULL);
					
					Msg(
						SDL_GetScancodeName(keysets.at(i).left),
						{255, 255, 255, 255},
						FontType::MID,
						renderer
					).render_centered(
						ARROW_TEXT_X,
						ARROW_TEXT_Y,
						Align::LEFT
					);

					Msg(
						SDL_GetScancodeName(keysets.at(i).right),
						{255, 255, 255, 255},
						FontType::MID,
						renderer
					).render_centered(
						ARROW_CANVAS_SIZE - ARROW_TEXT_X,
						ARROW_TEXT_Y,
						Align::RIGHT
					);
				});
				
				textured_arrows.do_with_texture(renderer, [&](){
					SDL_RenderCopyEx(renderer, arrows.get(), NULL, NULL, 90, NULL, SDL_FLIP_NONE);
					
					if(player_texture.get_texture() != NULL){
						SDL_RenderCopy(renderer, player_texture.get_texture(), NULL, NULL);
					}
				});
				
				SDL_SetTextureColorMod(textured_arrows.get(),
					player_texture.get_color().r,
					player_texture.get_color().g,
					player_texture.get_color().b
				);
				
				dst.w = dst.h = ARROW_SIZE;
				dst.y = DRAW_SCALE * player_history.back().y - dst.h / 2;
				dst.x = DRAW_SCALE * player_history.back().x - ARROW_CENTER_X;
				
				SDL_Point center;
				center.x = ARROW_CENTER_X;
				center.y = dst.h / 2;
				
				SDL_RenderCopyEx(renderer, textured_arrows.get(), NULL, &dst, RAD2DEG(player_history.back().direction), &center, SDL_FLIP_NONE);
			}

			double width = get_player_size(player_history.back().size) * CURVE_WIDTH_RATIO;
			dst.w = dst.h = DRAW_SCALE * width;
			dst.x = DRAW_SCALE * player_history.back().x - dst.w / 2.0;
			dst.y = DRAW_SCALE * player_history.back().y - dst.h / 2.0;
			
			SDL_RenderCopy(renderer, textured_circle.get(), NULL, &dst);
		}

		vector<SDL_Vertex> vertices;
		vector<int> indices;
		SDL_Vertex vertex;
		for(int player = 0; player < view->get_histories().size(); player++){
			if(view->get_histories()[player].empty()) continue;
			if(!view->get_histories()[player].back().alive) continue;
			double radius = RING_RATIO * get_player_size(view->get_histories()[player].back().size);
			double x = view->get_histories()[player].back().x, y = view->get_histories()[player].back().y;
			applying_to_player(view->get_powerup_effects(), player, settings.teams, [&](const PowerUpEffect& effect){
				double angle = M_PI * 2 * effect.timer / powerup_times[effect.desc];
				int parts = angle / RING_PART;
				vertex.color = powerup_ring_colors[(int)effect.desc.affects];
				for(int i = 0; i < parts; i++){
					if(i){
						indices.push_back(vertices.size()-2);
						indices.push_back(vertices.size()-1);
						indices.push_back(vertices.size());
						indices.push_back(vertices.size()-1);
						indices.push_back(vertices.size());
						indices.push_back(vertices.size()+1);
					}

					vertex.position = {
						.x = (float)(DRAW_SCALE * (x + sin(angle*i/parts)*(radius - RING_WIDTH/2))),
						.y = (float)(DRAW_SCALE * (y - cos(angle*i/parts)*(radius - RING_WIDTH/2))),
					};
					vertices.push_back(vertex);
					vertex.position = {
						.x = (float)(DRAW_SCALE * (x + sin(angle*i/parts)*(radius + RING_WIDTH/2))),
						.y = (float)(DRAW_SCALE * (y - cos(angle*i/parts)*(radius + RING_WIDTH/2))),
					};
					vertices.push_back(vertex);
				}
				radius += RING_INTERVAL;
			});
		}

		SDL_RenderGeometry(renderer, NULL, vertices.data(), vertices.size(), indices.data(), indices.size());
	});
}

Texture& BoardDrawer::get_texture() const{
	return *texture;
}


#define TITLE_Y 0.05
#define SCORES_Y 0.10
#define TEXT_X 0.05
#define PLAYER_NAME_X 0.07
#define SCORES_X 0.98
#define SCORES_DY 0.05


ScoreDrawer::ScoreDrawer(
	GameView* view,
	const GameSettings& settings,
	int x, int w, int h
) : view(view), settings(settings), x(x), w(w), h(h),
	tie_break(false), amount(-1) {}
	

string get_default_name(const char* type, int index){
	char name[50];
	snprintf(name, 50, "%s-%d", type, index);
	return name;
}

#define INACTIVE_MULTIPLIER 0.2

void ScoreDrawer::init(SDL_Renderer* renderer){
	for(int i = 0; i < settings.teams.size(); i++){
		const auto& player_texture = player_textures[settings.colors[i]];
		SDL_Color color = player_texture.get_color();
		const char* name = (settings.names[i].size() ? settings.names[i] : get_default_name("player", i)).c_str();
		
		names.push_back(Msg(
			name,
			color, FontType::NRM,
			renderer,
			player_texture.get_texture()
		));

		color.a *= INACTIVE_MULTIPLIER;
		names_inactive.push_back(Msg(
			name,
			color, FontType::NRM,
			renderer,
			player_texture.get_texture()
		));
	}
	
	if(settings.using_teams){
		for(int i = 0; i < settings.team_names.size(); i++){
			team_names.push_back(Msg(
				(settings.team_names[i].size() ? settings.team_names[i] : get_default_name("team", i)).c_str(),
				text_color,
				FontType::NRM,
				renderer
			));
		}

		player_indices = vector<vector<int>>(settings.team_names.size());

		for(int i = 0; i < settings.teams.size(); i++){
			const auto& player_texture = player_textures[settings.colors[i]];
			
			player_indices[settings.teams[i]].push_back(i);
		}
	}
	else{
		player_indices = vector<vector<int>>(settings.teams.size());
	}

	for(int i = 0; i < player_indices.size(); i++) order.push_back(i);

	int dy = h * SCORES_DY;
	int y = h * SCORES_Y;
	for(int i: order){
		ys.push_back(y);
		y += dy * (1 + player_indices[i].size());
	}

}

void ScoreDrawer::draw(SDL_Renderer* renderer){
	if(!names.size()) init(renderer);

	for(int i = 0; i < view->get_scores().size(); i++){
		if(scores.size() <= i || view->get_scores()[i] != scores[i]){
			const char* text = to_string(view->get_scores()[i]).c_str();
			SDL_Color color = settings.using_teams ? text_color : player_textures[settings.colors[i]].get_color();
			SDL_Texture* texture = settings.using_teams ? NULL :  player_textures[settings.colors[i]].get_texture();
			
			Msg msg(
				text,
				color, FontType::NRM,
				renderer,
				texture
			);
			
			color.a *= INACTIVE_MULTIPLIER;
			Msg msg_inactive(
				text,
				color, FontType::NRM,
				renderer,
				texture
			);

			if(scores.size() <= i){
				scores.push_back(view->get_scores()[i]);
				score_texts.push_back(std::move(msg));
				scores_inactive.push_back(std::move(msg_inactive));
			}
			else{
				scores[i] = view->get_scores()[i];
				score_texts[i] = std::move(msg);
				scores_inactive[i] = std::move(msg_inactive);
			}
		}
	}

	int dy = h * SCORES_DY;

	for(int i = 0; i < scores.size(); i++){
		int y = ys[i];
		
		auto& msgs = settings.using_teams ? team_names : (
			view->get_states()[i].active ? names : names_inactive
		);
		
		msgs[i].render_centered(x + w * TEXT_X, y, Align::LEFT);
		(settings.using_teams || view->get_states()[i].active ? score_texts : scores_inactive)[i].render_centered(
			x + w * SCORES_X, y, Align::RIGHT
		);

		for(auto j: player_indices[i]){
			y += dy;
			(view->get_states()[j].active ? names : names_inactive)[j].render_centered(
				x + w * PLAYER_NAME_X, y, Align::LEFT
			);
		}
	}

	if(settings.scores.criterion != WinCriterion::NEVER){
		int max_score, second_score;
		for(auto score: scores){
			if(score > max_score){
				second_score = max_score;
				max_score = score;
			}
			else if(score > second_score){
				second_score = score;
			}
		}

		if(!tie_break && view->is_tie_break()){
			tie_break = true;
			title = make_unique<Msg>(
				"Tie Break:",
				text_color,
				FontType::NRM,
				renderer
			);
			threshold = nullptr;

			amount = -1;
		}

		if(tie_break){
			int new_amount = second_score + settings.scores.tie_break_threshold;
			if(new_amount != amount || threshold.get() == nullptr){
				amount = new_amount;
				threshold = make_unique<Msg>(
					to_string(amount).c_str(),
					text_color,
					FontType::NRM,
					renderer
				);
			}
		}
		else{
			switch(settings.scores.criterion){
			case WinCriterion::BY_SCORE:
				if(amount != settings.scores.amount || threshold.get() == nullptr){
					amount = settings.scores.amount;
					threshold = make_unique<Msg>(
						to_string(amount).c_str(),
						text_color,
						FontType::NRM,
						renderer
					);
				}
				if(title.get() == nullptr){
					title = make_unique<Msg>(
						"First to reach:",
						text_color,
						FontType::NRM,
						renderer
					);
				}
				break;
			case WinCriterion::BY_ROUND:
				if(amount != view->get_round() || threshold.get() == nullptr){
					amount = view->get_round();
					char text[50];
					snprintf(text, 50, "%d/%d", view->get_round() + 1, settings.scores.amount);
					threshold = make_unique<Msg>(
						text,
						text_color,
						FontType::NRM,
						renderer
					);
				}
				if(title.get() == nullptr){
					title = make_unique<Msg>(
						"Round:",
						text_color,
						FontType::NRM,
						renderer
					);
				}
				break;
			}
		}

		title->render_centered(x + w * TEXT_X, h * TITLE_Y, Align::LEFT);
		threshold->render_centered(x + w * SCORES_X, h * TITLE_Y, Align::RIGHT);
	}
}

#define SCORE_FOLLOW_SPEED 3

void ScoreDrawer::step(){
	sort(order.begin(), order.end(), [&](int first, int second){
		return scores[first] > scores[second];
	});

	int dy = h * SCORES_DY;
	int y = h * SCORES_Y;
	for(int i: order){
		ys[i] = follow(ys[i], y, SCORE_FOLLOW_SPEED);
		y += dy * (1 + player_indices[i].size());
	}
}


#define WINNER_MARGIN 0.02

WinnerDrawer::WinnerDrawer(
	GameView* view,
	const GameSettings& settings,
	int y, int h, int w, int x
) : view(view), settings(settings),
	y(y), h(h), w(w), x(x + WINNER_MARGIN * w) {}

void WinnerDrawer::init(SDL_Renderer* renderer){
	if(texture.get() == nullptr){
		texture = make_unique<Texture>(renderer,
			SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET,
			w, h
		);
		
		SDL_SetTextureBlendMode(texture->get(), SDL_BLENDMODE_BLEND);
	}
}

vector<string> WinnerDrawer::get_names(){
	vector<string> names;
	for(auto winner: winners){
		string name = settings.using_teams ? settings.team_names[winner] : settings.names[winner];
		if(!name.size()) name = get_default_name(settings.using_teams ? "team" : "player", winner);
		
		names.push_back(name);
	}
	
	return names;
}

void WinnerDrawer::draw_msg(SDL_Renderer* renderer){
	vector<int> new_winners = view->get_round_winners();
	
	if(msg.get() != nullptr && new_winners.size() == winners.size()){
		bool same = true;
		for(int i = 0; i < winners.size(); i++){
			if(winners[i] != new_winners[i]){
				same = false;
				break;
			}		
		}
		if(same) return;
	}
	
	winners = new_winners;
	auto names = get_names();
	string text = "";
	SDL_Color color = text_color;
	SDL_Texture* text_texture = NULL;
	if(winners.size() == 1){
		text = settings.using_teams ? "Team " : "";
		text += names[0];
		if(!settings.using_teams){
			const auto& player_texture = player_textures[settings.colors[winners[0]]];
			color = player_texture.get_color();
			text_texture = player_texture.get_texture();
		}
		
		text += " wins this round";
	}
	else{
		text = settings.using_teams ? "Teams " : "";
		for(int i = 0; i < winners.size(); i++){
			if(i == winners.size() - 1) text += " and ";
			else if(i > 0) text += ", ";
			
			text += names[i];
		}
		
		text += " win this round";
	}
	
	msg = make_unique<Msg>(
		text.c_str(),
		color,
		FontType::NRM,
		renderer,
		text_texture
	);
	
	texture->do_with_texture(renderer, [&](){
		SDL_SetRenderDrawColor(renderer, bg_color.r, bg_color.g, bg_color.b, 128);
		SDL_RenderClear(renderer);
		
		msg->render_centered(x , h / 2, Align::LEFT);
	});
}

void WinnerDrawer::draw(SDL_Renderer* renderer){
	init(renderer);
	
	if(view->is_round_over()){
		draw_msg(renderer);
		
		SDL_Rect rect;
		
		rect.x = 0;
		rect.y = y;
		rect.w = w;
		rect.h = h;
		SDL_RenderCopy(renderer, texture->get(), NULL, &rect);
	}
}

void WinnerDrawer::step(){
	
}


#define WINNER_DRAWER_H 0.03

#define BOARD_SCALE 

GameDrawer::GameDrawer(GameView* view, const GameSettings& settings, const map<int, KeySet>& keysets) :
	view(view),
	settings(settings),
	board(view->get_board_size()),
	board_drawer(view, settings, keysets),
	is_initialized(false) {}

void GameDrawer::init(SDL_Renderer* renderer){
	if(!is_initialized){
		SDL_GetRendererOutputSize(renderer, &screen_width, &screen_height);

		SDL_RenderSetLogicalSize(renderer, screen_width, screen_height);

		is_initialized = true;

		score_drawer = make_unique<ScoreDrawer>(
			view,
			settings,
			screen_width / 2, screen_width / 2, screen_height
		);
		
		double scale = min(screen_width * 0.00475, screen_height * 0.0075);
		
		board_rect.w = scale * board.w;
		board_rect.h = scale * board.h;
		
		board_rect.x = (screen_width / 2 - board_rect.w) / 2;
		board_rect.y = (screen_height - board_rect.h) / 2;

		int h = screen_height * WINNER_DRAWER_H;
		winner_drawer = make_unique<WinnerDrawer>(
			view,
			settings,
			(screen_height - h)/2, h, screen_width/2, board_rect.x - 1
		);
		
	}
}

#define X_MARGIN 0.025
#define Y_MARGIN 0.05

void GameDrawer::draw(SDL_Renderer* renderer, bool paused){
	init(renderer);

	SDL_SetRenderDrawColor(renderer, 32, 32, 32, 0);
	SDL_RenderClear(renderer);
	
	board_drawer.draw(renderer, paused);
	score_drawer->draw(renderer);

    SDL_Rect frame;
    frame.x = board_rect.x - 1;
    frame.y = board_rect.y - 1;
    frame.w = board_rect.w + 2;
    frame.h = board_rect.h + 2;

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderFillRect(renderer, &frame);

    SDL_RenderCopy(renderer, board_drawer.get_texture().get(), NULL, &board_rect);

	winner_drawer->draw(renderer);
}

void GameDrawer::step(){
	if(score_drawer.get() != nullptr) score_drawer->step();
	if(winner_drawer.get() != nullptr) winner_drawer->step();
}
