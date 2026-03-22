#pragma once
#include "AEEngine.h"
#include "../Sprite.h"
#include "Vector2.hpp"

// ALL CONSTANTS???
#ifndef CONSTANTS
#define ASSETS_FOLDER (".\\Assets\\")
//#define LEVEL_PATH    (".\\Assets\\Levels\\levelInfo.json")
constexpr const char* LEVEL_PATH = ".\\Assets\\Levels\\levelInfo.json";
#endif // !LEVEL_PATH


bool AreCirclesIntersecting(Vector2 circle1_Center, f32 circle1_Radius, Vector2 circle2_Center, f32 circle2_Radius);

bool AreSquaresIntersecting(Vector2 square1_Center, float square1_Length,
	Vector2 square2_Center, float square2_Length);

f32 ClampInt(f32 value, f32 low, f32 high);

bool IsCursorInSquare(Vector2 square_Center, float squareLength);

bool IsCursorInRect(Vector2 rect_Center, float rect_Width, float rect_Length);
f32 ClampInt(f32 value, f32 low, f32 high);

std::string ExtractFileName(const std::string& fullpath, const std::string& last = ".");

