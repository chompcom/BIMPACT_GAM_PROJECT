#include "MainMenuScene.h"

#include "AEEngine.h"
#include "../Screens/Ui.h"
#include "../GameStateManager.h"
#include "../GameStateList.h"
#include "../Loaders/DataLoader.h"

namespace
{
	UIManager ui;
	UIManager confirmUi;
	s8 uiFont = 0;
	bool isQuitConfirmOpen = false;




}

void MainMenuLoad()
{
	InitMenuAudio();
	// Font
	uiFont = AEGfxCreateFont("Assets/Kenney Pixel.ttf", 48);
	ui.SetFont(uiFont);

	// Use full window as UI root
	ui.UseWindowAsRootRect();
	//system("cd");
	ui.LoadFromFile("Assets/UI/mainmenu.json"); // Load menu


	// Load confirmation windwo for quit button
	confirmUi.SetFont(uiFont);
	confirmUi.LoadFromFilePopUp("Assets/UI/confirmation_popup.json", Vector2(0.0f, 0.0f), Vector2(560.0f, 240.0f));

	UIElement* confirmMessage = confirmUi.FindById("message");
	if (confirmMessage) confirmMessage->text = "Exit the game?";

	confirmUi.BindOnClick("btn_yes", [](UIElement&)
		{
			ChangeState(GS_QUIT);
		});

	confirmUi.BindOnClick("btn_no", [](UIElement&)
		{
			isQuitConfirmOpen = false;
		});

	// Bind button actions
	ui.BindOnClick("btn_start", [](UIElement&)
		{
			ChangeState(GS_LEVEL1);
		});


	ui.BindOnClick("btn_editor", [](UIElement& self)
		{
			UNREFERENCED_PARAMETER(self);
			ChangeState(GS_CONTENT_EDIT);
		});

	ui.BindOnClick("btn_quit", [](UIElement& self)
		{
			UNREFERENCED_PARAMETER(self);
			isQuitConfirmOpen = true;
		});

	ui.BindOnClick("btn_credits", [](UIElement& self) {
		UNREFERENCED_PARAMETER(self);
		ChangeState(GS_CREDITS);
		});

	// Optional hover effects
	ui.BindOnHover("btn_start", [](UIElement& self)
		{
			self.backgroundColor = Color{ 0.6f, 0.6f, 0.6f, 1.0f };
		});

	ui.BindOnHoverExit("btn_start", [](UIElement& self)
		{
			self.backgroundColor = Color{ 0.44f, 0.44f, 0.44f, 1.0f };
		});

	ui.BindOnHover("btn_editor", [](UIElement& self)
		{
			self.backgroundColor = Color{ 0.6f, 0.6f, 0.6f, 1.0f };
		});

	ui.BindOnHoverExit("btn_editor", [](UIElement& self)
		{
			self.backgroundColor = Color{ 0.44f, 0.44f, 0.44f, 1.0f };
		});

	ui.BindOnHover("btn_credits", [](UIElement& self)
		{
			self.backgroundColor = Color{ 0.6f, 0.6f, 0.6f, 1.0f };
		});

	ui.BindOnHoverExit("btn_credits", [](UIElement& self)
		{
			self.backgroundColor = Color{ 0.44f, 0.44f, 0.44f, 1.0f };
		});

	ui.BindOnHover("btn_quit", [](UIElement& self)
		{
			self.backgroundColor = Color{ 0.6f, 0.6f, 0.6f, 1.0f };
		});

	ui.BindOnHoverExit("btn_quit", [](UIElement& self)
		{
			self.backgroundColor = Color{ 0.44f, 0.44f, 0.44f, 1.0f };
		});

	ui.BindOnClick("btn_controls", [](UIElement&) {
		//Set image controls to be visible
			ui.FindById("img_controls")->visible = !ui.FindById("img_controls")->visible;
		});

	ui.BindOnHover("btn_controls", [](UIElement& self)
		{
			self.backgroundColor = Color{ 0.6f, 0.6f, 0.6f, 1.0f };
		});

	ui.BindOnHoverExit("btn_controls", [](UIElement& self)
		{
			self.backgroundColor = Color{ 0.44f, 0.44f, 0.44f, 1.0f };
		});

}

void MainMenuInit(){
	MenuBGMAudio();
}

void MainMenuUpdate(float dt=AEFrameRateControllerGetFrameTime())
{
	UNREFERENCED_PARAMETER(dt);

	if (isQuitConfirmOpen)
	{
		if (AEInputCheckTriggered(AEVK_ESCAPE))
		{
			isQuitConfirmOpen = false;
		}

		confirmUi.Update();
		return;
	}
	if (ui.FindById("img_controls")->visible && (AEInputCheckTriggered(AEVK_LBUTTON) || AEInputCheckTriggered(AEVK_ESCAPE))) {
		ui.FindById("img_controls")->visible = false;
		//We skip updating the buttons this frame because otherwise it's possible to bring up the controls again instantly
		return;
	}
	else if (ui.FindById("img_controls")->visible) {
		//We do not want to update the buttons behind while the image is visible
		return;
	}

	ui.Update();
}

void MainMenuDraw()
{
	ui.Draw();

	if (isQuitConfirmOpen)
	{
		AEGfxSetRenderMode(AE_GFX_RM_COLOR);
		AEGfxSetBlendMode(AE_GFX_BM_BLEND);
		AEGfxSetTransparency(1.0f);

		Sprite overlay(
			DataLoader::GetOrCreateSquareMesh(),
			Vector2(0.0f, 0.0f),
			Vector2(
				AEGfxGetWinMaxX() - AEGfxGetWinMinX(),
				AEGfxGetWinMaxY() - AEGfxGetWinMinY()
			),
			Color{ 0.0f, 0.0f, 0.0f, 0.55f }
		);
		overlay.RenderSprite(true);

		confirmUi.Draw();
	}
}

void MainMenuFree(){}

void MainMenuUnload()
{
	FreeMenuAudio();

	ui.Clear();

	confirmUi.Clear();
	isQuitConfirmOpen = false;

	if (uiFont)
	{
		AEGfxDestroyFont(uiFont);
		uiFont = 0;
	}

	DataLoader::Unload();
}
