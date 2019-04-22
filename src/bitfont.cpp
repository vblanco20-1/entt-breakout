
#include "bitfont.h"
#include <SDL.h>
#include <SDL_image.h>
#include "sdl_render.h"
#include <iostream>
#include <fstream>
#include <sstream>

using namespace std;
vector<string> split_string(const string &target, const string &split_token) {
	vector<string> output;

	auto start = 0U;
	auto end = target.find(split_token);
	while (end != std::string::npos)
	{
		string word = target.substr(start, end - start);
		if(word.length() > 0)
		{
			output.push_back(std::move(word));
		}
		
		start = end + split_token.length();
		end = target.find(split_token, start);
	}
	string word = target.substr(start, end - start);
	if (word.length() > 0)
	{
		output.push_back(std::move(word));
	}

	return output;

}

struct ParamToken
{
	std::string str;
	int num;
};

ParamToken parse_parameter(const string &target)
{
	vector<string> tokens = split_string(target, "=");
	ParamToken NewToken;

	NewToken.str = tokens[0];
	NewToken.num = stoi(tokens[1]);

	return NewToken;
}
bool load_font(BitFont& font, std::string image_path, std::string info_path)
{
	SDL_RenderSprite all_sprite;
	if (!load_sprite(image_path, all_sprite))
	{
		return false;
	}

	string line;
	ifstream myfile(info_path);
	std::vector<std::string> info_lines;
	if (myfile.is_open())
	{
		while (getline(myfile, line))
		{
			std::vector<std::string> tokens = split_string(line, " ");

			if (tokens[0] == "char")
			{
				SpriteGlyph glyph;
				glyph.texture = all_sprite.texture;
				
				for (int i = 1; i < tokens.size(); i++) {
					ParamToken param = parse_parameter(tokens[i]);

					if (param.str == "id") {
						glyph.id = param.num;
					}
					else if (param.str == "x") {
						glyph.texture_rect.x = param.num;
					}
					else if (param.str == "y") {
						glyph.texture_rect.y = param.num;
					}
					else if (param.str == "width") {
						glyph.texture_rect.w = param.num;
						glyph.width = param.num;
					}
					else if (param.str == "height") {
						glyph.height = param.num;
						glyph.texture_rect.h = param.num;
					}
					else if (param.str == "xadvance") {
						glyph.xadvance = param.num;
						
					}
					else if (param.str == "xoffset") {
						glyph.xoffset = param.num;

					}
					else if (param.str == "yoffset") {
						glyph.yoffset = param.num;

					}
				}

				font.loaded_glyphs[glyph.id] = glyph;
			}

			cout << line << '\n';
		}
		myfile.close();
	}
	

	return false;
}

void draw_string(BitFont& font,std::string str, Vec2i coordinates)
{
	Vec2i coords = coordinates;
	for (auto c : str)
	{
		SDL_RenderSprite GlyphSprite;
		
		if (font.loaded_glyphs.find(c) != font.loaded_glyphs.end()) {

			SpriteGlyph glyph = font.loaded_glyphs[c];

			GlyphSprite.height = glyph.height;
			GlyphSprite.width = glyph.width;
			GlyphSprite.texture = glyph.texture;
			GlyphSprite.texture_rect = glyph.texture_rect;
			coords.x += glyph.xadvance;
			GlyphSprite.location = coords + Vec2i{ glyph.xoffset,glyph.yoffset };

			draw_sprite(GlyphSprite, get_main_renderer());
			
				
		}
	}
}
