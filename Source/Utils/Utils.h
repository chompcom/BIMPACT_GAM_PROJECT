#pragma once
#include "AEEngine.h"
#include "../Sprite.h"
#include "Vector2.hpp"

bool AreCirclesIntersecting(Vector2 circle1_Center, f32 circle1_Radius, Vector2 circle2_Center, f32 circle2_Radius);

bool AreSquaresIntersecting(Vector2 square1_Center, float square1Length,
	Vector2 square2_Center, float square2Length);

f32 ClampInt(f32 value, f32 low, f32 high);