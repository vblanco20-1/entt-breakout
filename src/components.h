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

    bool bHitPlayer : 1;
    bool bHitBoss : 1;
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
struct BossMovementComponent {

	//sine based movement
	float elapsed{ 0 };
	float period{ 3 };
	Vec2f center{ 0,0 };
	float size{ 200 };
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

enum class BulletType:int {
	PLAYER_DEFAULT,
	BOSS_01
};

struct BulletSpawner {
	float elapsed = 0.f;
	float fireRate = 0.1f;

	BulletType type{ BulletType::PLAYER_DEFAULT };

	std::vector<BulletData> bullets;
};

struct SphereCollider {
	float radius{ 0.f };
    
	Vec2f centerOffset  { 0.f,0.f };
};

struct EngineGlobalData {
	float deltaTime;
	float timeDilation{ 1.f };
};