#pragma once

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

	// Manual workflow (Please don't be stupid and use, but possible)
	UIElement* CreateElement(std::string const& id, UIElement* parent = nullptr);

	// JSON workflow (Good practice)
	bool LoadFromFile(std::string const& filePath);

	// Popup
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