#include "MainMenuScene.h"

#include "AEEngine.h"

#include "../Screens/Ui.h"
#include "../GameStateManager.h"
#include "../GameStateList.h"

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
	credsUI.FindById("creds1")->localPos.y += (speed * dt);
	credsUI.FindById("creds2")->localPos.y += (speed * dt);
	credsUI.FindById("creds3")->localPos.y += (speed * dt);

	credsUI.Update();
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