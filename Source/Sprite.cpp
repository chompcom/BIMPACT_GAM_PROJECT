#include "AEEngine.h"
#include "Sprite.h"

#include <iostream>

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

//void Sprite::RenderSprite() {
//		AEGfxSetColorToMultiply(color.r, color.g, color.b, color.a);
//		AEGfxSetColorToAdd(0.f, 0.f, 0.f, 1.f);
//		AEGfxSetTransform(transform.m);
//		AEGfxMeshDraw(mesh, AE_GFX_MDM_TRIANGLES);
//}

//changed this slightly so that the sprite alpha can be changed if needed
void Sprite::RenderSprite(bool changeAlpha) {
	AEGfxSetColorToMultiply(color.r, color.g, color.b, color.a);
	
	if (!changeAlpha)
	{
		AEGfxSetColorToAdd(0.f, 0.f, 0.f, 1.f);
	}
	else 
	{
		AEGfxSetColorToAdd(0.f, 0.f, 0.f, 0.f);
	}
	//AEGfxSetColorToAdd(0.f, 0.f, 0.f, 1.f);
	AEGfxSetTransform(transform.m);
	AEGfxMeshDraw(mesh, AE_GFX_MDM_TRIANGLES);
}

TexturedSprite::TexturedSprite(AEGfxVertexList* spriteMesh, AEGfxTexture* spriteTexture, Vector2 spritePosition, Vector2 spriteScale, Color spriteColor)
	: Sprite(spriteMesh, spritePosition, spriteScale, spriteColor), texture{ spriteTexture } {
}

void TexturedSprite::RenderSprite(bool changeAlpha, f32 uv_offsetX, f32 uv_offsetY) {
		AEGfxTextureSet(texture, uv_offsetX, uv_offsetY);
		Sprite::RenderSprite(changeAlpha);
}

/*AnimatedSprite::AnimatedSprite(AEGfxVertexList* spriteMesh, AEGfxTexture* spriteTexture, Vector2 spritePosition, Vector2 spriteScale, Color spriteColor, f32 initial_offsetX, f32 initial_offsetY)
	: TexturedSprite(spriteMesh, spriteTexture, spritePosition, spriteScale, spriteColor), current_sprite_uv_offset_x{ initial_offsetX }, current_sprite_uv_offset_y{ initial_offsetY } {

}*/

AnimatedSprite::AnimatedSprite(TexturedSprite texturedSprite, u32 spritesheetRows, u32 spritesheetCols, f32 initial_offsetX, f32 initial_offsetY)
	: TexturedSprite{texturedSprite}, spritesheet_rows{spritesheetRows}, spritesheet_cols{spritesheetCols}, sprite_uv_width{1.f/spritesheetCols}, sprite_uv_height{1.f/spritesheetRows},
	current_sprite_uv_offset_x{initial_offsetX}, initial_offsetX{initial_offsetX}, current_sprite_uv_offset_y{initial_offsetY}, initial_offsetY{initial_offsetY} {

}

void AnimatedSprite::SetAnimation(AnimationInfo animationInfo) {
	animations.push_back(animationInfo);
}

void AnimatedSprite::GetAnimation(std::string animationName) {
	for (s32 i{}; i < animations.size(); ++i) {
		if (animations[i].name == animationName) {
			currentAnimation = animations[i];
			break;
		}
	}
}

void AnimatedSprite::UpdateAnimation(f32 dt) {
	//if (currentAnimation.no_frames > 1) {
		animation_timer += dt;
		if (animation_timer >= currentAnimation.frame_duration) {
			animation_timer = 0;

			current_sprite_index = ++current_sprite_index % currentAnimation.no_frames;

			u32 current_sprite_row = currentAnimation.start_row;
			u32 current_sprite_col = currentAnimation.start_col + current_sprite_index;

			current_sprite_uv_offset_x = sprite_uv_width * current_sprite_col + initial_offsetX;
			current_sprite_uv_offset_y = sprite_uv_height * current_sprite_row + initial_offsetY;
		}
	//}
}

AEGfxVertexList* CreateSquareMesh(f32 sprite_uv_width, f32 sprite_uv_height) {
		// Informing the library that we're about to start adding triangles
		AEGfxMeshStart();

		// This shape has 2 triangles that makes up a square
		// Color parameters represent colours as ARGB
		// UV coordinates to read from loaded textures
		AEGfxTriAdd(
			-0.5f, -0.5f, 0xFFFFFFFF, 0.0f, sprite_uv_height,  // bottom-left: red
			0.5f, -0.5f, 0xFFFFFFFF, sprite_uv_width, sprite_uv_height,   // bottom-right: green
			-0.5f, 0.5f, 0xFFFFFFFF, 0.0f, 0.0f);  // top-left: blue

		AEGfxTriAdd(
			0.5f, -0.5f, 0xFFFFFFFF, sprite_uv_width, sprite_uv_height,   // bottom-right: green
			0.5f, 0.5f, 0xFFFFFFFF, sprite_uv_width, 0.0f,    // top-right: white
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