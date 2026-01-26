#pragma once

#include "Sprite.h"

//forward declaration, needed as Player.h needs the Gift class, 
//and Gift.h needs the player class
class Player;

//simple gift class for testing
class Gift {
public:
	Sprite sprite;
	Vector2 position;
	Vector2 velocity;
	bool pickUpState; //true if the gift is currently picked up

	//constructor
	Gift(Sprite sprite, Vector2 position = Vector2{ 0.f, 0.f });
};

void UpdateGift(Gift & gift, Player & player, f32 deltaTime);
