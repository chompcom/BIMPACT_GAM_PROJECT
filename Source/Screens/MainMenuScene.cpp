/* Start Header ************************************************************************/
/*!
\file		MainMenuScene.cpp

\author     Yee Kiat Lim, yeekiat.lim, 2503993
\par        yeekiat.lim@digipen.edu

\author 	Hong Josiah Qin, hong.j, 2501239
\par  		hong.j@digipen.edu

\author		Lee Hng Hwee Celest, hnghweecelest.l, 2502385
\par		hnghweecelest.l@digipen.edu

\author     Brandon Choo, b.choo, 2501888
\par        b.choo@digipen.edu

\author		Ming Jun, m.quah, 2501182
\par		m.quah@digipen.edu

\brief		The Main Menu screen where the player will traverse to different scenes.
Mainly uses the UI class to display information

Copyright (C) 2026 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited.
*/
/* End Header **************************************************************************/

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

/*!
\brief
	Load all assets and UI layouts for the Main Menu scene.

\par
	Methodology:
		1. Initialize menu audio and create the font used by UI elements.
		2. Load the main menu UI from JSON and bind button callbacks.
		3. Load the quit-confirmation popup UI and configure its buttons.
		4. Set up hover effects for interactive buttons.
*/
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

/*!
\brief
	Initialize the Main Menu scene after loading.

\par
	Methodology:
		1. Start background music for the menu.
		2. Perform any additional runtime initialization if needed.
*/
void MainMenuInit(){
	MenuBGMAudio();
}

/*!
\brief
	Update the Main Menu scene each frame.

\par
	Methodology:
		1. If the quit-confirmation popup is open:
			* Allow ESC to close it.
			* Update only the popup UI.
			* Skip updating the main UI.
		2. Otherwise, update the main menu UI normally.
*/
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

/*!
\brief
	Render the Main Menu scene.

\par
	Methodology:
		1. Draw the main menu UI.
		2. If the quit-confirmation popup is open:
			* Render a semi-transparent dark overlay.
			* Draw the popup UI on top.
*/
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

// Nothing to see here
void MainMenuFree(){}

/*!
\brief
	Unload all assets used by the Main Menu scene.

\par
	Methodology:
		1. Stop and free menu audio.
		2. Clear both UI managers.
		3. Destroy the font used by the menu.
		4. Unload textures and meshes through DataLoader.
*/
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
