/* Start Header ************************************************************************/
/*!
\file       GameStateManager.cpp
\author     Yee Kiat Lim, yeekiat.lim, 2503993
\par        yeekiat.lim@digipen.edu
\brief		This file implements the functionality of the Game State Manager.

Copyright (C) 2026 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited.
*/
/* End Header **************************************************************************/

#include "AEEngine.h"
#include "GameStateList.h"

#include "GameStateManager.h"

#include "Scenes/TestScene.h"
#include "Screens/LogoScene.h"
#include "Screens/MainMenuScene.h"
#include "Screens/ContentEditorScene.h"
#include "Screens/Credits.h"

int current = 0, previous = 0, next = 0;

FP fpLoad = nullptr, fpInitialize = nullptr, fpDraw = nullptr, fpFree = nullptr, fpUnload = nullptr;
Update fpUpdate = nullptr;

// Initializes GSM
void GSM_Initialize(int startingState)
{
	current = previous = next = startingState;
}

// Updates function pointers of GSM to point to respective cycle functions of current scene
void GSM_Update()
{
	switch (current)
	{
	case GS_LEVEL1:
		fpLoad = TestLoad;
		fpUnload = TestUnload;
		fpInitialize = TestInit;
		fpFree = TestFree;
		fpDraw = TestDraw;
		fpUpdate = TestUpdate;
		break;
	case GS_LOGO:
		fpLoad =       LogoLoad;
		fpUnload =     LogoUnload;
		fpInitialize = LogoInit;
		fpFree =       LogoFree;
		fpDraw =       LogoDraw;
		fpUpdate =     LogoUpdate;
		break;
	case GS_RESTART:
		break;
	case GS_QUIT:
		break;
	case GS_MAINMENU:
		fpLoad = MainMenuLoad;
		fpUnload = MainMenuUnload;
		fpInitialize = MainMenuInit;
		fpFree = MainMenuFree;
		fpDraw = MainMenuDraw;
		fpUpdate = MainMenuUpdate;
		break;
	case GS_CONTENT_EDIT:
		fpLoad = ContentEditorLoad;
		fpUnload = ContentEditorUnload;
		fpInitialize = ContentEditorInit;
		fpFree = ContentEditorFree;
		fpDraw = ContentEditorDraw;
		fpUpdate = ContentEditorUpdate;
		break;
	case GS_CREDITS:
		fpLoad = CreditsLoad;
		fpUnload = CreditsUnload;
		fpInitialize = CreditsInit;
		fpFree = CreditsFree;
		fpDraw = CreditsDraw;
		fpUpdate = CreditsUpdate;
		break;
	default:
		break;
	}

}

void ChangeState(GS_STATES thing)
{
	next = thing;
}
