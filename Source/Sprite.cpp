#include "AEEngine.h"
#include "Sprite.h"

Sprite::Sprite(AEGfxVertexList* spriteMesh, Vector2 spritePosition, Vector2 spriteScale, Color spriteColor) 
	: mesh{ spriteMesh }, transform{ 0 }, position {spritePosition}, scale{ spriteScale }, color{ spriteColor } {
		UpdateTransform();
}

Sprite::Sprite() : Sprite{nullptr,Vector2(),Vector2(1,1),Color{1.0,1.0,1.0,0.0}}
{
}

void Sprite::UpdateTransform() {
		// Create a scale matrix that scales by 500 x and y
		AEMtx33 transformScale = { 0 };
		AEMtx33Scale(&transformScale, scale.x, scale.y);

		// Create a rotation matrix that rotates by 90 degrees
		// Note that PI in radians is 180 degrees.
		// Since 90 degrees is 180/2, 90 degrees in radians is PI/2
		AEMtx33 transformRotate = { 0 };
		AEMtx33Rot(&transformRotate, 0.f);

		// Create a translation matrix that translates by
		// 200 in the x-axis and 100 in the y-axis
		AEMtx33 transformTranslate = { 0 };
		AEMtx33Trans(&transformTranslate, position.x, position.y);

		// Concatenate the matrices into the 'transform' variable.
		// We concatenate in the order of translation * rotation * scale
		// i.e. this means we scale, then rotate, then translate.
		transform = { 0 };
		AEMtx33Concat(&transform, &transformRotate, &transformScale);
		AEMtx33Concat(&transform, &transformTranslate, &transform);
}

void Sprite::RenderSprite() {
		AEGfxSetColorToMultiply(color.r, color.g, color.b, color.a);
		AEGfxSetColorToAdd(0.f, 0.f, 0.f, 1.f);
		AEGfxSetTransform(transform.m);
		AEGfxMeshDraw(mesh, AE_GFX_MDM_TRIANGLES);
}

TexturedSprite::TexturedSprite(AEGfxVertexList* spriteMesh, AEGfxTexture* spriteTexture, Vector2 spritePosition, Vector2 spriteScale, Color spriteColor)
	: Sprite(spriteMesh, spritePosition, spriteScale, spriteColor), texture{ spriteTexture } {
}

void TexturedSprite::RenderSprite() {
		AEGfxTextureSet(texture, 0.f, 0.f);
		Sprite::RenderSprite();
}

AEGfxVertexList* CreateSquareMesh() {
		// Informing the library that we're about to start adding triangles
		AEGfxMeshStart();

		// This shape has 2 triangles that makes up a square
		// Color parameters represent colours as ARGB
		// UV coordinates to read from loaded textures
		AEGfxTriAdd(
			-0.5f, -0.5f, 0xFFFFFFFF, 0.0f, 1.0f,  // bottom-left: red
			0.5f, -0.5f, 0xFFFFFFFF, 1.0f, 1.0f,   // bottom-right: green
			-0.5f, 0.5f, 0xFFFFFFFF, 0.0f, 0.0f);  // top-left: blue

		AEGfxTriAdd(
			0.5f, -0.5f, 0xFFFFFFFF, 1.0f, 1.0f,   // bottom-right: green
			0.5f, 0.5f, 0xFFFFFFFF, 1.0f, 0.0f,    // top-right: white
			-0.5f, 0.5f, 0xFFFFFFFF, 0.0f, 0.0f);  // top-left: blue

		// Saving the mesh (list of triangles) in pMesh
		AEGfxVertexList* mesh = AEGfxMeshEnd();

		return mesh;
}


AEGfxVertexList* CreateCircleMesh() {
		AEGfxMeshStart();

		for (f32 x{}; x <= 0.5f; x += 0.0025f) {
			f32 next_x{ x + 0.0025f };
			AEGfxTriAdd(
				0.0f, 0.0f, 0xFFFFFFFF, 0.5f, 0.5f,  // center of circle
				x, sqrtf(0.5f * 0.5f - x * x), 0xFFFFFFFF, 0.5f + x, 0.5f + sqrtf(0.5f * 0.5f - x * x),   // left end of triangle slice
				next_x, sqrtf(0.5f * 0.5f - next_x * next_x), 0xFFFFFFFF, 0.5f + next_x, 0.5f + sqrtf(0.5f * 0.5f - next_x * next_x));   // right end of triangle slice
			AEGfxTriAdd(
				0.0f, 0.0f, 0xFFFFFFFF, 0.5f, 0.5f,  // center of circle
				x, -sqrtf(0.5f * 0.5f - x * x), 0xFFFFFFFF, 0.5f + x, 0.5f - sqrtf(0.5f * 0.5f - x * x),   // left end of triangle slice
				next_x, -sqrtf(0.5f * 0.5f - next_x * next_x), 0xFFFFFFFF, 0.5f + next_x, 0.5f - sqrtf(0.5f * 0.5f - next_x * next_x));   // right end of triangle slice
			AEGfxTriAdd(
				0.0f, 0.0f, 0xFFFFFFFF, 0.5f, 0.5f,  // center of circle
				-x, sqrtf(0.5f * 0.5f - x * x), 0xFFFFFFFF, 0.5f - x, 0.5f + sqrtf(0.5f * 0.5f - x * x),   // left end of triangle slice
				-next_x, sqrtf(0.5f * 0.5f - next_x * next_x), 0xFFFFFFFF, 0.5f - next_x, 0.5f + sqrtf(0.5f * 0.5f - next_x * next_x));   // right end of triangle slice
			AEGfxTriAdd(
				0.0f, 0.0f, 0xFFFFFFFF, 0.5f, 0.5f,  // center of circle
				-x, -sqrtf(0.5f * 0.5f - x * x), 0xFFFFFFFF, 0.5f - x, 0.5f - sqrtf(0.5f * 0.5f - x * x),   // left end of triangle slice
				-next_x, -sqrtf(0.5f * 0.5f - next_x * next_x), 0xFFFFFFFF, 0.5f - next_x, 0.5f - sqrtf(0.5f * 0.5f - next_x * next_x));   // right end of triangle slice
		}

		AEGfxVertexList* mesh = AEGfxMeshEnd();

		return mesh;
}