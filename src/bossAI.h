#pragma once
#include <components.h>

struct RandomMovementState {
    Vec2f movementTarget;
    Vec2f velocity;

    float distanceTarget;
    float scaleX;
    float scaleY;

    float speed;
    Vec2f initialLocation;

    void update_logic(entt::registry& registry, entt::entity et, float deltaTime);
    void apply_movement(entt::registry& registry, entt::entity et, float deltaTime);
};

//the asteroid
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

    //Vec2f movementTarget;
    //Vec2f velocity;
    
    RandomMovementState moveState;

    int movestate;
    float statetime;
};

//swirly boy
class BOSS_02 : public AIImplementation {
public:
    virtual void init(entt::registry* registry, entt::entity pe);

    void build_spawner(entt::registry& registry, entt::entity pe, int i);

    virtual void update(float deltaTime);


    std::vector<entt::entity> spawners;

    RandomMovementState moveState;

    //Vec2f movementTarget;
    //Vec2f velocity;

    static constexpr float spawn_angles[] = { 10.f,35.f,50.f,40.f,90.f };

    static constexpr float spawn_rotationspeed[] = { 1.f,2.f,0.3,0.7,2.5 };

    static constexpr float spawn_scales[] = { 1.2f,0.8,2,1.5,0.6 };
};

//wizard
class BOSS_03 : public AIImplementation {
public:
    virtual void init(entt::registry* registry, entt::entity pe);
   

    virtual void update(float deltaTime);


    RandomMovementState moveState;


    entt::entity circle_spawner;
    entt::entity hyper_spawner;
   // Vec2f movementTarget;
   // Vec2f velocity; 


    // 0 default move + spam
    // 1 no move + big attack
    int state;
    float stateTimer;
    float balltimer;
};