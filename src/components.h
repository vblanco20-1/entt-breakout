#pragma once
#include "vmath.h"


#include <unordered_map>
#include "entt/entt.hpp"

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
	float rotation;
	
};

enum class BulletType:int {
	PLAYER_DEFAULT,
	BOSS_01
};

struct BulletSpawner {
	float elapsed = 0.f;
	float fireRate = 0.1f;
	float rotation = 0;
	BulletType type{ BulletType::PLAYER_DEFAULT };

	std::vector<BulletData> bullets;
};

struct ChildEntity {
	Vec2f childOffset;
	entt::entity parent;
};

struct SphereCollider {
	float radius{ 0.f };
    
	Vec2f centerOffset  { 0.f,0.f };
};

struct EngineGlobalData {
	float deltaTime;
	float timeDilation{ 1.f };
};

struct Editable {
	std::string name;
};

struct OriginalTemplate {
   std::string ogTemplate;
};
struct EntityDatabase {
    entt::registry databaseRegistry;
    std::unordered_map<std::string, entt::entity> templateMap;
};

struct GameEntity {
  
};

class AIImplementation {
public:
	virtual void init(entt::registry * registry, entt::entity pe) {
		sourcereg = registry;
		parent = pe;
	}
	virtual void update(float deltaTime) {

	}

entt::registry* sourcereg;
	entt::entity parent;
};
struct BossAI {
	std::unique_ptr<AIImplementation> impl;
	
};
struct BossAIName {
   
    std::string bossName;
};

struct Health {
	float hp = 100;
	float maxhp = 100;

	int getPercent() {
		int percent = (hp / maxhp) * 100;
		return percent;
	}
};
void clone_entity(entt::registry& sourcereg, entt::entity sourceEnt, entt::registry& destreg, entt::entity destEnt);
void stamp_entity(entt::registry& sourcereg, entt::entity sourceEnt, entt::registry& destreg, entt::entity destEnt);