#include "player_texture.h"

PlayerTexture::PlayerTexture(const SDL_Color& color) :
	color(color),
	using_texture(false),
	length(1) {}

PlayerTexture::PlayerTexture(TextureId image, int length) :
	color({255, 255, 255, 255}),
	using_texture(true),
	image(image),
	length(length) {}

const SDL_Color& PlayerTexture::get_color() const {
	return color;
}

SDL_Texture* PlayerTexture::get_texture() const {
	return using_texture ? get_texture_by_id(image) : NULL;
}

int PlayerTexture::get_length() const {
	return length;
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
	PlayerTexture({64, 160, 64, 255}),  // Dark Green
	PlayerTexture({64, 64, 160, 255}),  // Dark Blue
	PlayerTexture({160, 64, 64, 255}),  // Dark Red
	PlayerTexture({255, 255, 255, 255}),  // White
	PlayerTexture({160, 160, 160, 255}),  // Gray
	PlayerTexture({255, 160, 160, 255}),  // Pink
	PlayerTexture({136, 112, 64, 255}),  // Brown
	PlayerTexture(TextureId::RAINBOW, 90),
});
