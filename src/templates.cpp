#include <templates.h>
#include <sdl_render.h>
#include "animation.h"

void create_default_templates(EntityDatabase& db) {

    entt::registry& registry = db.databaseRegistry;
    //player
    {
        //initialize player
        auto player_entity = registry.create();
        registry.assign<SDL_RenderSprite>(player_entity);
        registry.assign<MovementComponent>(player_entity);
        registry.assign<BulletSpawner>(player_entity);
        registry.assign<RenderScale>(player_entity, Vec2f{ 0.3f,1.0f });
        load_sprite("../assets/sprites/paddleBlu.png", registry.get<SDL_RenderSprite>(player_entity));
        registry.assign<SphereCollider>(player_entity);

        registry.get<SDL_RenderSprite>(player_entity).sortlayer = 100000;

        registry.get<SphereCollider>(player_entity).radius = 5;

        BulletSpawner& bspawner = registry.get<BulletSpawner>(player_entity);
        bspawner.fireRate = 0.15;
        for (int i = -2; i <= 2; i++) {
            float angledeg = i * 10 + 90;

            float anglerad = angledeg * 0.01745329252;

            float x = cos(anglerad);
            float y = sin(anglerad);

            BulletData b0;
            b0.velocity = Vec2f{ x,y } *800;
            b0.offset = { 0,0 };
            b0.rotation = angledeg;
            bspawner.bullets.push_back(b0);
        }

        db.templateMap["PLAYER"] = player_entity;
    }

    //boss
    {
        //initialize boss
        auto boss_entity = registry.create();
        registry.assign<SDL_RenderSprite>(boss_entity);
        registry.assign<BossMovementComponent>(boss_entity);
        registry.assign<SphereCollider>(boss_entity);
        registry.assign<RenderScale>(boss_entity, Vec2f{ 5.0f,5.0f });
        registry.assign<Health>(boss_entity, Health{ 500,500 });
        //registry.assign<Health>(boss_entity, Health{ 50,50 });
        load_sprite("../assets/sprites/element_red_polygon_glossy.png", registry.get<SDL_RenderSprite>(boss_entity));
        BossMovementComponent& bmov = registry.get<BossMovementComponent>(boss_entity);

        registry.get<SphereCollider>(boss_entity).radius = 100;

        registry.get<SDL_RenderSprite>(boss_entity).sortlayer = -1000;

        bmov.center.x = 0;
        bmov.center.y = 700.f;
        bmov.period = 0.3;

        registry.assign<BossAIName>(boss_entity, "AI_BOSS_01");


        registry.assign<BulletSpawner>(boss_entity);

        db.templateMap["BOSS_01"] = boss_entity;
    }

    //boss 2
    {
        //initialize boss
        auto boss_entity = registry.create();
        registry.assign<SDL_RenderSprite>(boss_entity);
        registry.assign<BossMovementComponent>(boss_entity);
        registry.assign<SphereCollider>(boss_entity);
        registry.assign<RenderScale>(boss_entity, Vec2f{ 5.0f,5.0f });
        registry.assign<Health>(boss_entity, Health{ 500,500 });
        //registry.assign<Health>(boss_entity, Health{ 50,50 });
        load_sprite("../assets/sprites/element_blue_polygon_glossy.png", registry.get<SDL_RenderSprite>(boss_entity));
        BossMovementComponent& bmov = registry.get<BossMovementComponent>(boss_entity);

        registry.get<SphereCollider>(boss_entity).radius = 100;

        registry.get<SDL_RenderSprite>(boss_entity).sortlayer = -1000;

        bmov.center.x = 0;
        bmov.center.y = 700.f;
        bmov.period = 0.3;

        registry.assign<BossAIName>(boss_entity, "AI_BOSS_02");


        registry.assign<BulletSpawner>(boss_entity);

        db.templateMap["BOSS_02"] = boss_entity;
    }

    //player bullet

    {
        //ball
        auto ball_entity = registry.create();
        registry.assign<SDL_RenderSprite>(ball_entity);
        registry.assign<Bullet>(ball_entity);
        registry.assign<PlayAnimComponent>(ball_entity, "tiny_zombie_run_anim");
        registry.assign<RenderScale>(ball_entity, Vec2f{ 0.5f,0.5f });
        registry.assign<MovementComponent>(ball_entity);
        load_sprite("../assets/sprites/ballGrey.png", registry.get<SDL_RenderSprite>(ball_entity));

        registry.get<SDL_RenderSprite>(ball_entity).sortlayer = -5000;
        registry.assign<SphereCollider>(ball_entity);

        registry.get<SphereCollider>(ball_entity).radius = 10;

        registry.get<Bullet>(ball_entity).bHitBoss = true;
        registry.get<Bullet>(ball_entity).bHitPlayer = false;

        db.templateMap["BULLET_PLAYER_01"] = ball_entity;
    }
    {
        //ball
        auto ball_entity = registry.create();
        registry.assign<SDL_RenderSprite>(ball_entity);
        registry.assign<Bullet>(ball_entity);
        registry.assign<RenderScale>(ball_entity, Vec2f{ 1.0f,1.0f });
        registry.assign<MovementComponent>(ball_entity);
        load_sprite("../assets/sprites/ballBlue.png", registry.get<SDL_RenderSprite>(ball_entity));
        registry.assign<SphereCollider>(ball_entity);

        registry.get<SphereCollider>(ball_entity).radius = 10;

        registry.get<Bullet>(ball_entity).bHitBoss = false;
        registry.get<Bullet>(ball_entity).bHitPlayer = true;

        registry.get<SDL_RenderSprite>(ball_entity).sortlayer = 100;



        db.templateMap["BULLET_BOSS_01"] = ball_entity;
    }
    {
        //ball
        auto ball_entity = registry.create();
        registry.assign<SDL_RenderSprite>(ball_entity);
        registry.assign<Bullet>(ball_entity);
        registry.assign<RenderScale>(ball_entity, Vec2f{ 1.0f,1.0f });
        registry.assign<MovementComponent>(ball_entity);
        load_sprite("../assets/sprites/element_purple_diamond.png", registry.get<SDL_RenderSprite>(ball_entity));
        registry.assign<SphereCollider>(ball_entity);

        registry.get<SphereCollider>(ball_entity).radius = 10;

        registry.get<Bullet>(ball_entity).bHitBoss = false;
        registry.get<Bullet>(ball_entity).bHitPlayer = true;

        registry.get<SDL_RenderSprite>(ball_entity).sortlayer = 200;


        db.templateMap["BULLET_BOSS_02"] = ball_entity;
    }
}