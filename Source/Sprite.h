#pragma once
#include "AEEngine.h"
#include "Utils/Vector2.hpp"
#include <vector>

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
		Sprite(AEGfxVertexList* spriteMesh, Vector2 spritePosition, Vector2 spriteScale, Color spriteColor = Color{0.0f,0.0f,0.0f,1.0f});

		void UpdateTransform();

		//void RenderSprite()rue);

		void RenderSprite(bool changeAlpha = false);
};

class TexturedSprite : public Sprite {
	public:
		AEGfxTexture* texture;

		TexturedSprite(AEGfxVertexList* spriteMesh, AEGfxTexture* spriteTexture, Vector2 spritePosition, Vector2 spriteScale, Color spriteColor = Color{ 1.0f,1.0f,1.0f,1.0f });

		void RenderSprite(bool changeAlpha = false, f32 uv_offsetX = 0.f, f32 uv_offsetY = 0.f);
};

struct AnimationInfo {
	std::string name;
	s32 start_row;
	s32 start_col;
	s32 no_frames;
	f32 frame_duration{ 0.25f };
};

class AnimatedSprite : public TexturedSprite {
	public:
		u32 spritesheet_rows;// = 3;
		u32 spritesheet_cols;// = 3;
		f32 sprite_uv_width;// = 1.f / spritesheet_cols;
		f32 sprite_uv_height;// = 1.f / spritesheet_rows;

		f32 animation_timer = 0.f;
		//f32 animation_duration_per_frame = 0.25f;

		//std::pair<u32, u32> current_animation_index = { 0, 0 };
		u32 current_sprite_index = 0;

		f32 current_sprite_uv_offset_x;
		f32 current_sprite_uv_offset_y;

		f32 initial_offsetX;
		f32 initial_offsetY;

		std::vector<AnimationInfo> animations;
		AnimationInfo currentAnimation;


		//AnimatedSprite(AEGfxVertexList* spriteMesh, AEGfxTexture* spriteTexture, Vector2 spritePosition, Vector2 spriteScale, Color spriteColor = Color{ 1.0f,1.0f,1.0f,1.0f }, f32 initial_offsetX, f32 initial_offsetY);
		AnimatedSprite(TexturedSprite texturedSprite, u32 spritesheetRows = 1, u32 spritesheetCols = 1, f32 initial_offsetX = 0.f, f32 initial_offsetY = 0.f);

		void SetAnimation(AnimationInfo animationInfo);
		void GetAnimation(std::string animationName);
		void UpdateAnimation(f32 dt);

		//void RenderSprite(bool changeAlpha = false);
};


AEGfxVertexList* CreateSquareMesh(f32 sprite_uv_width = 1.0f, f32 sprite_uv_height = 1.0f);

AEGfxVertexList* CreateCircleMesh();