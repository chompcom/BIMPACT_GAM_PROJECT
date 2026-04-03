#include "Gift.h"
#include "Sprite.h"
#include "Player.h"
#include "BoundaryCollision.h"
#include "Utils/Utils.h"
#include "Collision.h"
#include "Loaders/DataLoader.h" // from testing
#include "rooms.h" //from main
#include <iostream>

//simple contructor for gift class, for testing
//Gift::Gift(std::string n, Labels t ,TexturedSprite sprite, TexturedSprite shadowSprite, Vector2 position) :
//	//initialiser list
//	name{ n },
//	traits{ t },
//	sprite{ sprite },
//	shadow{ shadowSprite },
//	position{ position },
//	velocity { Vector2 {0.f, 0.f}},
//	pickUpState{ false },
//	shakeState{ false }
//{
//}
//
//Gift::Gift(TexturedSprite sprite, TexturedSprite shadowSprite, Vector2 position) :
//	name{ "Unnamed Gift" },
//	traits{ },
//	sprite{ sprite },
//	shadow{ shadowSprite },
//	position{ position },
//	velocity{ Vector2 {0.f, 0.f} },
//	pickUpState{ false },
//	shakeState{ false }
//{	
//}
std::unordered_map<std::string, GiftType> allGiftTypes;

GiftType::GiftType() :
	name{ "default" },
	traits{ },
	sprite{ DataLoader::CreateTexture("Assets/default.png") }
{

}

GiftType::GiftType(std::string _name, Labels _traits, TexturedSprite _sprite) :
	name {_name},
	traits{_traits},
	sprite{_sprite}
{
}

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


//this is just to test throwing
void UpdateGift(Gift & gift, Player & player, f32 deltaTime, Vector2 boundaries, mapRooms::Room* curRoom)
{
	UNREFERENCED_PARAMETER(curRoom);
	//if player and gift are intersecting, pick up the gift
	if (CollisionIntersection_RectRect_Static(AABB{ player.position - player.sprite.scale / 2, player.position + player.sprite.scale / 2 },
		AABB{ gift.position - gift.giftType.sprite.scale / 2, gift.position + gift.giftType.sprite.scale / 2 }) && !player.pickUpState)
	{
		player.pickUpState = true;
		player.heldGift = &gift;
		gift.pickUpState = true;
		gift.velocity = Vector2{ 0.f, 0.f };
		PlayerPickUpAudio();
	}
	//if the gift is picked up, put it on the player's head
	if (gift.pickUpState)
	{
		gift.position = player.position + Vector2{ 80.f * player.direction.x, 80.f * player.direction.y };
	}
	//if not, calculate its position based on its velocity
	else
	{
		//gift.sprite.position = gift.position;
		gift.position += gift.velocity * deltaTime;

		int collideFlags = 0x00;

		if (CollisionBoundary_Static(gift.position, gift.giftType.sprite.scale, (int)boundaries.x, (int)boundaries.y, collideFlags)) {
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
		//else if (curRoom->roomGrid.CheckMapGridCollision(gift.position.x, gift.position.y, gift.sprite.scale.x, gift.sprite.scale.y, -1)) {
		//	collideFlags = curRoom->roomGrid.CheckMapGridCollision(gift.position.x, gift.position.y, gift.sprite.scale.x, gift.sprite.scale.y, -1);

		//	// Left and Right collision inverse x
		//	if (collideFlags & 0x00000003) {
		//		gift.velocity.x *= -1;
		//	}
		//	// Top and Bottom inverse y
		//	if (collideFlags & 0x0000000C) {
		//		gift.velocity.y *= -1;
		//	}
		//	// Dampen bounce
		//	gift.velocity /= 1.3f;
		//}
		else {
			gift.velocity /= 1.1;
		}

		//set the gift's sprite position to match its actual position if not 
		//getting shaken
		
	}
	if (gift.velocity.LengthSq() < 0.001f) gift.velocity = Vector2(0, 0);
	//set the gift's sprite position to match its actual position
	//gift.sprite.position = gift.position;

	//set the gift's sprite position to match its actual position if not 
	//getting shaken
	if (!gift.shakeState) {
		gift.giftType.sprite.position = gift.position;
		gift.shadow.position = gift.position - Vector2{ 0, 40 };
	}

	return;
}

void LoadGiftTypes()
{
	//put the json file contents into json value
	Json::Value giftJson { DataLoader::LoadJsonFile("Assets/gifts.json") };
	allGiftTypes.clear();

	//for each gift in the giftTypes block in the json value
	for (Json::Value& gift : giftJson["giftTypes"])
	{
		GiftType temp {gift["name"].asString(), {}, DataLoader::CreateTexture(gift["spritePath"].asString())};
		//insert each gift trait from the json into temp's traits

		for (Json::Value& trait : gift["traits"])
		{
			temp.traits.insert(trait.asString());
		}

		//insert key(which is used to identify and access the gift type) and tthe gift itself in allGiftTypes
		allGiftTypes.insert({ gift["name"].asString(), temp});
	}
}
 