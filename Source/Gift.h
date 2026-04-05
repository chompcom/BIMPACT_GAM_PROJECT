/* Start Header ************************************************************************/
/*!
\file     Gift.h
\author   Lee Hng Hwee Celest, hnghweecelest.l, 2502385
\par      hnghweecelest.l@digipen.edu
\brief    This file declares some functions that implement the gift

Copyright (C) 2026 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited.
*/
/* End Header **************************************************************************/

#pragma once

#include "Sprite.h"
#include <string>
#include <vector>
#include "Traits.h"
#include <unordered_map>

//forward declaration, needed as Player.h needs the Gift class, 
//and Gift.h needs the player class
class Player;
namespace mapRooms { class Room; };


//class for the gift type 
class GiftType {
public:
	std::string name;
	Labels traits;
	TexturedSprite sprite;

	//gift type constructors
	GiftType();
	GiftType(std::string name, Labels traits, TexturedSprite sprite);
};

//unordered map so that you can find gifts by name 
//Thanks Josiah for introducing me to unordered map didn't know they existed until i saw your code
extern std::unordered_map<std::string, GiftType> allGiftTypes;

//gift class for testing
class Gift {
public:
	GiftType giftType;
	TexturedSprite shadow;
	TexturedSprite hitbox;
	Vector2 position;
	Vector2 velocity;
	bool pickUpState; //true if the gift is currently picked up
	bool shakeState;

	//constructor
	Gift(GiftType giftType, TexturedSprite shadowSprite, TexturedSprite hitboxSprite, Vector2 position = Vector2{ 0.f, 0.f });
};

//Updates the gift's position and handles collision (collision coded by Yee Kiat and MJ)
void UpdateGift(Gift & gift, Player & player, f32 deltaTime, Vector2 boundaries);

//load all of the gift types from a json file
void LoadGiftTypes();
