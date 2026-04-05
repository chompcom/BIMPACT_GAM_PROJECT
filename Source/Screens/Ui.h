#pragma once

/*!***************************************************************************
\file       ui.h
\author     Quah Ming Jun
\author		Quah Ming Jun, m.quah
\par        m.quah@digipen.edu

\brief
	This header file contains the declaration and documentation for UI functions defined in Ui.cpp;
	This aims to overhaul the entire UI management in this project, which can be adopted
	subsequently for future projects for making UI workflow efficient.

Copyright (C) 2026 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited.
***************************************************************************/


#include <string>		// for string
#include <vector>		// for vectors
#include <memory>		// for smart pointer
#include <functional>	// i want function pointers
#include <algorithm>
#include <cmath>

#include "AEEngine.h"
//#include "Loaders/DataLoader.h"
#include "../Utils/Vector2.hpp"
#include "../Sprite.h"



/*!
\struct UIElement
\brief
	Represents a single UI node in the hierarchy. Each element stores
	layout information, appearance attributes, event callbacks, and
	child elements.

\par Notes:
	
	Firstly, Each UIelement node is essentially a div...

	Layout:
		* localPos: position relative to parent center
		* sizeRatio: size relative to parent size
		* resolvedPos: computed world position
		* resolvedSize: computed world size

	Appearance:
		* text, textScale, textColor
		* backgroundColor
		* texturePath
		* borderRadiusRatio and roundSegments for rounded shapes

	Events:
		* visible, clickable, hovered
		* onClick, onHover, onHoverExit callbacks
*/
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

/*!
\enum UIResolvedShape
\brief
	Identifies the final shape used when rendering a UI element.
*/
enum class UIResolvedShape
{
	Rect,	// If rectangle
	Circle,	// If circle
	RoundedRect	// If r_rect
};

// Ui Manager (RabbitHole gg)

/*!
\class UIManager
\brief
	Manages the entire UI system. Handles creation, loading, layout
	resolution, event processing, and rendering of UI elements.

\par	Notes:

		Responsibilities:
			* Maintain root UI elements
			* Load UI from JSON files
			* Resolve layout recursively
			* Handle mouse events and callbacks
			* Render shapes, textures, and text
			* Provide element lookup by ID
*/
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

	/*!
	\brief
		Find a UI element by ID. (Like html, it can be used to bind stuff).

	\return
		Pointer to element, or nullptr if not found.
	*/
	UIElement* FindById(std::string const& id);
	
	// Event Listeners
	void BindOnClick(std::string const& id, std::function<void(UIElement&)> fn);	 // Bind a click callback to an element.
	void BindOnHover(std::string const& id, std::function<void(UIElement&)> fn);	 // Bind a hover callback to an element.
	void BindOnHoverExit(std::string const& id, std::function<void(UIElement&)> fn); // Bind a hover-exit callback to an element.

	// Update
	void Update();
	
	// Draw / Render
	void Draw();	// Render all UI elements.

private:
	s8 font{};
	bool useWindowRoot{ true };

	Vector2 rootPos{};
	Vector2 rootSize{};

	std::vector<std::unique_ptr<UIElement>> roots{};

private:

	void RefreshWindowRootRect();	// Recompute the root.
	void ResolveElement(UIElement& element, Vector2 const& parentPos, Vector2 const& parentSize); // Recursively resolve world position and size of an element.
	void UpdateElement(UIElement& element, Vector2 const& mouseWorld, bool mouseTriggered); // Recursively update element state and process mouse events.
	void DrawElement(UIElement const& element) const; // Recursively draw an element and its children.

	// Resolve the shape
	UIResolvedShape ResolveShape(UIElement const& element) const;

	void DrawRect(UIElement const& element) const; // Draw a rectangular UI element.
	void DrawCircle(UIElement const& element) const; // Draw a circular UI element.
	void DrawRoundedRect(UIElement const& element) const;  // Draw a rounded-rectangle UI element.

	// Events tester
	bool IsPointInside(UIElement const& element, Vector2 const& point) const; // Check if a point lies inside an element.
	bool PointInRect(UIElement const& element, Vector2 const& point) const; // Check if a point lies inside a rectangular element.
	bool PointInCircle(UIElement const& element, Vector2 const& point) const; // Check if a point lies inside a circular element.
	bool PointInRoundedRect(UIElement const& element, Vector2 const& point) const; // Check if a point lies inside a rounded-rectangle element. (Rabbit hole)

	Vector2 GetCursorWorldPosition() const; // Get the cursor position in world coordinates.
	f32 WorldToNdcX(f32 x) const;			// Convert world X coordinate to Normalized Device Coordinates.
	f32 WorldToNdcY(f32 y) const;			// Convert world Y coordinate to Normalized Device Coordinates.

	// Recursive helper for FindById.
	UIElement* FindByIdRecursive(UIElement& element, std::string const& id);
};