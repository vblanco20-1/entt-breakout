#include "vmath.h"
#include <random>

Vec2f random_vector()
 {

 static std::mt19937 rng(std::random_device{}());

 std::uniform_real_distribution<float> distribution(-1.0, 1.0);

 return Vec2f{ distribution(rng), distribution(rng) }.normalized();
}