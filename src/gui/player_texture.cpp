#include "player_texture.h"

PlayerTexture::PlayerTexture(const SDL_Color& color) :
	color(color),
	using_texture(false),
	length(1),
	lateral(false) {}

PlayerTexture::PlayerTexture(TextureId image, int length, bool lateral) :
	color({255, 255, 255, 255}),
	using_texture(true),
	image(image),
	length(length),
	lateral(lateral) {}

const SDL_Color& PlayerTexture::get_color() const {
	return color;
}

SDL_Texture* PlayerTexture::get_texture() const {
	return using_texture ? get_texture_by_id(image) : NULL;
}

int PlayerTexture::get_length() const {
	return length;
}

bool PlayerTexture::is_lateral() const {
	return lateral;
}

vector<PlayerTexture> player_textures({
	PlayerTexture({64, 255, 64, 255}),  // Green
	PlayerTexture({255, 64, 64, 255}),  // Red
	PlayerTexture({64, 64, 255, 255}),  // Blue
	PlayerTexture({255, 255, 64, 255}),  // Yellow
	PlayerTexture({64, 255, 255, 255}),  // Cyan
	PlayerTexture({255, 64, 255, 255}),  // Magenta
	PlayerTexture({255, 160, 64, 255}),  // Orange
	PlayerTexture({160, 64, 255, 255}),  // Purple
	PlayerTexture({0, 160, 0, 255}),  // Dark Green
	PlayerTexture({0, 0, 160, 255}),  // Dark Blue
	PlayerTexture({160, 0, 0, 255}),  // Dark Red
	PlayerTexture({255, 255, 255, 255}),  // White
	PlayerTexture({160, 160, 160, 255}),  // Gray
	PlayerTexture({255, 160, 160, 255}),  // Pink
	PlayerTexture({136, 112, 64, 255}),  // Brown
	PlayerTexture(TextureId::RAINBOW, 90),
	PlayerTexture(TextureId::RED_STRIPES, 1, true),
	PlayerTexture(TextureId::TRAFFIC_LIGHT, 45),
});
