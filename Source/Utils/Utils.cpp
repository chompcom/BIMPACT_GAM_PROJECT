#include "AEEngine.h"
#include "../Sprite.h"
#include <Windows.h>	// Windows


bool AreCirclesIntersecting(Vector2 circle1_Center, f32 circle1_Radius, Vector2 circle2_Center, f32 circle2_Radius) {
	f32 radius_Sum = circle1_Radius + circle2_Radius;

	return circle1_Center.DistanceSq(circle2_Center) <= radius_Sum * radius_Sum ? true : false;
}

//To test player picking up and throwing boxes
bool AreSquaresIntersecting(Vector2 square1_Center, float square1Length,
	Vector2 square2_Center, float square2Length)
{
	return ((fabs(square1_Center.x - square2_Center.x) < ((square1Length + square2Length) / 2.0f) &&
		fabs(square1_Center.y - square2_Center.y) < ((square1Length + square2Length) / 2.0f))) ? true : false;
}

f32 ClampInt(f32 value, f32 low, f32 high) {
	return AEClamp(value, low, high);
}

std::string ExtractFileName(const std::string& fullpath) {
	// 1. Get ONLY filename by finding the last slash
	size_t lastSlash = fullpath.find_last_of("\\/");
	std::string file = (lastSlash == std::string::npos) ? fullpath : fullpath.substr(lastSlash + 1);

	// 2. Get filename WITHOUT extension
	size_t lastDot = file.find_last_of('.');
	if (lastDot != std::string::npos) file.erase(lastDot);

	// 3. Return file name ONLY
	return file.empty() ? "Default" : file;
}