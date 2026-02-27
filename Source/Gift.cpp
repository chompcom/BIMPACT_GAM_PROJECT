#include "Gift.h"
#include "Sprite.h"
#include "Player.h"
#include "BoundaryCollision.h"
#include "Utils/Utils.h"
#include "Collision.h"
#include <iostream>

//simple contructor for gift class, for testing
Gift::Gift(std::string n, Labels t ,TexturedSprite sprite, Vector2 position) :
	//initialiser list
	name{ n },
	traits{ t },
	sprite{ sprite },
	position{ position },
	velocity { Vector2 {0.f, 0.f}},
	pickUpState{ false },
	shakeState{ false }
{
}

Gift::Gift(TexturedSprite sprite, Vector2 position) :
	name{ "Unnamed Gift" },
	traits{ },
	sprite{ sprite },
	position{ position },
	velocity{ Vector2 {0.f, 0.f} },
	pickUpState{ false },
	shakeState{ false }
{	
}



//this is just to test throwing
void UpdateGift(Gift & gift, Player & player, f32 deltaTime)
{
	//if player and gift are intersecting, pick up the gift
	if (CollisionIntersection_RectRect_Static(AABB{ player.position - player.sprite.scale / 2, player.position + player.sprite.scale / 2 },
		AABB{ gift.position - gift.sprite.scale / 2, gift.position + gift.sprite.scale / 2 }) && !player.pickUpState)
	{
		player.pickUpState = true;
		player.heldGift = &gift;
		gift.pickUpState = true;
		gift.velocity = Vector2{ 0.f, 0.f };
	}
	//if the gift is picked up, put it on the player's head
	if (gift.pickUpState)
	{
		gift.position = player.position + Vector2{ 0.f, 80.f };
	}
	//if not, calculate its position based on its velocity
	else
	{
		//gift.sprite.position = gift.position;
		gift.position += gift.velocity * deltaTime;
		if (CollisionBoundary_Static(gift.position, gift.sprite.scale, 1600, 900)) {
			gift.velocity /= -1.3;
		}
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
	if (!gift.shakeState) gift.sprite.position = gift.position;

	return;
}
 