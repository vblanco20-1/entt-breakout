#include <bossAI.h>
#include <gameutils.h>
#include <random>
#include "sdl_render.h"

std::default_random_engine generator;
std::uniform_int_distribution<int> distribution(1, 10);
std::uniform_int_distribution<int> randompos(-30, 30);
std::uniform_real_distribution<float> randomfloat(-1, 1);


float RandomFloat() {
    return randomfloat(generator);
}

void BOSS_01::init(entt::registry* rg, entt::entity pe)
{
    AIImplementation::init(rg, pe);

    //create spawner child

    entt::registry& registry = *rg;
    //initialize spawner
    main_spawner = registry.create();
    registry.assign < SpriteLocation>(main_spawner, registry.get<SpriteLocation>(pe));

    registry.assign<ChildEntity>(main_spawner);
    registry.get<ChildEntity>(main_spawner).parent = pe;
    registry.assign<BulletSpawner>(main_spawner);

    BulletSpawner& bspawner = registry.get<BulletSpawner>(main_spawner);
    bspawner.fireRate = 0.5;
    bspawner.type = BulletType::BOSS_01;
    bspawner.rotation = 0;
    bspawner.elapsed = 3;
    for (int i = 0; i <= 28; i++) {
        float angledeg = (360 / 28) * i;

        float anglerad = angledeg * DEG_2_RAD;

        float x = cos(anglerad);
        float y = sin(anglerad);

        BulletData b0;
        b0.velocity = Vec2f{ x , y } *300;
        b0.offset = Vec2f{ x,y } *100;
        b0.rotation = angledeg;
        bspawner.bullets.push_back(b0);
    }
    velocity = {0.0, 0.0};
    movementTarget = defaultLoc;
    pstate.elapsed = 0.1;
    pstate.firerate = 1;
    stage = 0;
    enablePursuer = false;
   
    movestate = 0;
    statetime = 10;
}

void BOSS_01::update(float deltaTime)
{
    entt::entity player{0};
    for (auto et : sourcereg->view<PlayerInputComponent>()) {
        player = et;
    }

    if (!sourcereg->valid(player)) return;

    int percent = sourcereg->get<Health>(parent).getPercent();

    if (stage == 0 && percent < 70)
    {
        //enable pursuers at 70% hp
        stage = 1;
        enablePursuer = true;
    }
    else if (stage == 1 && percent < 30)
    {
        stage = 2;
        //increase fire rates

        sourcereg->get<BulletSpawner>(main_spawner).fireRate = 0.3;
    }

    
    BulletSpawner &spawner = sourcereg->get<BulletSpawner>(main_spawner);
        
    spawner.enabled = movestate == 0 || movestate == 3;
    if (enablePursuer & (movestate == 0))
    {
        pstate.elapsed -= deltaTime;
        if (pstate.elapsed < 0) {
            pstate.elapsed = pstate.firerate;

            Vec2f playerLoc = sourcereg->get<SpriteLocation>(player).location;
            Vec2f selflocation = sourcereg->get<SpriteLocation>(parent).location;

            Vec2f toplayer = (playerLoc- selflocation).normalized();

            for (int i = 0; i < 5; i++) {

                Vec2f spawnLoc = selflocation + ( toplayer* (i * 20.f));
                float velocity = (i * 20.f) + 400;
                Vec2f spawnVel = (toplayer * velocity) ;

                build_bullet("BULLET_BOSS_02",*sourcereg, spawnVel, spawnLoc, 0);
            }
        }
    }

    statetime -= deltaTime;

    SpriteLocation& loc = sourcereg->get<SpriteLocation>(parent);
    if (movestate == 0)
    {
        //random walk movement logic
        Vec2f totarget = (movementTarget - loc.location);

        float len = totarget.lenght();

        if (len < 30) {
            Vec2f rng;
            rng.x = RandomFloat();
            rng.y = RandomFloat() * 0.2;
            movementTarget = defaultLoc + (rng * 50);
        }
        totarget = (movementTarget - loc.location).normalized();
        velocity += (totarget * 50) * deltaTime;
        if (statetime < 0) {
            movestate = 1;
            statetime = 1;
        }

    }
    else if (movestate == 1) {
        //no movement
        velocity = { 0.f,0.f };

        if (statetime < 0) {
            movestate = 2;
            statetime = 1;
        }
    }
    else if (movestate == 2) {
        //swoop down

        velocity.x = 0;
        velocity.y = -300;
        if (loc.location.y < 100)
        {
            movestate = 3;
            statetime = 1;
        }
    }
    else if (movestate == 3) {
        velocity.x = 0;
        velocity.y = 0;
        spawner.fireRate = 0.1;
        spawner.rotation += deltaTime * 15;
        //no movement
        if (statetime < 0) {
            spawner.fireRate = 0.5;
            spawner.rotation = 0;

            movestate = 4;
            statetime = 1;
        }
    }
    else if (movestate == 4) {
        //swoop up
        velocity.x = 0;
        velocity.y = +150;
        if (loc.location.y > 650)
        {
            movestate = 0;
            statetime = 10;
            velocity.y = 0;
        }
    }

    loc.location += velocity * deltaTime;
}

void BOSS_02::init(entt::registry* rg, entt::entity pe)
{
    AIImplementation::init(rg, pe);

    //create spawner child

    entt::registry& registry = *rg;
    


    //create swirl spawners
    for (int i = 0; i < 3; i++) {
        //initialize spawner
        build_spawner(registry, pe, i);

    }

   
    velocity = { 0.0, 0.0 };
    movementTarget = defaultLoc;
}

void BOSS_02::build_spawner(entt::registry& registry, entt::entity pe, int i)
{
    entt::entity main_spawner = registry.create();
    registry.assign < SpriteLocation>(main_spawner, registry.get<SpriteLocation>(pe));

    registry.assign<ChildEntity>(main_spawner);
    registry.get<ChildEntity>(main_spawner).parent = pe;
    registry.assign<BulletSpawner>(main_spawner);

    BulletSpawner& bspawner = registry.get<BulletSpawner>(main_spawner);
    bspawner.fireRate = 0.1 * (1 / spawn_rotationspeed[i]);
    bspawner.type = BulletType::BOSS_01;
    bspawner.rotation = spawn_angles[i];
    bspawner.scaleMultiplier = spawn_scales[i];
    bspawner.elapsed = 3;

    //only one bullet at once
    BulletData b0;
    b0.velocity = Vec2f{ 1 , 0 } *300;
    b0.offset = Vec2f{ 0,0 };
    b0.rotation = 0;
    bspawner.bullets.push_back(b0);

    spawners.push_back(main_spawner);
}

void BOSS_02::update(float deltaTime)
{
    SpriteLocation& loc = sourcereg->get<SpriteLocation>(parent);
    //random walk movement logic
    Vec2f totarget = (movementTarget - loc.location);

    float len = totarget.lenght();

    if (len < 30) {
        Vec2f rng;
        rng.x = RandomFloat();
        rng.y = RandomFloat() * 0.2;
        movementTarget = defaultLoc + (rng * 50);
    }
    totarget = (movementTarget - loc.location).normalized();
    velocity += (totarget * 50) * deltaTime;
    loc.location += velocity * deltaTime;

    sourcereg->get<SDL_RenderSprite>(parent).has_rotation = true;
    sourcereg->get<SDL_RenderSprite>(parent).rotation += 180 * deltaTime;
    
    int percent = sourcereg->get<Health>(parent).getPercent();

    if (percent < 70 && spawners.size() == 3)
    {
        build_spawner(*sourcereg, parent, 4);
    }
    if (percent < 40 && spawners.size() == 4)
    {
        build_spawner(*sourcereg, parent, 5);
    }
    
    //spin spawners

    for (int i = 0; i < spawners.size(); i++) {
        BulletSpawner& spawner =  sourcereg->get<BulletSpawner>(spawners[i]);
        float extrarot = spawn_rotationspeed[i] * 180;
        spawner.rotation += deltaTime * extrarot;
    }
}
