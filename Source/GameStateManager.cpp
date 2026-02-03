#include "AEEngine.h"
#include "GameStateList.h"

#include "GameStateManager.h"

#include "Scenes/TestScene.h"

int current = 0, previous = 0, next = 0;

FP fpLoad = nullptr, fpInitialize = nullptr, fpDraw = nullptr, fpFree = nullptr, fpUnload = nullptr;
Update fpUpdate = nullptr;

void GSM_Initialize(int startingState)
{
	current = previous = next = startingState;

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
	default:
		break;
	}

}

void ChangeState(GS_STATES thing)
{
	next = thing;
}
