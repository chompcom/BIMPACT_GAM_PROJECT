#pragma once
#include "AEEngine.h"
#include "Utils/Vector2.hpp"
//#include "Collision.h"

bool CollisionBoundary_Static(Vector2& position, Vector2 scale, s32 windowLength, s32 windowHeight);

//bool CollisionBoundary_Static(const AABB& boundingBox, s32 windowLength, s32 windowHeight);

//bool CollisionBoundary_Dynamic(const AABB& aabb, Vector2 vel);