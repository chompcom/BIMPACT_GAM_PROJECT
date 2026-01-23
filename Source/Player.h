#pragma once

#include "Sprite.h"

//forward declaration, needed as Player.h needs the Gift class, 
//and Gift.h needs the player class
class Gift;

class Player {
public:
	Sprite sprite; 
	f32 throwStrength; //throw strength modifier
	f32 speed; //movement speed modifier
	Vector2 position;
	Vector2 direction; //direction player is facing
	bool pickUpState; //true if the player is holding a gift
	bool throwState; //true is player is throwing a gift
	Gift * heldGift; //points to the gift the player is holding
	f32 throwForce; //amount of force that will be applied to the gift thrown
	
	//constructor
	Player(Sprite sprite, f32 throwStrength, f32 speed, Vector2 position = Vector2{ 0.f, 0.f },
		Vector2 direction = Vector2 {0.f, -1.f});
};

void UpdatePlayer(Player & player, f32 deltaTime);
