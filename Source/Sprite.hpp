#pragma once
#include "AEEngine.h"

struct Vector2D {
	f32 x{};
	f32 y{};
};

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
		Vector2D position;
		Vector2D scale;
		Color color;

		Sprite(AEGfxVertexList* spriteMesh, Vector2D spritePosition, Vector2D spriteScale, Color spriteColor);

		void UpdateTransform();

		void RenderSprite();
};

AEGfxVertexList* CreateSquareMesh();

AEGfxVertexList* CreateCircleMesh();