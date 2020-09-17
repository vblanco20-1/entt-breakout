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