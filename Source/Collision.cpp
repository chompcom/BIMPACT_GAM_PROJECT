#include "AEEngine.h"
#include "Collision.h"

// ----------------------------------------------------------------------------
//
// Static AABB collision:
// AABB collision between two rectangles that are possibly already overlapping
// This is a rigid check, considering both rectangles are not moving
// If this function finds intersection between the two rectangles, there will be
// no need to call the dynamic collision check.
// return true if there is overlap.
//
// ----------------------------------------------------------------------------
bool CollisionIntersection_RectRect_Static(const AABB& aabb1, const AABB& aabb2) {
	if (aabb1.max.x < aabb2.min.x || aabb1.max.y < aabb2.min.y ||
		aabb1.min.x > aabb2.max.x || aabb2.min.y > aabb2.max.y) {
		return false;
	}
	else {
		return true;
	}
}

// ----------------------------------------------------------------------------
//
// Dynamic AABB collision:
// AABB collision between two rectangles, that we already know they are not 
// overlapping.
// Both rectangles may have velocities and the function must return the first
// time of intersection if there is any.
// return true if there is intersection.
//
// ----------------------------------------------------------------------------
float tFirst = 0.0f;
float tLast = 0.0f;
bool CollisionIntersection_RectRect_Dynamic(const AABB& aabb1, const Vector2& vel1,	const AABB& aabb2, const Vector2& vel2,	float& firstTimeOfCollision) {
	tFirst = 0.0f;
	tLast = (float)AEFrameRateControllerGetFrameTime();;

	Vector2 Vb = vel1 - vel2;

	if (Vb.x < 0) {
		if (aabb1.min.x > aabb2.max.x) {
			return false;
		}
		if (aabb1.max.x < aabb2.min.x) {
			tFirst = max((aabb1.max.x - aabb2.min.x) / Vb.x, tFirst);
		}
		if (aabb1.min.x < aabb2.max.x) {
			tLast = max((aabb1.min.x - aabb2.max.x) / Vb.x, tLast);
		}
	}
	else if (Vb.x > 0) {
		if (aabb1.max.x < aabb2.min.x) {
			return false;
		}
		if (aabb1.min.x > aabb2.max.x) {
			tFirst = max((aabb1.min.x - aabb2.max.x) / Vb.x, tFirst);
		}
		if (aabb1.max.x > aabb2.min.x) {
			tLast = max((aabb1.max.x - aabb2.min.x) / Vb.x, tLast);
		}
	}
	else {
		if (aabb1.max.x < aabb2.min.x || aabb1.min.x > aabb2.max.x) {
			return false;
		}
	}

	if (tFirst > tLast) {
		return false;
	}


	if (Vb.y < 0) {
		if (aabb1.min.y > aabb2.max.y) {
			return false;
		}
		if (aabb1.max.y < aabb2.min.y) {
			tFirst = max((aabb1.max.y - aabb2.min.y) / Vb.y, tFirst);
		}
		if (aabb1.min.y < aabb2.max.y) {
			tLast = max((aabb1.min.y - aabb2.max.y) / Vb.y, tLast);
		}
	}
	else if (Vb.y > 0) {
		if (aabb1.max.y < aabb2.min.y) {
			return false;
		}
		if (aabb1.min.y > aabb2.max.y) {
			tFirst = max((aabb1.min.y - aabb2.max.y) / Vb.y, tFirst);
		}
		if (aabb1.max.y > aabb2.min.y) {
			tLast = max((aabb1.max.y - aabb2.min.y) / Vb.y, tLast);
		}
	}
	else {
		if (aabb1.max.y < aabb2.min.y || aabb1.min.y > aabb2.max.y) {
			return false;
		}
	}

	if (tFirst > tLast) {
		return false;
	}


	firstTimeOfCollision = tFirst;

	return true;
}