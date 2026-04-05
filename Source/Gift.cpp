/* Start Header ************************************************************************/
/*!
\file     Gift.cpp
\author   Lee Hng Hwee Celest, hnghweecelest.l, 2502385
\par      hnghweecelest.l@digipen.edu
\brief    This file defines some functions that implement the gift

Copyright (C) 2026 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited.
*/
/* End Header **************************************************************************/

#include "Gift.h"
#include "Sprite.h"
#include "Player.h"
#include "BoundaryCollision.h"
#include "Utils/Utils.h"
#include "Collision.h"
#include "Loaders/DataLoader.h" // from testing
#include "rooms.h" //from main
#include <iostream>

//unordered map to store all the gift types taken from the json, using unordered map so that gifts can be found via their name
std::unordered_map<std::string, GiftType> allGiftTypes;

//default constructor for gift type (coded by Josiah)
GiftType::GiftType() :
	name{ "default" },
	traits{ },
	sprite{ DataLoader::CreateTexture("Assets/default.png") }
{
}

//constructor for gift type
GiftType::GiftType(std::string name, Labels traits, TexturedSprite sprite) :
	name {name},
	traits{traits},
	sprite{sprite}
{
}

//constructor for a gift
Gift::Gift(GiftType giftType, TexturedSprite shadowSprite, TexturedSprite hitboxSprite, Vector2 position) :
	giftType{ giftType },
	shadow{ shadowSprite },
	hitbox{hitboxSprite},
	position{ position },
	velocity{ Vector2 {0.f, 0.f} },
	pickUpState{ false },
	shakeState{ false }
{
}


//Updates the gift's position and handles collision (collision coded by Yee Kiat and MJ)
void UpdateGift(Gift & gift, Player & player, f32 deltaTime, Vector2 boundaries)
{

	//if player and gift are intersecting, pick up the gift
	if (CollisionIntersectionRectRectStatic(AABB{ player.position - player.sprite.scale / 2, player.position + player.sprite.scale / 2 },
		AABB{ gift.position - gift.giftType.sprite.scale / 2, gift.position + gift.giftType.sprite.scale / 2 }) && !player.pickUpState)
	{
		player.pickUpState = true;
		player.heldGift = &gift;
		gift.pickUpState = true;
		gift.velocity = Vector2{ 0.f, 0.f };
		//play the gift pick up audio when the player picks it up (coded by Brandon)
		PlayerPickUpAudio();
	}
	//if the gift is picked up, have it in front of the player
	if (gift.pickUpState)
	{
		gift.position = player.position + Vector2{ 80.f * player.direction.x, 80.f * player.direction.y };
	}
	//if the gift if not picked up, calculate its position based on its velocity, and handle collision
	else
	{
		
		gift.position += gift.velocity * deltaTime;

		//Gift collision handling (coded by Yee Kiat and MJ)
		int collideFlags = 0x00;

		if (CollisionBoundaryStatic(gift.position, gift.giftType.sprite.scale, (int)boundaries.x, (int)boundaries.y, collideFlags)) {
			// Left and Right collision inverse x
			if (collideFlags & 0x00000003) {
				gift.velocity.x *= -1;
			}
			// Top and Bottom inverse y
			if (collideFlags & 0x0000000C) {
				gift.velocity.y *= -1;
			}
			// Dampen bounce
			gift.velocity /= 1.3f;
		}
		else {
			gift.velocity /= 1.1;
		}
		
	}
	//if the gifts velocity is very low, set it to 0 (coded by Yee Kiat and MJ)
	if (gift.velocity.LengthSq() < 0.001f) gift.velocity = Vector2(0, 0);

	
	if (!gift.shakeState) {
		//set the gift's sprite position to match its actual position if not 
		//getting shaken
		gift.giftType.sprite.position = gift.position;
		gift.shadow.position = gift.position - Vector2{ 0, 40 };
		gift.hitbox.position = gift.position;
	}

}

//load all of the gift types from a json file
void LoadGiftTypes()
{
	//put the json file contents into json value
	Json::Value giftJson { DataLoader::LoadJsonFile("Assets/gifts.json") };
	allGiftTypes.clear();

	//for each gift in the giftTypes block in the json value
	for (Json::Value& gift : giftJson["giftTypes"])
	{
		//insert each gift trait from the json into temp's traits
		GiftType temp {gift["name"].asString(), {}, DataLoader::CreateTexture(gift["spritePath"].asString())};
		
		//insert each of the gifts traits into the gift's traits container
		for (Json::Value& trait : gift["traits"])
		{
			temp.traits.insert(trait.asString());
		}

		//insert key(which is used to identify and access the gift type) and tthe gift itself in allGiftTypes
		allGiftTypes.insert({ gift["name"].asString(), temp});
	}
}
 