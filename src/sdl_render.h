#pragma once
#include "entt/fwd.hpp"
#include "vmath.h"
#include "SDL_rect.h"
#include <string>

constexpr int WINDOW_WIDTH = 640;
constexpr int WINDOW_HEIGHT = 800;

constexpr Vec2i UP{ 0,-1 };
constexpr Vec2i RIGHT{ 1,0 };

constexpr Vec2i coordinate_center{ WINDOW_WIDTH / 2,WINDOW_HEIGHT - 100 };

struct SDL_Renderer;
struct SDL_Window;
struct SDL_Texture;


struct SDL_RenderSprite {

	Vec2i location;
	int width;
	int height;

	SDL_Rect texture_rect;
	SDL_Texture* texture{ nullptr };
	bool use_custom_texture_rect{ false };
	SDL_Rect to_rect() const {
		return SDL_Rect{ location.x - width / 2, location.y - height / 2, width, height };
	}
};
void draw_sprite(SDL_RenderSprite& sprite, SDL_Renderer*render_target);
void draw_sprites_sdl(entt::registry &registry);

bool load_sprite(std::string path, SDL_RenderSprite& sprite);

bool initialize_sdl();
void start_frame();
void end_frame();
void destroy_sdl();
SDL_Renderer* get_main_renderer();
//converts game space (centered) to screen space
constexpr Vec2i game_space_to_screen_space(Vec2f location) {
	auto up = (UP * location.y);
	auto right = (RIGHT * location.x);
	return coordinate_center + up + right;
}
