#include "MainMenuScene.h"

#include "AEEngine.h"

#include "../Screens/Ui.h"
#include "../GameStateManager.h"
#include "../GameStateList.h"
#include "../Loaders/DataLoader.h"

UIManager credsUI;

void CreditsLoad() {

	credsUI.UseWindowAsRootRect();
	credsUI.LoadFromFile("Assets/UI/credits.json");

}
void CreditsInit() {
	// nothing
}


constexpr float speed = 0.25f;

void CreditsUpdate(float dt = AEFrameRateControllerGetFrameTime()) {

	// SCROLL HEIAN ERA TECHNIQUE 
	credsUI.FindById("creds1")->parent->localPos.y += (speed * dt);
	credsUI.Update();

	// ESCAPE goes back to GS_MAINMENU
	if (AEInputCheckTriggered(AEVK_ESCAPE)) ChangeState(GS_MAINMENU);
}


void CreditsDraw() {
	credsUI.Draw();
}

void CreditsFree() {
	// Nothing
}

void CreditsUnload() {
	DataLoader::Unload();
}