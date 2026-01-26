#pragma once

#include "Sprite.h"
#include <string>
#include <vector>
//forward declaration, needed as Player.h needs the Gift class, 
//and Gift.h needs the player class
class Player;

//simple gift class for testing
class Gift {
public:
	std::string name;
	std::vector<std::string> traits;
	Sprite sprite;
	Vector2 position;
	Vector2 velocity;
	bool pickUpState; //true if the gift is currently picked up

	//constructor
	Gift(std::string name, std::vector<std::string> traits, Sprite sprite, Vector2 position = Vector2{ 0.f, 0.f });
	Gift(Sprite sprite, Vector2 position = Vector2{ 0.f, 0.f });
};

void UpdateGift(Gift & gift, Player & player, f32 deltaTime);
