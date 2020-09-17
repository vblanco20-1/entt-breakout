#include <bossAI.h>
#include <gameutils.h>

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
    for (int i = -8; i <= 8; i++) {
        float angledeg = i * 10 - 90;
    
        float anglerad = angledeg * DEG_2_RAD;
    
        float x = cos(anglerad);
        float y = sin(anglerad);
    
        BulletData b0;
        b0.velocity = Vec2f{ x , y } *400;
        b0.offset = Vec2f{ x,y } *100;
    	b0.rotation = angledeg;
        bspawner.bullets.push_back(b0);
    }

    pstate.elapsed = 0.1;
    pstate.firerate = 1;
    stage = 0;
    enablePursuer = false;
}

void BOSS_01::update(float deltaTime)
{
    entt::entity player;
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

    if (enablePursuer)
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
}
