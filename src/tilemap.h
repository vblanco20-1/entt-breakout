#include <vmath.h>
#include "entt/fwd.hpp"
#include "SDL_rect.h"
#include <array>

struct SDL_Texture;

struct BackgroundTile {
	int tile;
};

struct TileInformation {

	SDL_Rect texture_rect;	
	SDL_Texture* texture{ nullptr };
};

struct TilemapChunk {
	Vec2i ChunkCoordinate;
	std::array<BackgroundTile, 64 * 64> Tiles;
};

struct TileLocation {
	Vec2i coord;
};
struct GlobalTilemap {
//	std::array<TilemapChunk*,16*16> Chunks;

	std::array<BackgroundTile, 256 * 256> Tiles;

	TileInformation DefaulTile;
	void set_tile(Vec2i global_coordinates, int id);
	static Vec2i tile_coord_to_screen(Vec2i coordinate);
};



void update_tilemap(entt::registry& registry);