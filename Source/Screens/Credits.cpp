/*!***************************************************************************
\file       Credits.cpp
\author		Quah Ming Jun, m.quah
\par        m.quah@digipen.edu
\brief
			This source file defines the lifecycle functions for the Credits Scene.

Copyright (C) 2026 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited.
***************************************************************************/


#include "MainMenuScene.h"

#include "AEEngine.h"

#include "../Screens/Ui.h"
#include "../GameStateManager.h"
#include "../GameStateList.h"
#include "../Loaders/DataLoader.h"

UIManager credsUI;


// Load the UI layout for the Credits scene.
void CreditsLoad() {

	credsUI.UseWindowAsRootRect();
	credsUI.LoadFromFile("Assets/UI/credits.json");

}

// Nth to see here
void CreditsInit() {
	// nothing
}

constexpr float speed = 0.10f;
float timer = 20.0f, counter = 0;

// Update the Credits scene each frame.
void CreditsUpdate(float dt = AEFrameRateControllerGetFrameTime()) {

	// SCROLL HEIAN ERA TECHNIQUE 
	credsUI.FindById("creds1")->parent->localPos.y += (speed * dt);
	credsUI.Update();

	// ESCAPE goes back to GS_MAINMENU
	if (AEInputCheckTriggered(AEVK_ESCAPE)) ChangeState(GS_MAINMENU);
}

// Render the Credits scene.
void CreditsDraw() {
	credsUI.Draw();
}

// Nth to see here
void CreditsFree() {
	// Nothing
}

// Unload all assets used by the Credits scene.
void CreditsUnload() {
	DataLoader::Unload();
}
