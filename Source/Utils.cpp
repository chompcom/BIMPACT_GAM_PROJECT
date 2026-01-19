#include "AEEngine.h"
#include "Sprite.hpp"

bool AreCirclesIntersecting(Vector2D circle1_Center, f32 circle1_Radius, Vector2D circle2_Center, f32 circle2_Radius) {
	f32 distance = sqrt(pow(circle2_Center.x - circle1_Center.x,2.f)+pow(circle2_Center.y - circle1_Center.y,2.f));
	f32 radius_Sum = circle1_Radius + circle2_Radius;

	return distance <= radius_Sum ? true : false;
}