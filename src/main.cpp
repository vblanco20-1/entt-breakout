#include <SDL.h>

#include "entt/entt.hpp"
#include "components.h"
#include "sdl_render.h"
#include "vmath.h"
#include "bitfont.h"
#include "animation.h"
#include "tilemap.h"

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

entt::entity build_player_bullet(entt::registry& registry, Vec2f velocity, Vec2f location)
{
    //ball
    auto ball_entity = registry.create();
    registry.assign<SDL_RenderSprite>(ball_entity);
    registry.assign<Bullet>(ball_entity);
    registry.assign<PlayAnimComponent>(ball_entity, "tiny_zombie_run_anim");
    registry.assign<RenderScale>(ball_entity, Vec2f{ 1.0f,1.0f });
    registry.assign<SpriteLocation>(ball_entity, location);
    registry.assign<MovementComponent>(ball_entity);
    load_sprite("../assets/sprites/ballGrey.png", registry.get<SDL_RenderSprite>(ball_entity));
	registry.get<MovementComponent>(ball_entity).velocity = velocity;

	return ball_entity;
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
				build_player_bullet(registry, b.velocity, b.offset+ location.location);
			}
			
			sprite.elapsed = sprite.fireRate;
		}
	}
}

void process_ball_collisions(entt::registry &registry) {
	//bullets
    auto bulletview = registry.view<Bullet, SpriteLocation, MovementComponent>();
	for (auto et : bulletview)
    {
        SpriteLocation& location = bulletview.get<SpriteLocation>(et);
		if (location.location.y > 1000) {
			registry.destroy(et);
		}
	}


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

int main(int argc, char *argv[])
{
	auto main_registry = entt::registry{};
	
	initialize_sdl();

	//initialize player
	auto player_entity = main_registry.create();
	main_registry.assign<SDL_RenderSprite>(player_entity);
	main_registry.assign<PlayerInputComponent>(player_entity);
	main_registry.assign<SpriteLocation>(player_entity,0.0f,0.0f);
	main_registry.assign<MovementComponent>(player_entity);
	main_registry.assign<BulletSpawner>(player_entity);
	main_registry.assign<RenderScale>(player_entity, Vec2f{ 0.3f,1.0f });
	load_sprite("../assets/sprites/paddleBlu.png", main_registry.get<SDL_RenderSprite>(player_entity));

	BulletSpawner& bspawner =  main_registry.get<BulletSpawner>(player_entity);

	BulletData b0;
	b0.velocity = { 0,600.f };
	b0.offset = { 0,0 };

    BulletData b1;
    b1.velocity = { 100,600.f };
    b1.offset = { 0,0 };

    BulletData b2;
    b2.velocity = { -100,600.f };
    b2.offset = { 0,0 };

	bspawner.bullets.push_back(b0);
	bspawner.bullets.push_back(b1);
	bspawner.bullets.push_back(b2);
	////ball
	//auto ball_entity = main_registry.create();
	//main_registry.assign<SDL_RenderSprite>(ball_entity);
	//main_registry.assign<Bullet>(ball_entity);
	//main_registry.assign<PlayAnimComponent>(ball_entity, "tiny_zombie_run_anim");
	//main_registry.assign<RenderScale>(ball_entity, Vec2f{ 5.0f,5.0f });
	//main_registry.assign<SpriteLocation>(ball_entity, 0.0f, 100.0f);
	//main_registry.assign<MovementComponent>(ball_entity);
	//load_sprite("../assets/sprites/ballGrey.png", main_registry.get<SDL_RenderSprite>(ball_entity));
	//main_registry.get<MovementComponent>(ball_entity).velocity = random_vector() * 200;
	//
	////ball
	//auto ball_entity2 = main_registry.create();
	//main_registry.assign<SDL_RenderSprite>(ball_entity2);
	//main_registry.assign<Ball>(ball_entity2);
	//main_registry.assign<PlayAnimComponent>(ball_entity2, "ogre_run_anim");
	//main_registry.assign<RenderScale>(ball_entity2, Vec2f{ 5.0f,5.0f });
	//main_registry.assign<SpriteLocation>(ball_entity2, 0.0f, 100.0f);
	//main_registry.assign<MovementComponent>(ball_entity2);
	//load_sprite("../assets/sprites/ballGrey.png", main_registry.get<SDL_RenderSprite>(ball_entity2));
	//main_registry.get<MovementComponent>(ball_entity2).velocity = random_vector() * 200;


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

	//for (int y = 0; y <= ny; y+=2)
	//{
	//	for (int x = 0; x <= nx; x+=2)
	//	{
	//		const float fx = x / float(nx);
	//		const float fy = y /float( ny);
	//		Vec2f loc;
	//		loc.x = bricks_min_x + ((bricks_max_x - bricks_min_x) *fx);
	//		loc.y = bricks_min_y + ((bricks_max_y - bricks_min_y) *fy);
	//
	//		int type =( (x + y) % 7)%4;
	//		auto brick = build_brick(main_registry, loc, type);
	//
	//		main_registry.assign<TileLocation>(brick,TileLocation{x,y});
	//	}
	//}
	//
	bool quit = false;
	SDL_Event e;
	//While application is running
	while (!quit)
	{
		start_frame();
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
					//Adjust the velocity
					switch (e.key.keysym.sym)
					{
                    case SDLK_UP:
						main_registry.get<PlayerInputComponent>(player_entity).buttonMap["UP"] =  true;
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
						//case SDLK_UP:
						//	main_registry.get<PlayerInputComponent>(player_entity).movement_input.y += 1;
						//	break;
						//case SDLK_DOWN:
						//	main_registry.get<PlayerInputComponent>(player_entity).movement_input.y += -1;
						//	break;
						//case SDLK_LEFT:
						//	main_registry.get<PlayerInputComponent>(player_entity).movement_input.x += -1;
						//	break;
						//case SDLK_RIGHT:
						//	main_registry.get<PlayerInputComponent>(player_entity).movement_input.x += 1;
						//	break;
					}
				}
				else if (e.type == SDL_KEYUP)
				{
					//Adjust the velocity
					switch (e.key.keysym.sym)
					{
                        //case SDLK_UP:
                        //	main_registry.get<PlayerInputComponent>(player_entity).movement_input.y += 0;
                        //	break;
                        //case SDLK_DOWN:
                        //	main_registry.get<PlayerInputComponent>(player_entity).movement_input.y += 0;
                        //	break;
                        //case SDLK_LEFT:
                        //	main_registry.get<PlayerInputComponent>(player_entity).movement_input.x += 0;
                        //	break;
                        //case SDLK_RIGHT:
                        //	main_registry.get<PlayerInputComponent>(player_entity).movement_input.x += 0;
                        //	brea
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
					}
				}
			}
		}

		

		process_player_movement(main_registry);
		
		spawn_bullets(main_registry, 1.0f / 60.0f);
		move_objects(main_registry, 1.0f / 60.0f);

		process_ball_collisions(main_registry);
		process_border_collisions(main_registry);


		update_animations(main_registry);
		transform_sprites(main_registry);



		draw_string(kenney_font, "score:" + std::to_string( score), Vec2i{10,750});
		
		update_tilemap(main_registry);
		draw_sprites_sdl(main_registry);
		
		end_frame();
		
	}

	destroy_sdl();
	

	return 0;
}