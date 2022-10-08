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
	PlayerTexture({0, 255, 0, 255}),  // Green
	PlayerTexture({255, 0, 0, 255}),  // Red
	PlayerTexture({0, 0, 255, 255}),  // Blue
	PlayerTexture({255, 255, 0, 255}),  // Yellow
	PlayerTexture({0, 255, 255, 255}),  // Cyan
	PlayerTexture({255, 0, 255, 255}),  // Magenta
	PlayerTexture({255, 128, 0, 255}),  // Orange
	PlayerTexture({128, 0, 255, 255}),  // Purple
	PlayerTexture({0, 128, 0, 255}),  // Dark Green
	PlayerTexture({0, 0, 128, 255}),  // Dark Blue
	PlayerTexture({255, 255, 255, 255}),  // White
	PlayerTexture({128, 128, 128, 255}),  // Gray
	PlayerTexture({255, 128, 128, 255}),  // Pink
	PlayerTexture({96, 64, 0, 255}),  // Brown
	PlayerTexture(TextureId::RAINBOW, 90),
});
