/* Start Header ************************************************************************/
/*!
\file       BoundaryCollision.cpp
\author     Yee Kiat Lim, yeekiat.lim, 2503993
\par        yeekiat.lim@digipen.edu
\brief		This file implements functions that check for collisions between objects and 
			the walls of the viewscreen (Technically not needed, just in case).

Copyright (C) 2026 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited.
*/
/* End Header **************************************************************************/

#include "AEEngine.h"
#include "BoundaryCollision.h"

// Checks for collision between object and boundaries of viewport (Object moving out of screen)
// If so, shifts object back to valid position
bool CollisionBoundaryStatic(Vector2& position, Vector2 scale, s32 windowLength, s32 windowHeight) {
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

// Similar to above function, but returns which side of viewscreen object is colliding with
bool CollisionBoundaryStatic(Vector2& position, Vector2 scale, s32 windowLength, s32 windowHeight, int& collisionFlags) {
	const int	COLLISION_LEFT		= 0x00000001;	//0001
	const int	COLLISION_RIGHT		= 0x00000002;	//0010
	const int	COLLISION_TOP		= 0x00000004;	//0100
	const int	COLLISION_BOTTOM	= 0x00000008;	//1000

	Vector2 min = position - scale * 0.5f;
	Vector2 max = position + scale * 0.5f;

	if (min.x < (float)(-1 * windowLength / 2)) {
		position.x = (float)(-1 * windowLength / 2) + scale.x / 2.f;
		collisionFlags |= COLLISION_LEFT;
	}
	if (max.x > (float)(windowLength / 2)) {
		position.x = (float)(windowLength / 2) - scale.x / 2.f;
		collisionFlags |= COLLISION_RIGHT;
	}
	if (min.y < (float)(-1 * windowHeight / 2)) {
		position.y = (float)(-1 * windowHeight / 2) + scale.y / 2.f;
		collisionFlags |= COLLISION_TOP;

	}
	if (max.y > (float)(windowHeight / 2)) {
		position.y = (float)(windowHeight / 2) - scale.y / 2.f;
		collisionFlags |= COLLISION_BOTTOM;
	}

	return collisionFlags != 0x00000000;
}