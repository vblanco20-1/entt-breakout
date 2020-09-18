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
    
    moveState.velocity = { 0.0, 0.0 };
    moveState.initialLocation = defaultLoc;
    moveState.movementTarget = defaultLoc;
    moveState.scaleX = 50;
    moveState.scaleY = 50 * 0.2;
    moveState.speed = 50;
    moveState.distanceTarget = 10;


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
        
        moveState.update_logic(*sourcereg, parent, deltaTime);
        if (statetime < 0) {
            movestate = 1;
            statetime = 1;
        }

    }
    else if (movestate == 1) {
        //no movement
        moveState.velocity = { 0.f,0.f };

        if (statetime < 0) {
            movestate = 2;
            statetime = 1;
        }
    }
    else if (movestate == 2) {
        //swoop down

        moveState.velocity.x = 0;
        moveState.velocity.y = -300;
        if (loc.location.y < 100)
        {
            movestate = 3;
            statetime = 1;
        }
    }
    else if (movestate == 3) {
        moveState.velocity.x = 0;
        moveState.velocity.y = 0;
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
        moveState.velocity.x = 0;
        moveState.velocity.y = +150;
        if (loc.location.y > 650)
        {
            movestate = 0;
            statetime = 10;
            moveState.velocity.y = 0;
        }
    }
    moveState.apply_movement(*sourcereg, parent, deltaTime);

    //loc.location += velocity * deltaTime;
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

   
    moveState.velocity = { 0.0, 0.0 };
    moveState.initialLocation = defaultLoc;
    moveState.movementTarget = defaultLoc;
    moveState.scaleX = 50;
    moveState.scaleY = 50 * 0.2;
    moveState.speed = 50;
    moveState.distanceTarget = 10;
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
    moveState.update_logic(*sourcereg, parent, deltaTime);
    moveState.apply_movement(*sourcereg, parent, deltaTime);

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

void BOSS_03::init(entt::registry* rg, entt::entity pe)
{
    AIImplementation::init(rg, pe);

    moveState.velocity = { 0.0, 0.0 };
    moveState.initialLocation = defaultLoc;
    moveState.movementTarget = defaultLoc;
    moveState.scaleX = 60;
    moveState.scaleY = 50 * 0.2;
    moveState.speed = 70;
    moveState.distanceTarget = 20;

    balltimer = 2;

    entt::registry& registry = *rg;
    {
        //initialize spawner
        circle_spawner = registry.create();
        registry.assign < SpriteLocation>(circle_spawner, registry.get<SpriteLocation>(pe));

        registry.assign<ChildEntity>(circle_spawner);
        registry.get<ChildEntity>(circle_spawner).parent = pe;
        registry.assign<BulletSpawner>(circle_spawner);

        BulletSpawner& bspawner = registry.get<BulletSpawner>(circle_spawner);
        bspawner.fireRate = 2;
        bspawner.type = BulletType::BOSS_01;
        bspawner.rotation = 0;
        bspawner.elapsed = 1;
        for (int i = 0; i <= 36; i++) {
            float angledeg = (360 / 36) * i;

            float anglerad = angledeg * DEG_2_RAD;

            float x = cos(anglerad);
            float y = sin(anglerad);

            BulletData b0;
            b0.velocity = Vec2f{ x , y } *500;
            b0.offset = Vec2f{ x,y } *10;
            b0.rotation = angledeg;
            bspawner.bullets.push_back(b0);

            BulletData b1;
            b1.velocity = Vec2f{ x , y } *500;
            b1.offset = Vec2f{ x,y } *100;
            b1.rotation = angledeg;
            bspawner.bullets.push_back(b1);
        }
    }

    {
        //initialize spawner
        hyper_spawner = registry.create();
        registry.assign < SpriteLocation>(hyper_spawner, registry.get<SpriteLocation>(pe));

        registry.assign<ChildEntity>(hyper_spawner);
        registry.get<ChildEntity>(hyper_spawner).parent = pe;
        registry.assign<BulletSpawner>(hyper_spawner);

        BulletSpawner& bspawner = registry.get<BulletSpawner>(hyper_spawner);
        bspawner.fireRate = 5;
        bspawner.type = BulletType::BOSS_01;
        bspawner.rotation = 0;
        bspawner.elapsed = 0;
        bspawner.scaleMultiplier = 3.5;
        bspawner.enabled = false;
        for (int x = 0; x < 16; x++) {
            for (int y = 0; y < 4; y++) {


                BulletData b0;
                b0.velocity = Vec2f{ 0.f, -200.f };
                b0.offset = Vec2f{ x * 100.f + (y % 2) * 50.f - 800.f, y * 100.f }; //+ defaultLoc;
                b0.rotation = 0;
                bspawner.bullets.push_back(b0);
            }
        }       
    }

    state = 0;
    stateTimer = 10;
}

void BOSS_03::update(float deltaTime)
{
    if (state == 0)
    {
        moveState.update_logic(*sourcereg, parent, deltaTime);
        moveState.apply_movement(*sourcereg, parent, deltaTime);
    }   

    stateTimer -= deltaTime;

    if (stateTimer < 0)
    {
        if (state == 0)
        {
           
            //blast
            sourcereg->get<BulletSpawner>(hyper_spawner).enabled = true;
            sourcereg->get<BulletSpawner>(hyper_spawner).elapsed = 0;

            sourcereg->get<BulletSpawner>(circle_spawner).enabled = false;

            stateTimer = 5;
            state = 1;
        }
        else if (state == 1) {
            //stop blast
            sourcereg->get<BulletSpawner>(hyper_spawner).enabled = false;
            sourcereg->get<BulletSpawner>(hyper_spawner).elapsed = 0;


            sourcereg->get<BulletSpawner>(circle_spawner).enabled = true;

            stateTimer = 10;
            state = 0;
        }
    }
    entt::entity player{ 0 };
    for (auto et : sourcereg->view<PlayerInputComponent>()) {
        player = et;
    }
    //launch direct balls
    if(state == 0 && sourcereg->valid(player))
    {
        balltimer -= deltaTime;
        if (balltimer < 0) {
            balltimer = 2;

            Vec2f playerLoc = sourcereg->get<SpriteLocation>(player).location;
            Vec2f selflocation = sourcereg->get<SpriteLocation>(parent).location;

            Vec2f toplayer = (playerLoc - selflocation).normalized();

            for (int i = 0; i < 1; i++) {

                Vec2f spawnLoc = selflocation + (toplayer * (i * 20.f));
                float velocity = (i * 20.f) + 400;
                Vec2f spawnVel = (toplayer * velocity);

                auto et = build_bullet("BULLET_BOSS_03", *sourcereg, spawnVel, spawnLoc, 0);

                
            }
        }
    }

}

void RandomMovementState::update_logic(entt::registry& registry, entt::entity et, float deltaTime)
{
    SpriteLocation& loc = registry.get<SpriteLocation>(et);
    //random walk movement logic
    Vec2f totarget = (movementTarget - loc.location);

    float len = totarget.lenght();

    if (len < distanceTarget) {
        Vec2f rng;
        rng.x = RandomFloat() * scaleX;
        rng.y = RandomFloat() * scaleY;
        movementTarget = initialLocation + (rng);
    }
    totarget = (movementTarget - loc.location).normalized();
    velocity += (totarget * speed) * deltaTime;
}

void RandomMovementState::apply_movement(entt::registry& registry, entt::entity et, float deltaTime)
{
    SpriteLocation& loc = registry.get<SpriteLocation>(et);

    loc.location += velocity * deltaTime;
}
