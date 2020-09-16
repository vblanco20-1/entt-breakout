#include "sdl_render.h"
#include <SDL.h>
#include <SDL_image.h>
#include "entt/entt.hpp"

SDL_Renderer *gRenderer;
SDL_Window *gWindow;

void draw_sprite(SDL_RenderSprite& sprite, SDL_Renderer*render_target) {
	if (render_target && sprite.texture)
	{
		SDL_Rect renderQuad = sprite.to_rect();

		SDL_RenderCopy(render_target, sprite.texture, &sprite.texture_rect, &renderQuad);
	}
}


void draw_sprites_sdl(entt::registry &registry)
{
	auto spriteview = registry.view<SDL_RenderSprite>();

	for (auto et : spriteview)
	{
		SDL_RenderSprite &sprite = spriteview.get(et);
		draw_sprite(sprite, gRenderer);
	}
}


bool load_sprite(std::string path, SDL_RenderSprite& sprite)
{
	static std::unordered_map<std::string, SDL_RenderSprite> TextureCache;
	//The final texture
	SDL_Texture* newTexture = NULL;

	if (TextureCache.find(path) == TextureCache.end()) {
		//Load image at specified path
		SDL_Surface* loadedSurface = IMG_Load(path.c_str());
		if (loadedSurface == NULL)
		{
			printf("Unable to load image %s! SDL_image Error: %s\n", path.c_str(), IMG_GetError());
			return false;
		}
		else
		{
			//Create texture from surface pixels
			newTexture = SDL_CreateTextureFromSurface(gRenderer, loadedSurface);
			if (newTexture == NULL)
			{
				printf("Unable to create texture from %s! SDL Error: %s\n", path.c_str(), SDL_GetError());
			}
			else {
				printf("successful load of sprite %s \n", path.c_str());
			}

			sprite.texture = newTexture;
			sprite.texture_rect.x = 0;
			sprite.texture_rect.y = 0;
			sprite.texture_rect.w = loadedSurface->w;
			sprite.texture_rect.h = loadedSurface->h;
			sprite.height = loadedSurface->h;
			sprite.width = loadedSurface->w;
			sprite.location.x = 0;
			sprite.location.y = 0;
			TextureCache[path] = sprite;
			//Get rid of old loaded surface
			SDL_FreeSurface(loadedSurface);

		}
	}
	else {
		auto cached_sprite = TextureCache[path];
		sprite = cached_sprite;
		//printf("successful load of sprite from cache %s \n", path.c_str());
	}

	return true;

}

bool initialize_sdl()
{
	SDL_Init(SDL_INIT_VIDEO);

	gWindow = SDL_CreateWindow(
		"SDL2Test",
		SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED,
		WINDOW_WIDTH,
		WINDOW_HEIGHT,
		0
	);

	//Initialize PNG loading
	int imgFlags = IMG_INIT_PNG;
	if (!(IMG_Init(imgFlags) & imgFlags))
	{
		printf("SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError());
		return false;
	}

	gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (!gRenderer)
	{
		return false;
	}

	SDL_SetRenderDrawColor(gRenderer, 0, 0, 0, SDL_ALPHA_OPAQUE);

	return true;
}

void start_frame()
{
	//Clear screen
	SDL_RenderClear(gRenderer);
}

void end_frame()
{
	//Update screen
	SDL_RenderPresent(gRenderer);
}

void destroy_sdl()
{
	SDL_Delay(3000);

	SDL_DestroyWindow(gWindow);
	SDL_Quit();
}

SDL_Renderer* get_main_renderer()
{
	return gRenderer;
}
