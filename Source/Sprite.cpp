/* Start Header ************************************************************************/
/*!
\file       Sprite.cpp
\author     Yee Kiat Lim, yeekiat.lim, 2503993
\par        yeekiat.lim@digipen.edu
\brief		This file implements the necessary functionality for sprites as well as 
			utility functions to create meshes for the sprites.

Copyright (C) 2026 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited.
*/
/* End Header **************************************************************************/

#include "AEEngine.h"
#include "Sprite.h"

#include <iostream>

// Constructor for sprite
Sprite::Sprite(AEGfxVertexList* spriteMesh, Vector2 spritePosition, Vector2 spriteScale, Color spriteColor) 
	: mesh{ spriteMesh }, transform{ 0 }, position {spritePosition}, scale{ spriteScale }, color{ spriteColor } {
		UpdateTransform();
}

// Default constructor for sprite
Sprite::Sprite() : Sprite{nullptr,Vector2(),Vector2(1,1),Color{1.0,1.0,1.0,0.0}}
{
}

// Update transformation matrix of sprite
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

// Handles rendering of sprite
void Sprite::RenderSprite(bool changeAlpha) {
	// Set sprite color
	AEGfxSetColorToMultiply(color.r, color.g, color.b, color.a);

	if (!changeAlpha) AEGfxSetColorToAdd(0.f, 0.f, 0.f, 1.f);
	else AEGfxSetColorToAdd(0.f, 0.f, 0.f, 0.0f);

	// Render sprite
	AEGfxSetTransform(transform.m);
	AEGfxMeshDraw(mesh, AE_GFX_MDM_TRIANGLES);
}

// Constructor for textured sprite
TexturedSprite::TexturedSprite(AEGfxVertexList* spriteMesh, AEGfxTexture* spriteTexture, Vector2 spritePosition, Vector2 spriteScale, Color spriteColor)
	: Sprite(spriteMesh, spritePosition, spriteScale, spriteColor), texture{ spriteTexture } {
}

// Handles rendering of textured sprite
void TexturedSprite::RenderSprite(bool changeAlpha, f32 UVOffsetX, f32 UVOffsetY) {
		// Set sprite texture
		AEGfxTextureSet(texture, UVOffsetX, UVOffsetY);
		Sprite::RenderSprite(changeAlpha);
}

// Constructor for animated sprite
AnimatedSprite::AnimatedSprite(TexturedSprite texturedSprite, u32 spritesheetRows, u32 spritesheetCols, f32 initialOffsetX, f32 initialOffsetY)
	: TexturedSprite{texturedSprite}, spritesheetRows{spritesheetRows}, spritesheetCols{spritesheetCols}, spriteUVWidth{1.f/spritesheetCols}, spriteUVHeight{1.f/spritesheetRows},
	currentSpriteUVOffsetX{initialOffsetX}, initialOffsetX{initialOffsetX}, currentSpriteUVOffsetY{initialOffsetY}, initialOffsetY{initialOffsetY} {

}

// Add animation to animated sprite
void AnimatedSprite::SetAnimation(AnimationInfo animationInfo) {
	animations.push_back(animationInfo);
}

// Set current animation of animated sprite
void AnimatedSprite::GetAnimation(std::string animationName) {
	for (s32 i{}; i < animations.size(); ++i) {
		if (animations[i].name == animationName) {
			currentAnimation = animations[i];
			break;
		}
	}
}

// Update which frame animation is on
void AnimatedSprite::UpdateAnimation(f32 dt) {
	animationTimer += dt;
	// Duration for current frame has passed, move to next frame in animation
	if (animationTimer >= currentAnimation.frameDuration) {
		animationTimer = 0;

		// Get index of next frame
		currentSpriteIndex = ++currentSpriteIndex % currentAnimation.noFrames;

		// Get position of next frame
		u32 currentSpriteRow = currentAnimation.startRow;
		u32 currentSpriteCol = currentAnimation.startCol + currentSpriteIndex;

		// Move offsets to next frame
		currentSpriteUVOffsetX = spriteUVWidth * currentSpriteCol + initialOffsetX;
		currentSpriteUVOffsetY = spriteUVHeight * currentSpriteRow + initialOffsetY;
	}
}

// Create square mesh
AEGfxVertexList* CreateSquareMesh(f32 spriteUVWidth, f32 spriteUVHeight) {
		// Informing the library that we're about to start adding triangles
		AEGfxMeshStart();

		// This shape has 2 triangles that makes up a square
		// Color parameters represent colours as ARGB
		// UV coordinates to read from loaded textures
		AEGfxTriAdd(
			-0.5f, -0.5f, 0xFFFFFFFF, 0.0f, spriteUVHeight,  // bottom-left: red
			0.5f, -0.5f, 0xFFFFFFFF, spriteUVWidth, spriteUVHeight,   // bottom-right: green
			-0.5f, 0.5f, 0xFFFFFFFF, 0.0f, 0.0f);  // top-left: blue

		AEGfxTriAdd(
			0.5f, -0.5f, 0xFFFFFFFF, spriteUVWidth, spriteUVHeight,   // bottom-right: green
			0.5f, 0.5f, 0xFFFFFFFF, spriteUVWidth, 0.0f,    // top-right: white
			-0.5f, 0.5f, 0xFFFFFFFF, 0.0f, 0.0f);  // top-left: blue

		// Saving the mesh (list of triangles) in pMesh
		AEGfxVertexList* mesh = AEGfxMeshEnd();

		return mesh;
}

// Create circle mesh
AEGfxVertexList* CreateCircleMesh() {
		AEGfxMeshStart();

		for (f32 x{}; x <= 0.5f; x += 0.0025f) {
			f32 nextX{ x + 0.0025f };
			AEGfxTriAdd(
				0.0f, 0.0f, 0xFFFFFFFF, 0.5f, 0.5f,  // center of circle
				x, sqrtf(0.5f * 0.5f - x * x), 0xFFFFFFFF, 0.5f + x, 0.5f + sqrtf(0.5f * 0.5f - x * x),   // left end of triangle slice
				nextX, sqrtf(0.5f * 0.5f - nextX * nextX), 0xFFFFFFFF, 0.5f + nextX, 0.5f + sqrtf(0.5f * 0.5f - nextX * nextX));   // right end of triangle slice
			AEGfxTriAdd(
				0.0f, 0.0f, 0xFFFFFFFF, 0.5f, 0.5f,  // center of circle
				x, -sqrtf(0.5f * 0.5f - x * x), 0xFFFFFFFF, 0.5f + x, 0.5f - sqrtf(0.5f * 0.5f - x * x),   // left end of triangle slice
				nextX, -sqrtf(0.5f * 0.5f - nextX * nextX), 0xFFFFFFFF, 0.5f + nextX, 0.5f - sqrtf(0.5f * 0.5f - nextX * nextX));   // right end of triangle slice
			AEGfxTriAdd(
				0.0f, 0.0f, 0xFFFFFFFF, 0.5f, 0.5f,  // center of circle
				-x, sqrtf(0.5f * 0.5f - x * x), 0xFFFFFFFF, 0.5f - x, 0.5f + sqrtf(0.5f * 0.5f - x * x),   // left end of triangle slice
				-nextX, sqrtf(0.5f * 0.5f - nextX * nextX), 0xFFFFFFFF, 0.5f - nextX, 0.5f + sqrtf(0.5f * 0.5f - nextX * nextX));   // right end of triangle slice
			AEGfxTriAdd(
				0.0f, 0.0f, 0xFFFFFFFF, 0.5f, 0.5f,  // center of circle
				-x, -sqrtf(0.5f * 0.5f - x * x), 0xFFFFFFFF, 0.5f - x, 0.5f - sqrtf(0.5f * 0.5f - x * x),   // left end of triangle slice
				-nextX, -sqrtf(0.5f * 0.5f - nextX * nextX), 0xFFFFFFFF, 0.5f - nextX, 0.5f - sqrtf(0.5f * 0.5f - nextX * nextX));   // right end of triangle slice
		}

		AEGfxVertexList* mesh = AEGfxMeshEnd();

		return mesh;
}