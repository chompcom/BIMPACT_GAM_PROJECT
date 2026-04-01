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


constexpr float speed = 0.10f;
float timer = 20.0f, counter = 0;
void CreditsUpdate(float dt = AEFrameRateControllerGetFrameTime()) {
	if(AEInputCheckTriggered(AEVK_ESCAPE)) ChangeState(GS_MAINMENU);
	// SCROLL HEIAN ERA TECHNIQUE
	credsUI.FindById("creds1")->localPos.y += (speed * dt);
	credsUI.FindById("creds2")->localPos.y += (speed * dt);
	credsUI.FindById("creds3")->localPos.y += (speed * dt);
	//std::cout << credsUI.FindById("creds3")->localPos.y << std::endl;
	if (credsUI.FindById("creds3")->localPos.y > 0) {
		counter += dt;
		if (counter >= timer) {
			ChangeState(GS_MAINMENU);
		}
		return;
	}
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