#pragma once
#include "AEEngine.h"
#include "Utils/Vector2.hpp"

struct AABB
{
	Vector2	min;
	Vector2	max;
};

bool CollisionIntersection_RectRect_Static(const AABB& aabb1,					//Input
	const AABB& aabb2);					//Input


bool CollisionIntersection_RectRect_Dynamic(const AABB& aabb1,					//Input
	const Vector2& vel1,					//Input 
	const AABB& aabb2,					//Input 
	const Vector2& vel2,					//Input
	float& firstTimeOfCollision);		//Output: the calculated value of tFirst

// ----------------------------------------------------------------------------
//
// Collision test between 2 rectangles using AABB-AABB collision algorithm.
// First check "Step 1", is the static intersection check => return intersection 
// in case both rectangles are already overlapping.
// Second check "Step 2 until 5", in case the first check fails, we apply a more 
// dynamic collision test to see if any rectangle's movement may lead to intersection 
// in this game loop.
// returns true if there is intersection.
//
// ----------------------------------------------------------------------------
inline bool CollisionIntersection_RectRect(const Vector2& pos1,					//Input
	const Vector2& scale1,					//Input 
	const Vector2& vel1,					//Input 
	const Vector2& pos2,					//Input 
	const Vector2& scale2,					//Input 
	const Vector2& vel2,					//Input
	float& firstTimeOfCollision)		//Output: return the calculated value of tFirst
{
	AABB aabb1{ pos1 - scale1 * 0.5f, pos1 + scale1 * 0.5f };
	AABB aabb2{ pos2 - scale2 * 0.5f, pos2 + scale2 * 0.5f };

	//Step 1
	bool staticCollision = false;
	staticCollision = CollisionIntersection_RectRect_Static(aabb1, aabb2);
	if (staticCollision)
	{
		return true;
	}

	//Step 2 until 5
	return CollisionIntersection_RectRect_Dynamic(aabb1, vel1, aabb2, vel2, firstTimeOfCollision);
}