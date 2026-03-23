#pragma once

/*!***************************************************************************
\file       ui.h
\author     Quah Ming Jun
\par        DP email: m.quah\@digipen.edu
\par        Course: CSD1451

\brief
	This header file contains the declaration and documentation for UI functions defined in Ui.cpp;
	This aims to overhaul the entire UI management in this project, which can be adopted
	subsequently for future projects for making UI workflow efficient.
***************************************************************************/


#include <string>	// for string
#include <vector>	// for vectors
#include <memory>	// for smart pointer
#include <functional>	// i want function pointers
#include <algorithm>
#include <cmath>

#include "AEEngine.h"
#include "Loaders/DataLoader.h"
#include "../Utils/Vector2.hpp"
#include "../Sprite.h"


//struct COLOR {
//	f32 r{}, g{}, b{}, a{};
//};

// Each UIelement node is essentially a div...
struct UIElement
{
	// For UI manager registration
	std::string id{};

	// Node management
	UIElement* parent{ nullptr };
	std::vector<std::unique_ptr<UIElement>> children{};

	// Local
	Vector2 localPos{};		// Relative offset from parent center
	Vector2 sizeRatio{};	// Relative offset from parent size

	// World (Done by Ui Manager)
	Vector2 resolvedPos{};	// Real world coordinates
	Vector2 resolvedSize{};	// Real size

	// Attributes
	std::string text{};
	Color backgroundColor{ 1.f, 1.f, 1.f, 1.f };
	f32 textScale{ 1.0f };
	Color textColor{ 0.f, 0.f, 0.f, 1.f };
	std::string texturePath{};
	f32 borderRadiusRatio{ 0.0f };  // What a pain (rabbit-hole of research)
	int roundSegments{ 8 };			// Circle Mesh basically

	// Events Attributes
	bool visible{ true };
	bool clickable{ false };
	bool hovered{ false };
	bool drawBackground{ false };

	std::function<void(UIElement&)> onClick{};
	std::function<void(UIElement&)> onHover{};
	std::function<void(UIElement&)> onHoverExit{};
};

enum class UIResolvedShape
{
	Rect,	// If rectangle
	Circle,	// If circle
	RoundedRect	// If r_rect
};

// Ui Manager (RabbitHole gg)
class UIManager
{
public:
	UIManager();

	void SetFont(s8 newFont);

	void UseWindowAsRootRect();						// Use window as root.
	void SetRootRect(Vector2 pos, Vector2 size);	// Use something else as root

	void Clear();


	/*
	\brief
		Manual workflow (Please don't be stupid and use, but possible).

	\param[in] id
		id.

	\param[in] parent
		Parent element (html like).

	\return
		Pointer to element created.


	\par	How to create raw text (and where to use)

		// GLOBAL
		static UIManager winUI;

		// LOAD
		UIElement* winScreen = winUI.CreateElement("win_div");
		winScreen->localPos = Vector2(0.0f, 0.0f);
		winScreen->sizeRatio = Vector2(1.0f, 1.0f);
		winScreen->text = "YOU WIN!";
		winScreen->textScale = 5.0f;
		winScreen->visible = true;
		winUI.SetFont(font);

		// DRAW
		winUI.draw();

		// Update
		winUI.update();

		// Unload
		Dataloader::unload();	// As long as this is called in the unload function pointer, all meshes created at runtime will be freed.
	*/
	UIElement* CreateElement(std::string const& id, UIElement* parent = nullptr);


	/*
	\brief Creates window from JSON file. JSON workflow (Good practice)

	\param[in] filePath
		Current JSON Filepath from root of this program's executable containing the UI information...
		To find current filepath, cd or echo %CD% (cmd), $pwd.path (powershell), or echo $PWD (linux).
		In code, do system("echo %CD%") depending on current cli (make sure it is visible).

	\return
		Result of Loading

	\par
		How to use this guide (for entire windows that are the scene / screens itself for this state)

	// Global
	static UIManager pauseUi;

	// Load Function Pointer;
	std::string filePath = "Assets/UI/pause.json";
	bool res = pauseUi.LoadFromFile(filePath);
	if (!res){
		std::string cliInput = "powershell.exe -c \"Test-Path ([System.IO.Path]::Combine($PWD.Path, '" + filePath + "'))\"";
		system(cliInput.c_str());	// Look and see if yo shit is FALSE, that means it doesn't exist on that path
		return;
	}

	UIElement* tipText = pauseUi.FindById("tip_text");
	tipText->text = RandomTip();	// Perhaps you want to change the text here
	tipText->scale = 0.35f;			// Scale Tip size
	pauseUi.SetFont(font);	// Make sure to set font hahah. I'm not creating a font object in the UIManager itself by default so gl. Perhaps could be done in future.

	// Update
	pauseUi.update();

	// Draw
	pauseUi.draw();

	// Unload
		Dataloader::unload();	// As long as this is called in the unload function pointer, all meshes created at runtime will be freed.
	*/
	bool LoadFromFile(std::string const& filePath);

	/*
	\brief Creates popup window from JSON file

	\param[in] filePath
		Current JSON Filepath from root of this program's executable containing the UI information...
		To find current filepath, cd or echo %CD% (cmd), $pwd.path (powershell), or echo $PWD (linux).
		In code, do system("echo %CD%") depending on current cli (make sure it is visible).

	\param[in] pos
		Vector2 [x, y] position of object

	\param[in] size
		Vector2 [x, y] size or scale of object

	\return
		Result of Loading

	\par
		How to use this guide (for popup windows mostly that do not cover entirety of the screen itself)

	// Global
	static UIManager pauseUi;

	// Load Function Pointer;
	std::string filePath = "Assets/UI/pause_popup.json";
	bool res = pauseUi.LoadFromFilePopUp(filePath, Vector2(0.0f,0.0f), Vector2(580.0f, 250.0f));
	if (!res){
		// Debug check: Verify if the file actually exists via PowerShell
		std::string cliInput = "powershell.exe -c \"Test-Path ([System.IO.Path]::Combine($PWD.Path, '" + filePath + "'))\"";
		system(cliInput.c_str());	// Look and see if yo shit is FALSE, that means it doesn't exist on that path
		return;
	}
	
	UIElement* tipText = pauseUi.FindById("tip_text");
	tipText->text = RandomTip();	// Perhaps you want to change the text here
	tipText->scale = 0.35f;			// Scale Tip size
	pauseUi.SetFont(font);	// Make sure to set font hahah. I'm not creating a font object in the UIManager itself by default so gl. Perhaps could be done in future.

	// Update
	pauseUi.update();

	// Draw
	pauseUi.draw();

	// Unload
		Dataloader::unload();	// As long as this is called in the unload function pointer, all meshes created at runtime will be freed.
	*/
	bool LoadFromFilePopUp(std::string const& filePath, Vector2 pos, Vector2 size);


	// Find By Id (Can be used to bind stuff)
	UIElement* FindById(std::string const& id);
	
	// Event Listeners
	void BindOnClick(std::string const& id, std::function<void(UIElement&)> fn);
	void BindOnHover(std::string const& id, std::function<void(UIElement&)> fn);
	void BindOnHoverExit(std::string const& id, std::function<void(UIElement&)> fn);

	// Update
	void Update();
	
	// Draw / Render
	void Draw();

private:
	s8 font{};
	bool useWindowRoot{ true };

	Vector2 rootPos{};
	Vector2 rootSize{};

	std::vector<std::unique_ptr<UIElement>> roots{};

private:

	void RefreshWindowRootRect();
	void ResolveElement(UIElement& element, Vector2 const& parentPos, Vector2 const& parentSize);
	void UpdateElement(UIElement& element, Vector2 const& mouseWorld, bool mouseTriggered);
	void DrawElement(UIElement const& element) const;

	// Resolve the shape
	UIResolvedShape ResolveShape(UIElement const& element) const;

	void DrawRect(UIElement const& element) const;
	void DrawCircle(UIElement const& element) const;
	void DrawRoundedRect(UIElement const& element) const;

	// Events tester
	bool IsPointInside(UIElement const& element, Vector2 const& point) const;
	bool PointInRect(UIElement const& element, Vector2 const& point) const;
	bool PointInCircle(UIElement const& element, Vector2 const& point) const;
	bool PointInRoundedRect(UIElement const& element, Vector2 const& point) const;

	Vector2 GetCursorWorldPosition() const;
	f32 WorldToNdcX(f32 x) const;
	f32 WorldToNdcY(f32 y) const;

	UIElement* FindByIdRecursive(UIElement& element, std::string const& id);
};