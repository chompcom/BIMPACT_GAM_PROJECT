#include "UI.h"

namespace
{
	constexpr f32 kSquareTolerance = 0.001f;		
	constexpr f32 kRoundRectAspectThreshold = 5.0f; // For scale purposes, if aspect ratio > 5, then use piecewise scaling

	// Get color from json
	void ReadColor(Json::Value const& value, Color& outColor)
	{
		if (!value.isArray() || value.size() < 4) return;

		outColor.r = value[0].asFloat();
		outColor.g = value[1].asFloat();
		outColor.b = value[2].asFloat();
		outColor.a = value[3].asFloat();
	}

	// Thanks to celeste for reading the docs ig
	void PrintTextCentered(s8 font, char const* text, f32 ndcX, f32 ndcY, f32 scale, Color color)
	{
		if (!text) return;

		f32 textWidth = 0.0f;
		f32 textHeight = 0.0f;
		AEGfxGetPrintSize(font, text, scale, &textWidth, &textHeight);	// Get print size

		// Print base on text size
		AEGfxPrint(
			font,
			text,
			ndcX - textWidth * 0.5f,
			ndcY - textHeight * 0.5f,
			scale,
			color.r, color.g, color.b, color.a
		);
	}
}

// UIManager init
UIManager::UIManager(): font(0), useWindowRoot(true), rootPos(0.f, 0.f), rootSize(0.f, 0.f)
{
	RefreshWindowRootRect();
}

// Set Font
void UIManager::SetFont(s8 newFont)
{
	font = newFont;
}

// Window as root
void UIManager::UseWindowAsRootRect()
{
	useWindowRoot = true;
	RefreshWindowRootRect();
}

// Window should not be root, perhaps an overlay
void UIManager::SetRootRect(Vector2 pos, Vector2 size)
{
	useWindowRoot = false;
	rootPos = pos;
	rootSize = size;
}

// Refresh current root... Tbf not a need but issok
void UIManager::RefreshWindowRootRect()
{
	rootPos = Vector2(0.f, 0.f);
	rootSize = Vector2(
		AEGfxGetWinMaxX() - AEGfxGetWinMinX(),
		AEGfxGetWinMaxY() - AEGfxGetWinMinY()
	);
}

// Clear all uielements
void UIManager::Clear()
{
	roots.clear();
}

// Create unique element using id and parent
UIElement* UIManager::CreateElement(std::string const& id, UIElement* parent)
{
	std::unique_ptr<UIElement> element = std::make_unique<UIElement>();	// Smart pointers, given that it is complex to manage which parents or child to be deleted
	element->id = id;

	UIElement* raw = element.get();	// Retrieves pointer

	if (parent == nullptr) roots.push_back(std::move(element));
	else
	{
		element->parent = parent;
		parent->children.push_back(std::move(element));	// Store elements into parent vector array
	}

	return raw;
}

// Load UI from File (Hope this works properly)
bool UIManager::LoadFromFile(std::string const& filePath)
{
	Clear();	// Hopefully doesn't cause complications like clear ongoing UI rendered? SHOULD ONLY BE USED IN LOAD() in current state

	Json::Value root = DataLoader::LoadJsonFile(filePath); // I love json
	if (root.isNull()) return false;


	Json::Value const& elements = root["elements"];
	if (!elements.isArray()) return false;

	// Utility mini function for loading nodes
	std::function<bool(Json::Value const&, UIElement*)> loadNode = [this, &loadNode](Json::Value const& value, UIElement* parent) -> bool
	{
		if (!value.isObject()) return false;
		if (!value.isMember("id")) return false;

		UIElement* element = CreateElement(value["id"].asString(), parent);
		if (!element) return false;

		if (value.isMember("x"))													element->localPos.x = value["x"].asFloat();
		if (value.isMember("y"))													element->localPos.y = value["y"].asFloat();
		if (value.isMember("w"))													element->sizeRatio.x = value["w"].asFloat();
		if (value.isMember("h"))													element->sizeRatio.y = value["h"].asFloat();
		if (value.isMember("visible"))												element->visible = value["visible"].asBool();
		if (value.isMember("clickable"))											element->clickable = value["clickable"].asBool();
		if (value.isMember("drawBackground"))										element->drawBackground = value["drawBackground"].asBool();
		if (value.isMember("borderRadiusRatio"))									element->borderRadiusRatio = value["borderRadiusRatio"].asFloat();
		if (value.isMember("roundSegments"))										element->roundSegments = value["roundSegments"].asInt();
		if (value.isMember("text"))													element->text = value["text"].asString();
		if (value.isMember("textScale"))											element->textScale = value["textScale"].asFloat();
		if (value.isMember("texture"))												element->texturePath = value["texture"].asString();
		if (value.isMember("backgroundColor"))										ReadColor(value["backgroundColor"], element->backgroundColor);
		if (value.isMember("textColor"))											ReadColor(value["textColor"], element->textColor);

		Json::Value const& children = value["children"];
		
		// Recursively load children elements
		if (children.isArray())
		{
			for (Json::ArrayIndex i = 0; i < children.size(); ++i)
			{
				if (!loadNode(children[i], element)) return false;
			}
		}

		return true;
		};


	// Loads all elements found at the current level
	for (Json::ArrayIndex i = 0; i < elements.size(); ++i)
	{
		if (!loadNode(elements[i], nullptr))
			return false;
	}

	return true;
}


bool UIManager::LoadFromFilePopUp(std::string const& filePath, Vector2 pos, Vector2 size) {
	this->Clear();
	//this->SetFont(this->font);

	this->SetRootRect(pos, size);
	bool res = this->LoadFromFile(filePath);
	
	return res;
}

// Finds UI by ID (Should be useful later on)
UIElement* UIManager::FindById(std::string const& id)
{
	for (std::unique_ptr<UIElement>& root : roots)
	{
		if (!root) continue;

		UIElement* found = FindByIdRecursive(*root, id);
		if (found) return found;
	}

	return nullptr;
}

// Find Uielement by id recursively
UIElement* UIManager::FindByIdRecursive(UIElement& element, std::string const& id)
{
	if (element.id == id)
		return &element;

	for (std::unique_ptr<UIElement>& child : element.children)
	{
		if (!child) continue;

		UIElement* found = FindByIdRecursive(*child, id);
		if (found) return found;
	}

	return nullptr;
}

// Bind onclick function
void UIManager::BindOnClick(std::string const& id, std::function<void(UIElement&)> fn)
{
	UIElement* element = FindById(id);
	if (element) element->onClick = fn;
}

// Bind onhover functions
void UIManager::BindOnHover(std::string const& id, std::function<void(UIElement&)> fn)
{
	UIElement* element = FindById(id);
	if (element) element->onHover = fn;
}

// Bind onhover exit functions
void UIManager::BindOnHoverExit(std::string const& id, std::function<void(UIElement&)> fn)
{
	UIElement* element = FindById(id);
	if (element) element->onHoverExit = fn;
}

// Update function
void UIManager::Update()
{
	if (useWindowRoot) RefreshWindowRootRect();


	// Manage events
	Vector2 mouseWorld = GetCursorWorldPosition(); 
	bool mouseTriggered = AEInputCheckTriggered(AEVK_LBUTTON);

	for (std::unique_ptr<UIElement>& root : roots)
	{
		if (!root || !root->visible) continue;

		ResolveElement(*root, rootPos, rootSize);
		UpdateElement(*root, mouseWorld, mouseTriggered);
	}
}

// Convert to world postiion and size
void UIManager::ResolveElement(UIElement& element, Vector2 const& parentPos, Vector2 const& parentSize)
{
	element.resolvedSize.x = parentSize.x * element.sizeRatio.x;
	element.resolvedSize.y = parentSize.y * element.sizeRatio.y;

	element.resolvedPos.x = parentPos.x + parentSize.x * element.localPos.x;
	element.resolvedPos.y = parentPos.y + parentSize.y * element.localPos.y;

	for (std::unique_ptr<UIElement>& child : element.children)
	{
		if (!child) continue;
		ResolveElement(*child, element.resolvedPos, element.resolvedSize);
	}
}

// Update element
void UIManager::UpdateElement(UIElement& element, Vector2 const& mouseWorld, bool mouseTriggered)
{
	bool wasHovered = element.hovered;
	bool isHovered = false;

	// Test if user hovered over element
	if (element.visible && element.clickable) isHovered = IsPointInside(element, mouseWorld);
	element.hovered = isHovered;
	
	if (!wasHovered && element.hovered) {
		HoverAudio();
		if(element.onHover)
		element.onHover(element);	// Execute onhover function
	}
	else if (wasHovered && !element.hovered && element.onHoverExit) element.onHoverExit(element);	// Execute onhover exit function
	
	// If element is hovered over and mouse is triggered
	if (element.hovered && mouseTriggered && element.onClick) {
		ButtonAudio();
		element.onClick(element);
	}
	// Update children recursively
	for (std::unique_ptr<UIElement>& child : element.children)
	{
		if (!child || !child->visible) continue;
		UpdateElement(*child, mouseWorld, mouseTriggered);
	}
}

// Identify / Classify shapes
UIResolvedShape UIManager::ResolveShape(UIElement const& element) const
{
	f32 width = element.resolvedSize.x;
	f32 height = element.resolvedSize.y;

	if (element.borderRadiusRatio <= 0.0f)
		return UIResolvedShape::Rect;

	if (std::fabs(width - height) <= kSquareTolerance && element.borderRadiusRatio >= 0.5f)
	{
		return UIResolvedShape::Circle;
	}

	return UIResolvedShape::RoundedRect;
}

// Test if point is inside element
bool UIManager::IsPointInside(UIElement const& element, Vector2 const& point) const
{
	switch (ResolveShape(element))
	{
	case UIResolvedShape::Circle:
		return PointInCircle(element, point);

	case UIResolvedShape::RoundedRect:
		return PointInRoundedRect(element, point);

	case UIResolvedShape::Rect:
	default:
		return PointInRect(element, point);
	}
}

// Test if point is inside rectangle
bool UIManager::PointInRect(UIElement const& element, Vector2 const& point) const
{
	f32 halfW = element.resolvedSize.x * 0.5f;
	f32 halfH = element.resolvedSize.y * 0.5f;

	return 
	   (point.x >= element.resolvedPos.x - halfW && point.x <= element.resolvedPos.x + halfW &&
		point.y >= element.resolvedPos.y - halfH && point.y <= element.resolvedPos.y + halfH);
}

// Test if point is in circle
bool UIManager::PointInCircle(UIElement const& element, Vector2 const& point) const
{
	f32 radius = element.resolvedSize.x * 0.5f;
	return element.resolvedPos.DistanceSq(point) <= (radius * radius);
}

// Rounded rect uses SDF math for calculation. 
// I don't really understand this 2D Signed Distance Function Formula tbh i just based it off math and help from claude.
bool UIManager::PointInRoundedRect(UIElement const& element, Vector2 const& point) const
{
	// 1. Define dimensions and clamped radius
	const f32 halfWidth = element.resolvedSize.x * 0.5f;
	const f32 halfHeight = element.resolvedSize.y * 0.5f;
	const f32 minSide = AEMin(element.resolvedSize.x, element.resolvedSize.y);

	// Scale the ratio and clamp it so the radius never exceeds the rectangle's bounds
	f32 radius = element.borderRadiusRatio * minSide;
	radius = AEClamp(radius, 0.0f, minSide * 0.5f);

	// This allows us to calculate the distance for only one corner
	const f32 deltaX = std::fabs(point.x - element.resolvedPos.x);
	const f32 deltaY = std::fabs(point.y - element.resolvedPos.y);

	// (The center point of the corner's curvature circle)
	const f32 qx = deltaX - (halfWidth - radius);
	const f32 qy = deltaY - (halfHeight - radius);

	// Distance to the corner if the point is outside the inner box
	const f32 externalDist = std::sqrt(AEMax(qx, 0.0f) * AEMax(qx, 0.0f) + AEMax(qy, 0.0f) * AEMax(qy, 0.0f));

	// Distance to the nearest edge if the point is inside the inner box
	const f32 internalDist = AEMin(AEMax(qx, qy), 0.0f);

	// Final distance
	const f32 signedDistance = (externalDist + internalDist) - radius;

	return signedDistance <= 0.0f;
}


// Draw all elements in Ui
void UIManager::Draw()
{
	for (std::unique_ptr<UIElement> const& root : roots)
	{
		if (!root || !root->visible) continue;
		DrawElement(*root);
	}
}

// Draw elements recursively
void UIManager::DrawElement(UIElement const& element) const
{
	if (!element.visible) return;

	AEGfxSetBlendMode(AE_GFX_BM_BLEND);
	//AEGfxSetTransparency(1.0f);

	if (element.drawBackground)
	{
		switch (ResolveShape(element))
		{
		case UIResolvedShape::Circle:
			DrawCircle(element);
			break;

		case UIResolvedShape::RoundedRect:
			DrawRoundedRect(element);
			break;

		case UIResolvedShape::Rect:
		default:
			DrawRect(element);
			break;
		}
	}

	// For images exclusively
	if (!element.texturePath.empty())
	{
		// Set all back settings
		AEGfxSetRenderMode(AE_GFX_RM_TEXTURE);
		AEGfxSetBlendMode(AE_GFX_BM_BLEND);
		AEGfxSetTransparency(1.0f);		// V IMPORTANT

		TexturedSprite img = DataLoader::CreateTexture(element.texturePath);

		// Position and transformation
		img.position = element.resolvedPos;
		img.scale = element.resolvedSize;
		img.UpdateTransform();

		// COLOR AND ALPHA
		img.color = Color{ element.backgroundColor.r, element.backgroundColor.g, element.backgroundColor.b, element.backgroundColor.a};
		img.RenderSprite(true);

		// SET IT BACK TO BLEND MODE FOR OTHER SEQUENCES
		AEGfxSetBlendMode(AE_GFX_BM_BLEND);
	}

	// For text rendering
	if (!element.text.empty() && font)
	{
		PrintTextCentered(
			font,
			element.text.c_str(),
			WorldToNdcX(element.resolvedPos.x),
			WorldToNdcY(element.resolvedPos.y),
			element.textScale,
			element.textColor
		);
	}


	// Continue to render elements recursively for children
	for (std::unique_ptr<UIElement> const& child : element.children)
	{
		if (!child) continue;
		DrawElement(*child);
	}
}

// Drawing rect
void UIManager::DrawRect(UIElement const& element) const
{
	AEGfxVertexList* mesh = DataLoader::GetOrCreateSquareMesh();
	if (!mesh) return;

	AEGfxSetRenderMode(AE_GFX_RM_COLOR);

	Sprite sprite(
		mesh,
		element.resolvedPos,
		element.resolvedSize,
		element.backgroundColor
	);
	sprite.RenderSprite(true);
}

// Drawing circle meshes
void UIManager::DrawCircle(UIElement const& element) const
{
	AEGfxVertexList* mesh = DataLoader::GetOrCreateCircleMesh();
	if (!mesh) return;

	AEGfxSetRenderMode(AE_GFX_RM_COLOR);

	Sprite sprite(
		mesh,
		element.resolvedPos,
		element.resolvedSize,
		element.backgroundColor
	);
	sprite.RenderSprite(true);
}

// Drawing round rectangle functions
void UIManager::DrawRoundedRect(UIElement const& element) const
{
	f32 width = element.resolvedSize.x;
	f32 height = element.resolvedSize.y;

	f32 smaller = AEMin(width, height);
	if (smaller <= 0.0f) return;

	f32 aspectRatio = AEMax(width, height) / smaller;	// Get current aspect ratio

	AEGfxSetRenderMode(AE_GFX_RM_COLOR);

	// If aspect ratio is not extreme, to use normal scale method
	if (aspectRatio <= kRoundRectAspectThreshold)
	{
		AEGfxVertexList* mesh = DataLoader::GetOrCreateRoundRectMesh(element.borderRadiusRatio, element.roundSegments);

		if (!mesh) return;

		Sprite sprite(mesh, element.resolvedPos, element.resolvedSize, element.backgroundColor);
		sprite.RenderSprite(true);
	}
	// Aspect ratio is extreme... To prevent distortion in rendering create a new roundrect mesh with the new width and height... Honestly a pain
	else
	{
		// Honestly cleanest rendering method, otherwise use piecewise scaling, which may look slightly different from unit shape due to distort
		AEGfxVertexList* mesh = DataLoader::CreateRoundRectMesh( element.borderRadiusRatio, element.roundSegments, width, height);

		if (!mesh) return;

		Sprite sprite(mesh, element.resolvedPos, Vector2(1.0f, 1.0f), element.backgroundColor);
		sprite.RenderSprite(true);

		AEGfxMeshFree(mesh);	// FREE CURRENT MESH
	}
}

// Get current mouse position in world coordinates
Vector2 UIManager::GetCursorWorldPosition() const
{
	s32 mouseX{}, mouseY{};
	AEInputGetCursorPosition(&mouseX, &mouseY);

	f32 x = AEGfxGetWinMinX() + static_cast<f32>(mouseX);
	f32 y = AEGfxGetWinMaxY() - static_cast<f32>(mouseY);

	return Vector2(x, y);
}

f32 UIManager::WorldToNdcX(f32 x) const
{
	f32 minX = AEGfxGetWinMinX();
	f32 maxX = AEGfxGetWinMaxX();
	return ((x - minX) / (maxX - minX)) * 2.0f - 1.0f;
}

f32 UIManager::WorldToNdcY(f32 y) const
{
	f32 minY = AEGfxGetWinMinY();
	f32 maxY = AEGfxGetWinMaxY();
	return ((y - minY) / (maxY - minY)) * 2.0f - 1.0f;
}