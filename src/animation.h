#include "SDL_rect.h"
#include "entt/fwd.hpp"
#include <vector>
#include <string>
#include <unordered_map>
#include <memory>

struct SDL_Texture;


struct FrameData {

	int framenum;
	SDL_Rect texture_rect;
	std::string notify_name;
	SDL_Texture* texture{ nullptr };
};

//put this to trigger change of animation
struct PlayAnimComponent {
	std::string animname;
};
struct AnimationTrack {
	//allways sorted
	std::vector<FrameData> frames;
};

struct AnimationComponent {
	std::weak_ptr<AnimationTrack> Track;
	int currentframe;
	float elapsed;
};


struct AnimationDatabase {

	std::unordered_map<std::string, std::shared_ptr<AnimationTrack>> anim_tracks;

	void load_animations_from_tileinfo(const std::string& infopath, const std::string& spritesheet);
};

void update_animations(entt::registry& registry);