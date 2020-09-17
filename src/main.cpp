#include <SDL.h>

#include "entt/entt.hpp"
#include "components.h"
#include "sdl_render.h"
#include "vmath.h"
#include "bitfont.h"
#include "animation.h"
#include "tilemap.h"
#include <chrono>
#include <iostream>
#include <string_view>
#include <bossAI.h>
#include <gameutils.h>
int score = 0;



void transform_sprites(entt::registry &registry)
{


	auto view = registry.view<SDL_RenderSprite,SpriteLocation>();

	for (auto et : view)
	{
		SDL_RenderSprite &sprite = view.get<SDL_RenderSprite>(et);
		SpriteLocation &location = view.get<SpriteLocation>(et);
		Vec2i screenspace = game_space_to_screen_space(location.location);
		sprite.location = screenspace;

		if (registry.has<RenderScale>(et)) {

			RenderScale& scale = registry.get<RenderScale>(et);

			sprite.width = sprite.texture_rect.w * scale.scale.x;
			sprite.height = sprite.texture_rect.h * scale.scale.y;
		}
	}
}

void move_objects(entt::registry &registry, float deltaSeconds) {

	auto view = registry.view<MovementComponent, SpriteLocation>();

	for (auto et : view)
	{
		MovementComponent &movement = view.get<MovementComponent>(et);
		SpriteLocation &location = view.get<SpriteLocation>(et);

		movement.velocity += movement.acceleration * deltaSeconds;
		location.location += movement.velocity * deltaSeconds;
	}
}
void process_boss_movement(entt::registry& registry, float deltaSeconds)
{
	//auto view = registry.view<SpriteLocation, BossMovementComponent>();
	//
	//for (auto et : view)
	//{
	//	SpriteLocation& loc = view.get<SpriteLocation>(et);
	//	BossMovementComponent& bmov = view.get<BossMovementComponent>(et);
	//
	//	bmov.elapsed += deltaSeconds * bmov.period;
	//	loc.location.x = sin(bmov.elapsed) * bmov.size;
	//}

	auto childview = registry.view<SpriteLocation, ChildEntity>();
	for (auto et : childview)
	{
		SpriteLocation& loc = childview.get<SpriteLocation>(et);
		ChildEntity& ch = childview.get<ChildEntity>(et);

		if (registry.valid(ch.parent)) {
			loc.location = registry.get<SpriteLocation>(ch.parent).location + ch.childOffset;
		}
		else {
			registry.destroy(et);
		}
	}
}
void process_player_movement(entt::registry &registry) {
	auto view = registry.view<PlayerInputComponent,MovementComponent>();

	for (auto et : view)
	{
		MovementComponent& movement = view.get<MovementComponent>(et);
		PlayerInputComponent& input = view.get<PlayerInputComponent>(et);

		bool up, down, left, right,space;
		up = input.buttonMap["UP"];
		down = input.buttonMap["DOWN"];
		left = input.buttonMap["LEFT"];
		right = input.buttonMap["RIGHT"];
		space = input.buttonMap["SPACE"];

        movement.velocity.x = 0;
        movement.velocity.y = 0;

		input.movement_input.x = 0;
		input.movement_input.y = 0;

		if (up) {
			input.movement_input.y += 1;
		}
        if (down) {
			input.movement_input.y -= 1;
        }if (left) {
			input.movement_input.x -= 1;
        }if (right) {
			input.movement_input.x += 1;
        }

		input.movement_input = input.movement_input *(  space ? 0.3f : 1.f);
	}


	for (auto et : view)
	{
		MovementComponent &movement = view.get<MovementComponent>(et);
		PlayerInputComponent &input = view.get<PlayerInputComponent>(et);

		movement.velocity.x = input.movement_input.x * 300;
		movement.velocity.y = input.movement_input.y * 300;
	}
}



void process_border_collisions(entt::registry &registry) {

	//borders
	float min_x = -WINDOW_WIDTH / 2.f;
	float max_x = WINDOW_WIDTH / 2.f;

	//borders
	float min_y = -100;
	float max_y = WINDOW_HEIGHT -100;

	//clamp player to viewport ------------------
	auto playerview = registry.view<PlayerInputComponent, SpriteLocation, SDL_RenderSprite>();
	for (auto et : playerview)
	{
		SpriteLocation &location = playerview.get<SpriteLocation>(et);
		SDL_RenderSprite &sprite = playerview.get<SDL_RenderSprite>(et);
		

		//grab data from location and sprite info
		float xloc = (float)location.location.x;
		float xextent = sprite.width / 2.f;

		//left edge
		if (xloc - xextent < min_x)
		{
			location.location.x = min_x + xextent;
		}
		else if (xloc + xextent > max_x)
		{
			location.location.x = max_x - xextent;
		}
	}

	//bounce ball ------------------
	auto ballview = registry.view<Ball, SpriteLocation, SDL_RenderSprite, MovementComponent>();
	for (auto et : ballview)
	{
		SpriteLocation &location = ballview.get<SpriteLocation>(et);
		SDL_RenderSprite &sprite = ballview.get<SDL_RenderSprite>(et);
		MovementComponent&movement = ballview.get<MovementComponent>(et);
		//grab data from location and sprite info
		float xloc = location.location.x;
		float xextent = sprite.width / 2;
		float yloc = location.location.y;
		float yextent = sprite.height / 2;
		//left edge
		if (xloc - xextent < min_x)
		{
			location.location.x = min_x + xextent;
			movement.velocity.x *= -1;
		}
		//right edge
		else if (xloc + xextent > max_x)
		{
			location.location.x = max_x - xextent;
			movement.velocity.x *= -1;
		}
		//top edge		
		else if (yloc + yextent > max_y)
		{
			location.location.y = max_y - yextent;
			movement.velocity.y *= -1;
		}
		//down edge
		else if (yloc - yextent < min_y)
		{
			location.location.y = min_y + yextent;
			movement.velocity.y *= -1;
		}
	}
}
void spawn_death_particles(entt::registry& registry, entt::entity target)
{
    Vec2f center = registry.get<SpriteLocation>(target).location;
    for (int i = 0; i < 8; i++)
    {
        //ball
        auto ball_entity = registry.create();
        registry.assign<SDL_RenderSprite>(ball_entity);
        registry.assign<RenderScale>(ball_entity, Vec2f{ 1.0f,1.0f });
        registry.assign<MovementComponent>(ball_entity);
        load_sprite("../assets/sprites/element_red_diamond.png", registry.get<SDL_RenderSprite>(ball_entity));
		registry.get<SDL_RenderSprite>(ball_entity).sortlayer = 5000;
        registry.assign<SpriteLocation>(ball_entity, center);
		registry.assign<Lifetime>(ball_entity, 2.f);
		registry.assign<GameEntity>(ball_entity);
        Vec2f rot = { 100,0 };
        rot.rotate_degrees((360 / 8) * i);

        registry.get<MovementComponent>(ball_entity).velocity = rot;
    }
}

void spawn_bullets(entt::registry& registry, float deltaSeconds) 
{
    auto spawnerview = registry.view<SpriteLocation,BulletSpawner>();
	for (auto et : spawnerview)
	{
        SpriteLocation& location = spawnerview.get<SpriteLocation>(et);
        BulletSpawner& sprite = spawnerview.get<BulletSpawner>(et);

		sprite.elapsed -= deltaSeconds;
		if (sprite.elapsed < 0) {
			for (auto b : sprite.bullets) {

				Vec2f offset = b.offset;
				Vec2f velocity = b.velocity;

				offset.rotate_degrees(sprite.rotation);
				velocity.rotate_degrees(sprite.rotation);
				if (sprite.type == BulletType::PLAYER_DEFAULT) {
					build_bullet("BULLET_PLAYER_01",registry, velocity, offset + location.location,b.rotation + sprite.rotation);
									
				}
				else if (sprite.type == BulletType::BOSS_01) {
					build_bullet("BULLET_BOSS_01",registry, velocity, offset + location.location, b.rotation + sprite.rotation);
					
				}
				
			}
			
			sprite.elapsed = sprite.fireRate;
		}
	}
}

void process_ball_collisions(entt::registry &registry) {
	


	//bounce ball ------------------
	auto ballview = registry.view<Ball, SpriteLocation, SDL_RenderSprite, MovementComponent>();
	for (auto et : ballview)
	{
		SpriteLocation &location = ballview.get<SpriteLocation>(et);
		const SDL_RenderSprite &sprite = ballview.get<SDL_RenderSprite>(et);
		MovementComponent&movement = ballview.get<MovementComponent>(et);

		//grab data from location and sprite info
		const float xloc = location.location.x;
		const float xextent = sprite.width / 2;
		const float yloc = location.location.y;
		const float yextent = sprite.height / 2;


		float ball_max_x = xloc + xextent;
		float ball_min_x = xloc - xextent;
		
		float ball_max_y = yloc + yextent;
		float ball_min_y = yloc - yextent;
		
		//collide against bricks
		auto brickview = registry.view<Brick, SpriteLocation, SDL_RenderSprite>();
		for (auto brick : brickview)
		{
			const SpriteLocation &brick_location = brickview.get<SpriteLocation>(brick);
			const SDL_RenderSprite &brick_sprite = brickview.get<SDL_RenderSprite>(brick);

			float brick_min_x = brick_location.location.x - brick_sprite.width / 2;
			float brick_max_x = brick_location.location.x + brick_sprite.width / 2;
			
			float brick_min_y = brick_location.location.y - brick_sprite.height / 2;
			float brick_max_y = brick_location.location.y + brick_sprite.height / 2;
			
			bool collides = true;
			// Collision tests
			if (ball_max_x < brick_min_x || ball_min_x > brick_max_x) collides = false;
			if (ball_max_y < brick_min_y || ball_min_y > brick_max_y) collides = false;

			if (collides) {

				Vec2f diff = brick_location.location - location.location;

				float factor = (float)brick_sprite.height / (float)brick_sprite.width ;
				diff.x *= factor;
				if (abs(diff.x) < abs(diff.y))
				{
					movement.velocity.y *= -1;
				}
				else {
					movement.velocity.x *= -1;
				}

				score += brickview.get<Brick>(brick).score_value;
				registry.destroy(brick);
				
			}

		}

		//collide against player paddle
		auto playerview = registry.view<PlayerInputComponent,SpriteLocation, SDL_RenderSprite>();
		for (auto player : playerview)
		{
			
			const SpriteLocation &brick_location =playerview.get<SpriteLocation>(player);
			const SDL_RenderSprite &brick_sprite =playerview.get<SDL_RenderSprite>(player);

			float brick_min_x = brick_location.location.x - brick_sprite.width / 2;
			float brick_max_x = brick_location.location.x + brick_sprite.width / 2;

			float brick_min_y = brick_location.location.y - brick_sprite.height / 2;
			float brick_max_y = brick_location.location.y + brick_sprite.height / 2;

			bool collides = true;
			// Collision tests
			if (ball_max_x < brick_min_x || ball_min_x > brick_max_x) collides = false;
			if (ball_max_y < brick_min_y || ball_min_y > brick_max_y) collides = false;

			if (collides) {
				float deltax = location.location.x - brick_location.location.x;

				deltax /= ((float)brick_sprite.width);

				movement.velocity.x = sin(deltax) * 300;
				movement.velocity.y = cos(deltax) * 300;

				printf("deltax: %f", deltax);
				//registry.destroy(brick);
			}
		}
	}
}

uint32_t build_brick(entt::registry &registry, Vec2f location, int type) {
	
	//ball
	auto brick = registry.create();
	std::string sprite;
	int score = 1;
	switch (type)
	{
	case 0:
		sprite = "../assets/sprites/element_yellow_rectangle.png";
		score = 2;
		registry.assign<PlayAnimComponent>(brick, "wogol_run_anim");
		break;
	case 1:
		sprite = "../assets/sprites/element_red_rectangle.png";
		registry.assign<PlayAnimComponent>(brick, "knight_m_run_anim");
		score = 5;
		break;
	case 2:
		sprite = "../assets/sprites/element_purple_rectangle.png";
		registry.assign<PlayAnimComponent>(brick, "elf_m_run_anim");
		score = 3;
		break;

	case 3:
		sprite = "../assets/sprites/element_grey_rectangle.png";
		registry.assign<PlayAnimComponent>(brick, "necromancer_run_anim");
		score = 1;
		break;
	default:

		sprite = "../assets/sprites/element_blue_rectangle.png";
		registry.assign<PlayAnimComponent>(brick, "masked_orc_run_anim");
		score = 4;
		break;
	}


	//ball
	//auto brick = registry.create();
	registry.assign<SDL_RenderSprite>(brick);
	registry.assign<Brick>(brick);
	registry.get<Brick>(brick).score_value = score;
	registry.assign<SpriteLocation>(brick, location);
	registry.assign<RenderScale>(brick, Vec2f{ 1.0f,1.0f });
	load_sprite(sprite, registry.get<SDL_RenderSprite>(brick));
	return brick;
}

void collide_bullets(entt::registry& registry) {
    //bullets
    auto bulletview = registry.view<Bullet, SpriteLocation>();
    for (auto et : bulletview)
    {
        SpriteLocation& location = bulletview.get<SpriteLocation>(et);
        if (location.location.y > 1000 || location.location.y < -1000) {
            registry.destroy(et);
        }
    }

	struct BossCollider {
		Vec2f center;
		float radius;
		entt::entity et;
	};

	struct PlayerCollider {
		Vec2f center;
		float radius;
		entt::entity et;
	};

	std::vector<BossCollider> bosses;
	std::vector<PlayerCollider> players;

    auto playerview = registry.view<PlayerInputComponent, SpriteLocation, SphereCollider>();
	for (auto et : playerview)
	{
        PlayerCollider pc;
        pc.center = playerview.get<SpriteLocation>(et).location;
		pc.center += playerview.get<SphereCollider>(et).centerOffset;

		pc.radius = playerview.get<SphereCollider>(et).radius;
		pc.et = et;
		players.push_back(pc);
	}

    auto bossview = registry.view<BossMovementComponent, SpriteLocation, SphereCollider>();
    for (auto et : bossview)
    {
		BossCollider pc;
        pc.center = bossview.get<SpriteLocation>(et).location;
        pc.center += bossview.get<SphereCollider>(et).centerOffset;

        pc.radius = bossview.get<SphereCollider>(et).radius;
		pc.et = et;
		bosses.push_back(pc);

		for (auto player : players) {

            Vec2f diff = pc.center - player.center;

            float dist = diff.lenght();
			if (dist < pc.radius + player.radius)
			{
                spawn_death_particles(registry, player.et);

                registry.destroy(player.et);
				return;
			}
		}
    }

    //bullet colliders
    auto bulletview2 = registry.view<Bullet, SpriteLocation, SphereCollider>();
    for (auto et : bulletview2)
    {

		Bullet& bullt = bulletview2.get<Bullet>(et);
        SpriteLocation& location = bulletview2.get<SpriteLocation>(et);
		SphereCollider& collider = bulletview2.get<SphereCollider>(et);

		Vec2f center = location.location + collider.centerOffset;
		float radius = collider.radius;

		if (bullt.bHitBoss) {
			for (auto c : bosses) {

				Vec2f diff = center - c.center;

				float dist = diff.lenght();
				if (dist < radius + c.radius) {
					registry.destroy(et);
					
					registry.get<Health>(c.et).hp -= 1;
					if (registry.get<Health>(c.et).hp < 0)
					{
						registry.destroy(c.et);
					}
					break;
				}
			}
		}
		if (bullt.bHitPlayer) {
            for (auto c : players) {

                Vec2f diff = center - c.center;

                float dist = diff.lenght();
                if (dist < radius + c.radius) {
					spawn_death_particles(registry, c.et);
					
					registry.destroy(et);
					
					registry.destroy(c.et);

                    break;
                }
            }
		}
    }


    return;
}



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

		registry.get<SphereCollider>(player_entity).radius = 10;

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



void init_game(entt::registry& main_registry, EntityDatabase& db)
{
	//initialize player
	auto player_entity = main_registry.create();


	apply_template("PLAYER", db, main_registry, player_entity);
	main_registry.assign<PlayerInputComponent>(player_entity);
	main_registry.assign<SpriteLocation>(player_entity, 0.0f, 0.0f);
	main_registry.assign<Editable>(player_entity, "player");
	//initialize boss
	auto boss_entity = main_registry.create();
	main_registry.assign<SpriteLocation>(boss_entity, 0.0f, 700.0f);

	apply_template("BOSS_01", db, main_registry, boss_entity);
	main_registry.assign<Editable>(boss_entity, "boss");
}

void process_boss_AI(entt::registry &registry, float deltaTime)
{
    auto bossinit = registry.view<BossAIName>();

	for (auto et : bossinit) {
		std::string bossname = bossinit.get(et).bossName;
		
		//init AI if it isnt there
		if (!registry.has<BossAI>(et))
		{
			if (bossname.compare("AI_BOSS_01") == 0)
			{
				std::unique_ptr<BOSS_01> boss = std::make_unique<BOSS_01>();
				boss->init(&registry, et);

				registry.assign<BossAI>(et,std::move( boss));
			}
		}
		//tick AI
		if (registry.has<BossAI>(et))
		{
			registry.get<BossAI>(et).impl->update(deltaTime);
		}
	}
}

void update_lifetimes(entt::registry& registry, float deltaTime)
{
    auto view = registry.view<Lifetime>();

	for (auto et : view) {
		view.get(et).timeLeft -= deltaTime;
		if (view.get(et).timeLeft < 0) {
			registry.destroy(et);
		}
	}
}


int main(int argc, char* argv[])
{
	auto main_registry = entt::registry{};
	
	initialize_sdl();

	main_registry.set<EntityDatabase>();
	EntityDatabase& db = main_registry.ctx<EntityDatabase>();
	create_default_templates(db);

	init_game(main_registry, db);

	

	BitFont kenney_font;
	load_font(kenney_font,"../assets/font/kenney_numbers.png", "../assets/font/kenney_numbers.fnt");

	main_registry.set<AnimationDatabase>();
	main_registry.ctx<AnimationDatabase>().load_animations_from_tileinfo("../assets/sprites/rogelike_tilelist.txt", "../assets/sprites/dungeontileset.png");
	//borders
	const float bricks_min_x = -WINDOW_WIDTH / 2.f +50;
	const float bricks_max_x = WINDOW_WIDTH / 2.f -50;

	//borders
	const float bricks_min_y = 300;
	const float bricks_max_y = WINDOW_HEIGHT - 150;
	 
	const int ny = 8;
	const int nx = 8;


	bool quit = false;
	SDL_Event e;
	//While application is running
	main_registry.set<EngineGlobalData>();
	auto start = std::chrono::system_clock::now();
	auto end = std::chrono::system_clock::now();
	bool wants_reset = false;
	double gametime = 0;
	while (!quit)
	{
        end = std::chrono::system_clock::now();
        auto elapsed =
            std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
		start = std::chrono::system_clock::now();
		main_registry.ctx<EngineGlobalData>().deltaTime = double(elapsed.count()) / 1000000000.f;
		//std::cout << "timing is " << double(elapsed.count()) / 1000000.f << std::endl;

		
		start_frame(main_registry);

		auto player_entity = main_registry.view<PlayerInputComponent>().data()[0];

		entt::entity boss{0};
        for (auto et : main_registry.view<BossAI, Health>()) {
            boss = et;
        }

		if (main_registry.valid(player_entity) && main_registry.valid(boss)) {
			gametime += double(elapsed.count()) / 1000000000.f;
		}

		//Handle events on queue
		while (SDL_PollEvent(&e) != 0)
        {
			process_imgui_event(&e);
			//User requests quit
			if (e.type == SDL_QUIT)
			{
				quit = true;
            }
            else
			{
				//If a key was pressed
				if (e.type == SDL_KEYDOWN)
				{
					if (main_registry.valid(player_entity))
					{
						//Adjust the velocity
						switch (e.key.keysym.sym)
						{
						case SDLK_UP:
							main_registry.get<PlayerInputComponent>(player_entity).buttonMap["UP"] = true;
							break;
						case SDLK_DOWN:
							main_registry.get<PlayerInputComponent>(player_entity).buttonMap["DOWN"] = true;
							break;
						case SDLK_LEFT:
							main_registry.get<PlayerInputComponent>(player_entity).buttonMap["LEFT"] = true;
							break;
						case SDLK_RIGHT:
							main_registry.get<PlayerInputComponent>(player_entity).buttonMap["RIGHT"] = true;
							break;
						case SDLK_SPACE:
							main_registry.get<PlayerInputComponent>(player_entity).buttonMap["SPACE"] = true;
							break;
						case SDLK_w:
							main_registry.get<PlayerInputComponent>(player_entity).buttonMap["UP"] = true;
							break;
						case SDLK_s:
							main_registry.get<PlayerInputComponent>(player_entity).buttonMap["DOWN"] = true;
							break;
						case SDLK_a:
							main_registry.get<PlayerInputComponent>(player_entity).buttonMap["LEFT"] = true;
							break;
						case SDLK_d:
							main_registry.get<PlayerInputComponent>(player_entity).buttonMap["RIGHT"] = true;
							break;
						}
					}

					if (e.key.keysym.sym == SDLK_r)
					{
						wants_reset = true;
					}
					
				}
				else if (e.type == SDL_KEYUP)
				{
					if (main_registry.valid(player_entity))
					{
						//Adjust the velocity
						switch (e.key.keysym.sym)
						{

						case SDLK_UP:
							main_registry.get<PlayerInputComponent>(player_entity).buttonMap["UP"] = false;
							break;
						case SDLK_DOWN:
							main_registry.get<PlayerInputComponent>(player_entity).buttonMap["DOWN"] = false;
							break;
						case SDLK_LEFT:
							main_registry.get<PlayerInputComponent>(player_entity).buttonMap["LEFT"] = false;
							break;
						case SDLK_RIGHT:
							main_registry.get<PlayerInputComponent>(player_entity).buttonMap["RIGHT"] = false;
							break; //k;
						case SDLK_SPACE:
							main_registry.get<PlayerInputComponent>(player_entity).buttonMap["SPACE"] = false;
							break;

						case SDLK_w:
							main_registry.get<PlayerInputComponent>(player_entity).buttonMap["UP"] = false;
							break;
						case SDLK_s:
							main_registry.get<PlayerInputComponent>(player_entity).buttonMap["DOWN"] = false;
							break;
						case SDLK_a:
							main_registry.get<PlayerInputComponent>(player_entity).buttonMap["LEFT"] = false;
							break;
						case SDLK_d:
							main_registry.get<PlayerInputComponent>(player_entity).buttonMap["RIGHT"] = false;
							break; //k;
						}
					}
				}
			}
		}

		if (wants_reset) {

			auto bulletview2 = main_registry.view<GameEntity>();
			for (auto e : bulletview2) {
				main_registry.destroy(e);
			}
			init_game(main_registry, db);
			wants_reset = false;
			gametime = 0;
		}

		float deltaTime = main_registry.ctx<EngineGlobalData>().deltaTime * main_registry.ctx<EngineGlobalData>().timeDilation;

		process_player_movement(main_registry);
		process_boss_movement(main_registry, deltaTime);
		process_boss_AI(main_registry, deltaTime);


		spawn_bullets(main_registry, deltaTime);
		move_objects(main_registry, deltaTime);

		collide_bullets(main_registry);
		//process_ball_collisions(main_registry);
		//process_border_collisions(main_registry);


		update_animations(main_registry);
		update_lifetimes(main_registry, deltaTime);

		transform_sprites(main_registry);

        update_tilemap(main_registry);
        draw_sprites_sdl(main_registry);
		draw_screen();
		//entt::entity boss;
		for (auto et : main_registry.view<BossAI,Health>()) {
			boss = et;
		}

		if (main_registry.valid(boss)) {
			
			int percent = main_registry.get<Health>(boss).getPercent();
			draw_string(kenney_font, "boss hp:" + std::to_string(percent), Vec2i{ 10,650 });
		}
		else {
			draw_string(kenney_font, "you are winner", Vec2i{ 500,WINDOW_HEIGHT/2 });
		}
        
		if (!main_registry.valid(player_entity))
		{
			draw_string(kenney_font, "you died", Vec2i{ 500,WINDOW_HEIGHT / 2 });
			draw_string(kenney_font, "press r to restart", Vec2i{ 500,(WINDOW_HEIGHT / 2)+100 });
		}

		draw_string(kenney_font, "time:" + std::to_string(gametime), Vec2i{10,750});
		
		
		
		//draw_ui(main_registry);

		end_frame(main_registry);
		
	}

	destroy_sdl();
	

	return 0;
}