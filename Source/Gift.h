#pragma once

#include "Sprite.h"
//#include "Loaders/DataLoader.h"
#include <string>
#include <vector>
#include "Traits.h"
#include <unordered_map>

//forward declaration, needed as Player.h needs the Gift class, 
//and Gift.h needs the player class
class Player;
namespace mapRooms { struct Room; };

class GiftType {
public:
	std::string name;
	Labels traits;
	TexturedSprite sprite;

	GiftType();
	GiftType(std::string _name, Labels _traits, TexturedSprite _sprite);
};

//trying out unordered map so that you can find gifts by name 
//Thanks Josiah for introducing me to unordered map didn't know they existed until i saw your code
extern std::unordered_map<std::string, GiftType> allGiftTypes;

//praying this works
//simple gift class for testing
class Gift {
public:
	//std::string name;
	//Labels traits;
	//TexturedSprite sprite;
	GiftType giftType;
	TexturedSprite shadow;
	TexturedSprite hitbox;
	Vector2 position;
	Vector2 velocity;
	bool pickUpState; //true if the gift is currently picked up
	bool shakeState;

	//constructor
	//Gift(std::string name, Labels traits, TexturedSprite sprite, TexturedSprite shadowSprite, Vector2 position = Vector2{ 0.f, 0.f });
	Gift(GiftType giftType, TexturedSprite shadowSprite, TexturedSprite hitboxSprite, Vector2 position = Vector2{ 0.f, 0.f });

	//Gift(TexturedSprite sprite, TexturedSprite shadowSprite, Vector2 position = Vector2{ 0.f, 0.f });
};

void UpdateGift(Gift & gift, Player & player, f32 deltaTime, Vector2 boundaries, mapRooms::Room* curRoom);

void LoadGiftTypes();
