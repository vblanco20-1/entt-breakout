#pragma once
#include "vmath.h"




struct SpriteLocation{
	Vec2f location;
};
struct RenderScale {
	Vec2f scale;
};
struct Ball {
	int num_bounces;
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

struct PlayerInputComponent {
	Vec2f movement_input;
};