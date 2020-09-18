#pragma once
#include <components.h>

static const Vec2f defaultLoc{ 0.0f, 700.0f };
class BOSS_01 : public AIImplementation {
public:
    virtual void init(entt::registry* registry, entt::entity pe);
    virtual void update(float deltaTime);

    int stage;

    struct PursuerState {
        
        float firerate;
        float elapsed;
    };

    bool enablePursuer{false};
    PursuerState pstate;
    entt::entity main_spawner;

    Vec2f movementTarget;
    Vec2f velocity;
    
    int movestate;
    float statetime;
};

class BOSS_02 : public AIImplementation {
public:
    virtual void init(entt::registry* registry, entt::entity pe);

    void build_spawner(entt::registry& registry, entt::entity pe, int i);

    virtual void update(float deltaTime);


    std::vector<entt::entity> spawners;

    Vec2f movementTarget;
    Vec2f velocity;

    static constexpr float spawn_angles[] = { 10.f,35.f,50.f,40.f,90.f };

    static constexpr float spawn_rotationspeed[] = { 1.f,2.f,0.3,0.7,0.5 };

    static constexpr float spawn_scales[] = { 1.f,0.6,2,1.5,0.5 };
};