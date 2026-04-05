/* Start Header ************************************************************************/
/*!
\file     Tutorial.cpp
\author   Lee Hng Hwee Celest, hnghweecelest.l, 2502385
\par      hnghweecelest.l@digipen.edu
\brief    This file defines some functions that implement the tutorial using MJ's UI 
          manager

Copyright (C) 2026 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited.
*/
/* End Header **************************************************************************/
#include "AEEngine.h"
#include "../Screens/Ui.h"
#include "../GameStateManager.h"
#include "../GameStateList.h"
#include "../Loaders/DataLoader.h"

//anonymous namespace for variables that will be needed in this file
namespace
{
	//variable to store the UI manager
	UIManager tutorialUi;
	bool showTutorial{true};
}

//Initialise the tutorial's UIManager variables 
void InitTutorial()
{
	tutorialUi.SetFont(AEGfxCreateFont("Assets/Kenney Pixel.ttf", 48));
	tutorialUi.UseWindowAsRootRect();
	tutorialUi.LoadFromFile("Assets/UI/tutorial.json"); // Load menu
}

//Renders the tutorial UI
void RenderTutorial()
{
	if (showTutorial)
	{
		tutorialUi.Update();
		tutorialUi.Draw();
	}
}

//Handles the tutorial input
void TutorialInput()
{
	//if the player tries to move or if they left click, close the tutorial
	if (AEInputCheckTriggered(AEVK_LBUTTON) || 
	AEInputCheckTriggered(AEVK_W) || AEInputCheckTriggered(AEVK_A) || 
	AEInputCheckTriggered(AEVK_S) || AEInputCheckTriggered(AEVK_D))
	{
		showTutorial = false;
	}
}

//clear the tutorial's UI mananger
void TutorialUnload()
{
	tutorialUi.Clear();
}
