/* Start Header ************************************************************************/
/*!
\file       GameStateList.h
\author     Yee Kiat Lim, yeekiat.lim, 2503993
\par        yeekiat.lim@digipen.edu
\brief		This file implements the various game states and level states of the game.

Copyright (C) 2026 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited.
*/
/* End Header **************************************************************************/

#pragma once
#include "AEEngine.h"

enum GS_STATES
{
	GS_LEVEL1 = 0,
	
	GS_LOGO,
	GS_QUIT,
	GS_RESTART,

	GS_MAINMENU,
	GS_CONTENT_EDIT,

	GS_CREDITS
};

enum LV_STATES
{
	RUNNING = 0,
	PAUSED,

	WIN,
	LOSE
};