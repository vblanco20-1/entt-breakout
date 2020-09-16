
#include "animation.h"
#include "bitfont.h"
#include <iostream>
#include <fstream>
#include <sstream>

#include "sdl_render.h"
#include "entt/entt.hpp"
using namespace std;
void AnimationDatabase::load_animations_from_tileinfo(const std::string& infopath, const std::string& spritesheet)
{
	string line;
	ifstream myfile(infopath);

	if (myfile.is_open())
	{
		SDL_RenderSprite Spritesheet;
		SDL_Texture* sprite_texture;
		bool loaded = load_sprite(spritesheet, Spritesheet);
		if (!loaded) {
			return;
		}
		else {
			sprite_texture = Spritesheet.texture;
		}


		while (getline(myfile, line))
		{
			std::vector<std::string> tokens = split_string(line, " ");
			int x, y, w, h, len;
			//only interested on the 6 long ones, with animation data
			if (tokens.size() == 6) {
				std::string name = tokens[0];
				x = stoi(tokens[1]);
				y = stoi(tokens[2]);
				w = stoi(tokens[3]);
				h = stoi(tokens[4]);
				len = stoi(tokens[5]);

				auto newTrack = std::make_shared<AnimationTrack>();
				
				for (int i = 0; i < len; i++) {
					FrameData frame;
					frame.framenum = i;
					frame.notify_name = "";
					frame.texture_rect.h = h;
					frame.texture_rect.w = w;

					frame.texture_rect.x = x + w * i;
					frame.texture_rect.y = y;
					frame.texture = sprite_texture;
					newTrack->frames.push_back(frame);
				}

				anim_tracks[name] = newTrack;
				cout << "animation succesfully loaded: " << name << endl;
			}
			//static sprite
			if (tokens.size() == 5) {
				std::string name = tokens[0];
				x = stoi(tokens[1]);
				y = stoi(tokens[2]);
				w = stoi(tokens[3]);
				h = stoi(tokens[4]);
				
				auto newTrack = std::make_shared<AnimationTrack>();

				for (int i = 0; i < 1; i++) {
					FrameData frame;
					frame.framenum = i;
					frame.notify_name = "";
					frame.texture_rect.h = h;
					frame.texture_rect.w = w;

					frame.texture_rect.x = x + w * i;
					frame.texture_rect.y = y;
					frame.texture = sprite_texture;
					newTrack->frames.push_back(frame);
				}

				anim_tracks[name] = newTrack;
				cout << "animation succesfully loaded: " << name << endl;
			}

			
		}
		myfile.close();
	}	
}


void update_animations(entt::registry& registry) {
	AnimationDatabase& anim_db = registry.ctx<AnimationDatabase>();

	auto create_animview = registry.view<PlayAnimComponent>();
	
	//init animations
	for (auto et : create_animview)
	{
		PlayAnimComponent& anim = create_animview.get(et);
		auto track = anim_db.anim_tracks[anim.animname];

		registry.assign_or_replace<AnimationComponent>(et);
		registry.get<AnimationComponent>(et).currentframe= 0;
		registry.get<AnimationComponent>(et).Track = track;
		registry.get<AnimationComponent>(et).elapsed = 0;

		registry.remove<PlayAnimComponent>(et);
	}

	//update animations
	auto animview = registry.view<AnimationComponent,SDL_RenderSprite>();
	for (auto et : animview)
	{
		AnimationComponent& animc = animview.get<AnimationComponent>(et);
		SDL_RenderSprite& sprite = animview.get<SDL_RenderSprite>(et);

		std::shared_ptr<AnimationTrack> track = animc.Track.lock();

		int numframes = track->frames.size();

		animc.elapsed += (1.0 / 60.0);
		animc.currentframe = int(animc.elapsed * 5.0f) % numframes;

		FrameData currentframe = track->frames[animc.currentframe];

		sprite.texture = currentframe.texture;
		sprite.texture_rect = currentframe.texture_rect;
	}
}