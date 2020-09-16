#include "tilemap.h"
#include "sdl_render.h"
#include "animation.h"
#include "entt/entt.hpp"
void update_tilemap(entt::registry& registry)
{
	AnimationDatabase& anim_db = registry.ctx<AnimationDatabase>();

	std::vector<FrameData> groundframes;
	groundframes.push_back(anim_db.anim_tracks["floor_1"]->frames[0]);
	groundframes.push_back(anim_db.anim_tracks["floor_2"]->frames[0]);
	groundframes.push_back(anim_db.anim_tracks["floor_3"]->frames[0]);
	groundframes.push_back(anim_db.anim_tracks["floor_4"]->frames[0]);
	groundframes.push_back(anim_db.anim_tracks["floor_5"]->frames[0]);

	srand(0);
	//for (int y = 0; y < 32; y++) {
	//	for (int x = 0; x <32 ; x++) {
	//		SDL_RenderSprite sprite;
	//		sprite.location = GlobalTilemap::tile_coord_to_screen({ x,y });
	//		sprite.location.x += 8.f;
	//		sprite.location.y -= 16.f;
	//
	//		if (sprite.location.x < -20 || sprite.location.x > WINDOW_WIDTH + 20) {
	//			continue;
	//		}
	//		if (sprite.location.y < -20 || sprite.location.y > WINDOW_HEIGHT + 20) {
	//			continue;
	//		}
	//
	//		int rng = rand() %5;
	//		
	//		sprite.texture = groundframes[rng].texture;
	//		sprite.texture_rect = groundframes[rng].texture_rect;
	//
	//		
	//		//sprite.location.x = ;
	//		//sprite.location.y = y * 16;
	//		sprite.width = 16;
	//		sprite.height = 16;
	//		draw_sprite(sprite, get_main_renderer());
	//
	//	}
	//}

	auto view = registry.view<SDL_RenderSprite, TileLocation>();

	for (auto et : view)
	{
		SDL_RenderSprite& sprite = view.get<SDL_RenderSprite>(et);
		TileLocation& location = view.get<TileLocation>(et);
		Vec2i screenspace = GlobalTilemap::tile_coord_to_screen(location.coord);
		
		screenspace.x += sprite.width / 2;
		screenspace.y -= sprite.height;
		sprite.location = screenspace;
		
	}
}

void GlobalTilemap::set_tile(Vec2i global_coordinates, int id)
{

}

Vec2i GlobalTilemap::tile_coord_to_screen(Vec2i coordinate)
{
	return game_space_to_screen_space({ (coordinate.x)*16.f +20 ,(coordinate.y) * 16.f });
}
