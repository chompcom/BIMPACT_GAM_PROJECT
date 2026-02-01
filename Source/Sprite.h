#pragma once
#include "AEEngine.h"
#include "Utils/Vector2.hpp"

struct Color {
	f32 r{};
	f32 g{};
	f32 b{};
	f32 a{};
};

class Sprite {
	public:
		AEGfxVertexList* mesh;
		AEMtx33 transform;
		Vector2 position;
		Vector2 scale;
		Color color;

		Sprite();
		Sprite(AEGfxVertexList* spriteMesh, Vector2 spritePosition, Vector2 spriteScale, Color spriteColor);

		void UpdateTransform();

		void RenderSprite();
};

class TexturedSprite : public Sprite {
	public:
		AEGfxTexture* texture;

		TexturedSprite(AEGfxVertexList* spriteMesh, AEGfxTexture* spriteTexture, Vector2 spritePosition, Vector2 spriteScale, Color spriteColor);

		void RenderSprite();
};

AEGfxVertexList* CreateSquareMesh();

AEGfxVertexList* CreateCircleMesh();