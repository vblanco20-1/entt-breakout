#include <SDL.h>
#include <cmath>


struct Vec2f {
	float x{0.f};
	float y{0.f};

	constexpr Vec2f operator+(const Vec2f& other)const
	{
		Vec2f v{};
		v.x = x + other.x;
		v.y = y + other.y;
		return v;
	}
	constexpr Vec2f operator-(const Vec2f& other)const
	{
		Vec2f v{};
		v.x = x - other.x;
		v.y = y - other.y;
		return v;
	}
	Vec2f &operator+=(const Vec2f& other)
	{
		x = x + other.x;
		y = y + other.y;
		return *this;
	}
	Vec2f &operator-=(const Vec2f& other)
	{
		x = x - other.x;
		y = y - other.y;
		return *this;
	}

	float lenght() const {
		return sqrt(x*x+ y*y);
	}

	Vec2f normalized() {
		float l = lenght();

		return Vec2f{ x / l, y / l };
	}
	constexpr Vec2f& operator*(float other)const
	{
		Vec2f v{};
		v.x =x * other;
		v.y =y * other;
		return v;
	}
};

struct Vec2i {

	int x{0};
	int y{0};

	constexpr Vec2i operator+(const Vec2i& other)const
	{
		Vec2i v{};
		v.x = x + other.x;
		v.y = y + other.y;
		return v;
	}
	
	constexpr Vec2i operator-(const Vec2i& other)const
	{
		Vec2i v{};
		v.x = x - other.x;
		v.y = y - other.y;
		return v;
	}
	Vec2i &operator+=(const Vec2i& other)
	{
		x = x + other.x;
		y = y + other.y;
		return *this;
	}
	Vec2i &operator-=(const Vec2i& other)
	{
		x = x - other.x;
		y = y - other.y;
		return *this;
	}
	constexpr Vec2i& operator*(float other)const
	{
		Vec2i v{};
		v.x = int(float(x) * other);
		v.y = int(float(y) * other);
		return v;
	}
};




struct SDL_RenderSprite {

	Vec2i location;
	int width;
	int height;	

	SDL_Rect texture_rect;
	SDL_Texture* texture{ nullptr };
	bool use_custom_texture_rect{false};
	SDL_Rect to_rect() const {
		return SDL_Rect{ location.x - width / 2, location.y - height / 2, width, height };
	}
};
struct SpriteLocation{
	Vec2f location;
};
struct Ball {
	int num_bounces;
};
struct Brick {
	int hits_left;
};
struct MovementComponent {
	Vec2f acceleration;
	Vec2f velocity;
	Vec2f max_velocity;
};

struct PlayerInputComponent {
	Vec2f movement_input;
};