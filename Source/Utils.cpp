#include "AEEngine.h"
#include "Sprite.h"

bool AreCirclesIntersecting(Vector2 circle1_Center, f32 circle1_Radius, Vector2 circle2_Center, f32 circle2_Radius) {
	f32 radius_Sum = circle1_Radius + circle2_Radius;

	return circle1_Center.DistanceSq(circle2_Center) <= radius_Sum*radius_Sum ? true : false;
}