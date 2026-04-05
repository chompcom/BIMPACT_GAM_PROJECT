/* Start Header ************************************************************************/
/*!
\file       GameStateManager.h
\author     Yee Kiat Lim, yeekiat.lim, 2503993
\par        yeekiat.lim@digipen.edu
\brief		This file implements the necessary functionality for bosses to exist (Boss
			behaviour is handled by FSM).

Copyright (C) 2026 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited.
*/
/* End Header **************************************************************************/

#pragma once
#include "AEEngine.h"
#include "GameStateList.h"

typedef void(*FP)();

using Update = void(*)(float);

extern int current, previous, next;

extern FP fpLoad, fpInitialize, fpDraw, fpFree, fpUnload;

extern Update fpUpdate;

void GSM_Initialize(int startingState);
void GSM_Update();
void ChangeState(GS_STATES thing);