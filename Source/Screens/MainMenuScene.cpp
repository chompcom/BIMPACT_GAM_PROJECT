// Mainmenu UI Test

#include "MainMenuScene.h"

#include "AEEngine.h"
#include "../Screens/Ui.h"
#include "../GameStateManager.h"
#include "../GameStateList.h"

namespace
{
	UIManager ui;
	s8 uiFont = 0;
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

	// Bind button actions
	ui.BindOnClick("btn_start", [](UIElement& self)
		{
			UNREFERENCED_PARAMETER(self);
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
			ChangeState(GS_QUIT);	// Apparently game running must be changed too. I thought gsm would handle this lmao.
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

	ui.BindOnHover("btn_quit", [](UIElement& self)
		{
			self.backgroundColor = Color{ 0.6f, 0.6f, 0.6f, 1.0f };
		});

	ui.BindOnHoverExit("btn_quit", [](UIElement& self)
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
	ui.Update();
	//if (AEInputCheckTriggered(AEVK_ESCAPE))
	//{
	//	ChangeState(GS_QUIT);
	//}
}

void MainMenuDraw()
{
	// Soft background
	//AEGfxSetBackgroundColor(0.12f, 0.12f, 0.12f);

	ui.Draw();
}

void MainMenuFree(){}

void MainMenuUnload()
{
	FreeMenuAudio();

	ui.Clear();

	if (uiFont)
	{
		AEGfxDestroyFont(uiFont);
		uiFont = 0;
	}

	DataLoader::Unload();
}
