#include "sdl_render.h"
#include <SDL.h>
#include <SDL_image.h>
#include "entt/entt.hpp"

#include "imgui.h"
#include "imgui_sdl.h"
#include "examples/imgui_impl_sdl.h"
#include <iostream>
#include "components.h"
#include "compedit.h"
SDL_Renderer *gRenderer;
SDL_Window *gWindow;
SDL_Texture* texTarget;
void draw_sprite(SDL_RenderSprite& sprite, SDL_Renderer*render_target) {
	if (render_target && sprite.texture)
	{
		SDL_Rect renderQuad = sprite.to_rect();

		//SDL_SetTextureColorMod(sprite.texture,
		//	255,
		//	0,
		//	0);


		if (sprite.has_rotation) {
			SDL_RenderCopyEx(render_target, sprite.texture, &sprite.texture_rect, &renderQuad,sprite.rotation,nullptr,SDL_RendererFlip::SDL_FLIP_NONE);
		}
		else {
			SDL_RenderCopy(render_target, sprite.texture, &sprite.texture_rect, &renderQuad);
		}		
	}
}


void draw_sprites_sdl(entt::registry &registry)
{
	auto spriteview = registry.view<SDL_RenderSprite>();
	SDL_SetRenderTarget(gRenderer,texTarget);

  	SDL_RenderClear(gRenderer);

	static std::vector<SDL_RenderSprite*> sortsprites;
	sortsprites.clear();

	for (auto et : spriteview)
	{
		SDL_RenderSprite &sprite = spriteview.get(et);
		sortsprites.push_back(&sprite);		
	}

	std::sort(sortsprites.begin(), sortsprites.end(), [](auto a, auto b) {
		return a->sortlayer < b->sortlayer;
	});

	for (auto sprite : sortsprites)
	{
		draw_sprite(*sprite, gRenderer);
	}
	

	SDL_SetRenderTarget(gRenderer, nullptr);;
}

void draw_ui(entt::registry& registry)
{

	//ImGui::ShowDemoWindow();
	EngineGlobalData& dty = registry.ctx<EngineGlobalData>();
	ImGui::Begin("Level select");
	if (ImGui::Button("1", { 50,50 })) {
		dty.levelRequest = 1;
	}
	ImGui::SameLine();
	if (ImGui::Button("2", { 50,50 })) {
		dty.levelRequest = 2;
	}
    ImGui::SameLine();
    if (ImGui::Button("3", { 50,50 })) {
        dty.levelRequest = 3;
    }
	ImGui::End();

	

    ImGui::Begin("Test");

    ImGui::Text("DeltaTime: %f ms", dty.deltaTime * 1000.f);
    ImGui::SliderFloat("Time dilation", &dty.timeDilation, 0, 10);
    ImGui::Separator();

    display_editor(registry);
    ImGui::End();
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

    SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
	gWindow = SDL_CreateWindow(
		"Bulletgame",
		SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED,
		TRUE_WINDOW_WIDTH,
		WINDOW_HEIGHT,
		window_flags
	);
   
	//Initialize PNG loading
	int imgFlags = IMG_INIT_PNG;
	if (!(IMG_Init(imgFlags) & imgFlags))
	{
		printf("SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError());
		return false;
	}

	gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED /*| SDL_RENDERER_PRESENTVSYNC*/);
	if (!gRenderer)
	{
		return false;
	}

    //Make a target texture to render too
    texTarget = SDL_CreateTexture(gRenderer, SDL_PIXELFORMAT_RGBA8888,
        SDL_TEXTUREACCESS_TARGET, WINDOW_WIDTH, WINDOW_HEIGHT);

	SDL_SetRenderDrawColor(gRenderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
	

    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
	ImGui_ImplSDL2_InitForD3D(gWindow);
	ImGuiSDL::Initialize(gRenderer, TRUE_WINDOW_WIDTH, WINDOW_HEIGHT);
    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsClassic();

  


	return true;
}

void start_frame(entt::registry& registry)
{
  
    //Clear screen
	SDL_SetRenderTarget(gRenderer, nullptr);
	SDL_SetRenderDrawColor(gRenderer, 0, 0, 100, SDL_ALPHA_OPAQUE);
	SDL_RenderClear(gRenderer);
	SDL_SetRenderDrawColor(gRenderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
	ImGui_ImplSDL2_NewFrame(gWindow);
	ImGui::NewFrame();
	

	draw_ui(registry);
	
    ImGui::Render();

    ImGuiSDL::Render(ImGui::GetDrawData());
       
}


void draw_screen()
{
	SDL_Rect dst;

	dst.x = TRUE_WINDOW_WIDTH / 2 - WINDOW_WIDTH / 2;
	dst.y = 0;
	dst.w = WINDOW_WIDTH;
	dst.h = WINDOW_HEIGHT;

	SDL_RenderCopyEx(gRenderer, texTarget, nullptr, &dst, 0, nullptr, SDL_FLIP_NONE);
}

void end_frame(entt::registry& registry)
{
	//draw_screen();

	
	SDL_RenderFlush(gRenderer);
	SDL_RenderPresent(gRenderer);
}

void destroy_sdl()
{
	SDL_Delay(3000);

	SDL_DestroyWindow(gWindow);
	SDL_Quit();
}


void process_imgui_event(SDL_Event* event)
{
	ImGui_ImplSDL2_ProcessEvent(event);
}

SDL_Renderer* get_main_renderer()
{
	return gRenderer;
}
