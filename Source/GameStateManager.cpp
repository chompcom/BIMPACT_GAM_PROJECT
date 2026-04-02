#include "AEEngine.h"
#include "GameStateList.h"

#include "GameStateManager.h"

#include "Scenes/TestScene.h"
#include "Screens/MainMenuScene.h"
#include "Screens/ContentEditorScene.h"
#include "Screens/Credits.h"

int current = 0, previous = 0, next = 0;

FP fpLoad = nullptr, fpInitialize = nullptr, fpDraw = nullptr, fpFree = nullptr, fpUnload = nullptr;
Update fpUpdate = nullptr;

void GSM_Initialize(int startingState)
{
	current = previous = next = startingState;
	//DataLoader::Load();
	//InitAudio();
	//some additional code
	printf("GSM:Initialize\n");
}

void GSM_Update()
{
	//some unfinished code here
	printf("GSM:Update\n");

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
	case GS_LEVEL2:
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
