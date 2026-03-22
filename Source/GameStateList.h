#pragma once
#include "AEEngine.h"

enum GS_STATES
{
	GS_LEVEL1 = 0,
	GS_LEVEL2,
	
	GS_QUIT,
	GS_RESTART,

	GS_MAINMENU,
	GS_CONTENT_EDIT
};

enum LV_STATES
{
	RUNNING = 0,
	PAUSED,

	WIN,
	LOSE
};