/* Start Header ************************************************************************/
/*!
\file       Sprite.h
\author     Yee Kiat Lim, yeekiat.lim, 2503993
\par        yeekiat.lim@digipen.edu
\brief		This file implements the Sprite classes used to render various sprites.

Copyright (C) 2026 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited.
*/
/* End Header **************************************************************************/

#pragma once
#include "AEEngine.h"
#include "Utils/Vector2.hpp"
#include <vector>

// Color of sprites
struct Color {
	f32 r{};
	f32 g{};
	f32 b{};
	f32 a{};
};

// Basic sprite
class Sprite {
	public:
		AEGfxVertexList* mesh;
		AEMtx33 transform;
		Vector2 position;
		Vector2 scale;
		Color color;

		Sprite();
		Sprite(AEGfxVertexList* spriteMesh, Vector2 spritePosition = Vector2{}, Vector2 spriteScale = Vector2{1, 1}, Color spriteColor = Color{0.0f,0.0f,0.0f,1.0f});

		void UpdateTransform();

		void RenderSprite(bool changeAlpha = false);
};

// Sprite that supports texture
class TexturedSprite : public Sprite {
	public:
		AEGfxTexture* texture;

		TexturedSprite(AEGfxVertexList* spriteMesh, AEGfxTexture* spriteTexture, Vector2 spritePosition, Vector2 spriteScale, Color spriteColor = Color{ 1.0f,1.0f,1.0f,1.0f });

		// Function supports rendering of textured and animated sprites
		void RenderSprite(bool changeAlpha = false, f32 UVOffsetX = 0.f, f32 UVOffsetY = 0.f);
};

struct AnimationInfo {
	std::string name;
	s32 startRow{};
	s32 startCol{};
	s32 noFrames{};
	f32 frameDuration{ 0.25f };
};

// Sprite that supports animations
class AnimatedSprite : public TexturedSprite {
	public:
		u32 spritesheetRows;
		u32 spritesheetCols;
		f32 spriteUVWidth;
		f32 spriteUVHeight;

		f32 animationTimer = 0.f;

		u32 currentSpriteIndex = 0;

		f32 currentSpriteUVOffsetX;
		f32 currentSpriteUVOffsetY;

		f32 initialOffsetX;
		f32 initialOffsetY;

		std::vector<AnimationInfo> animations;
		AnimationInfo currentAnimation;


		AnimatedSprite(TexturedSprite texturedSprite, u32 spritesheetRows = 1, u32 spritesheetCols = 1, f32 initialOffsetX = 0.f, f32 initialOffsetY = 0.f);

		void SetAnimation(AnimationInfo animationInfo);
		void GetAnimation(std::string animationName);
		void UpdateAnimation(f32 dt);
};


AEGfxVertexList* CreateSquareMesh(f32 spriteUVWidth = 1.0f, f32 spriteUVHeight = 1.0f);

AEGfxVertexList* CreateCircleMesh();