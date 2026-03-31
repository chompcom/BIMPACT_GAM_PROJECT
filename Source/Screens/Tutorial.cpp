// Mainmenu UI Test

#include "MainMenuScene.h"

#include "AEEngine.h"
#include "../Screens/Ui.h"
#include "../GameStateManager.h"
#include "../GameStateList.h"

namespace
{
	UIManager tutorialUi;
	bool showTutorial{true};
}

void InitTutorial()
{
	// Font
	//uiFont = AEGfxCreateFont("Assets/Kenney Pixel.ttf", 48);
	//ui.SetFont(font);

	

	InitMenuAudio();
	// Font
	//uiFont = AEGfxCreateFont("Assets/Kenney Pixel.ttf", 48);
	tutorialUi.SetFont(AEGfxCreateFont("Assets/Kenney Pixel.ttf", 48));

	// Use full window as UI root
	tutorialUi.UseWindowAsRootRect();
	//system("cd");
	tutorialUi.LoadFromFile("Assets/UI/tutorial.json"); // Load menu
}

void RenderTutorial()
{
	if (showTutorial)
	{
		std::cout << "Harikitte kou!!";
		tutorialUi.Update();
		tutorialUi.Draw();
	}
}

void TutorialInput()
{
	if (AEInputCheckTriggered(AEVK_LBUTTON) || 
	AEInputCheckTriggered(AEVK_W) || AEInputCheckTriggered(AEVK_A) || 
	AEInputCheckTriggered(AEVK_S) || AEInputCheckTriggered(AEVK_D))
	{
		showTutorial = false;
	}
}

void TutorialUnload()
{
	tutorialUi.Clear();
}
