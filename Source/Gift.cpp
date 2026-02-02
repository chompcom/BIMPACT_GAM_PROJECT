#include "Gift.h"
#include "Sprite.h"
#include "Player.h"
#include "Utils.h"
#include "BoundaryCollision.h"

//simple contructor for gift class, for testing
Gift::Gift(std::string n, std::vector<std::string> t ,Sprite sprite, Vector2 position) :
	//initialiser list
	name{ n },
	traits{ t },
	sprite{ sprite },
	position{ position },
	velocity { Vector2 {0.f, 0.f}},
	pickUpState{ false }
{
}

Gift::Gift(Sprite sprite, Vector2 position) :
	name{ "Unnamed Gift" },
	traits{ },
	sprite{ sprite },
	position{ position },
	velocity{ Vector2 {0.f, 0.f} },
	pickUpState{ false }
{	
}



//this is just to test throwing
void UpdateGift(Gift & gift, Player & player, f32 deltaTime)
{
	//if player and gift are intersecting, pick up the gift
	if (AreSquaresIntersecting(player.position, player.sprite.scale.x,
		gift.position, gift.sprite.scale.x) && !player.pickUpState)
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
		gift.position += gift.velocity * deltaTime;
		if (CollisionBoundary_Static(gift.position, gift.sprite.scale, 1600, 900)) {
			gift.velocity.x /= -1.3;
			gift.velocity.y /= -1.3;
		}
		else {
			gift.velocity.x /= 1.1;
			gift.velocity.y /= 1.1;
		}
	}
	//set the gift's sprite position to match its actual position
	gift.sprite.position = gift.position;

	return;
}
 