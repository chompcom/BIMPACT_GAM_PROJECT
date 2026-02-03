#include "AEEngine.h"
#include "BoundaryCollision.h"

/*bool CollisionBoundary_Static(const AABB& boundingBox, s32 windowLength, s32 windowHeight) {
	if ((boundingBox.min.x < (float)(-1 * windowLength / 2)) or (boundingBox.max.x > (float)(windowLength / 2)) or
		(boundingBox.min.y < (float)(-1 * windowHeight / 2)) or (boundingBox.max.y > (float)(windowHeight / 2))) {
		return true;
	}
	else {
		return false;
	}
}*/

bool CollisionBoundary_Static(Vector2& position, Vector2 scale, s32 windowLength, s32 windowHeight) {
	Vector2 min = position - scale * 0.5f;
	Vector2 max = position + scale * 0.5f;

	if (min.x < (float)(-1 * windowLength / 2)){
		position.x = (float)(-1 * windowLength / 2) + scale.x / 2.f;
		return true;
	}
	else if (max.x > (float)(windowLength / 2)) {
		position.x = (float)(windowLength / 2) - scale.x / 2.f;
		return true;
	}
	else if (min.y < (float)(-1 * windowHeight / 2)) {
		position.y = (float)(-1 * windowHeight / 2) + scale.y / 2.f;
		return true;
	}
	else if (max.y > (float)(windowHeight / 2)) {
		position.y = (float)(windowHeight / 2) - scale.y / 2.f;
		return true;
	}
	else {
		return false;
	}
}

/*bool CollisionBoundary_Dynamic(const AABB& boundingBox, s32 windowLength, s32 windowHeight, Vector2 velocity) {
	float tFirst = 0.0f;
	float tLast = (float)AEFrameRateControllerGetFrameTime();;


	if (velocity.x < 0) {
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
}*/