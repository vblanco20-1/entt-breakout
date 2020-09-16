#include <unordered_map>
#include <vector>
#include <string>
#include "SDL_rect.h"
#include "vmath.h"

struct SDL_Texture;
struct SpriteGlyph {
	int width;
	int height;
	int xoffset;
	int yoffset;
	char id;
	int xadvance;

	SDL_Rect texture_rect;
	SDL_Texture* texture{ nullptr };
};

struct BitFont {

	std::unordered_map<char, SpriteGlyph> loaded_glyphs;
	SDL_Texture* texture;
};
std::vector<std::string> split_string(const std::string& target, const std::string& split_token);
bool load_font(BitFont& font,std::string image_path, std::string info_path);
void draw_string(BitFont& font, std::string str, Vec2i coordinates);


