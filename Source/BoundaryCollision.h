/* Start Header ************************************************************************/
/*!
\file       BoundaryCollision.h
\author     Yee Kiat Lim, yeekiat.lim, 2503993
\par        yeekiat.lim@digipen.edu
\brief		This file implements interfaces to functions that handle boundary collision
			between objects and the viewscreen.

Copyright (C) 2026 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited.
*/
/* End Header **************************************************************************/

#pragma once
#include "AEEngine.h"
#include "Utils/Vector2.hpp"

bool CollisionBoundaryStatic(Vector2& position, Vector2 scale, s32 windowLength, s32 windowHeight);
bool CollisionBoundaryStatic(Vector2& position, Vector2 scale, s32 windowLength, s32 windowHeight, int& collisionFlags);
