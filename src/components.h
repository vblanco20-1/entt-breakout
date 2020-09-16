#pragma once
#include "vmath.h"


#include <unordered_map>

struct SpriteLocation{
	Vec2f location;
};
struct RenderScale {
	Vec2f scale;
};
struct Ball {
	int num_bounces;
};
struct Bullet {
    int num_bounces;
};
struct Brick {
	int hits_left;
	int score_value;
};
struct MovementComponent {
	Vec2f acceleration;
	Vec2f velocity;
	Vec2f max_velocity;
};

struct PlayerInputComponent {
	Vec2f movement_input;

	std::unordered_map<std::string, bool> buttonMap;

	PlayerInputComponent() {
		buttonMap["UP"] = false;
		buttonMap["DOWN"] = false;
		buttonMap["LEFT"] = false;
		buttonMap["RIGHT"] = false;
		buttonMap["SPACE"] = false;
	}
};

struct BulletData {
	Vec2f offset;
	Vec2f velocity;
};

struct BulletSpawner {
	float elapsed = 0.f;
	float fireRate = 0.1f;



	std::vector<BulletData> bullets;
};