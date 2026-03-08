#include "Sprite.h"
#include "Player.h"
#include "Gift.h"
#include "AEEngine.h"
#include "BoundaryCollision.h"
#include "Collision.h"
#include <iostream>

//contructor for player class
Player::Player(TexturedSprite sprite, f32 throwStrength, f32 speed, Vector2 position, Vector2 direction) :
	//initialiser list
	sprite{ sprite },
	throwStrength{ throwStrength },
	speed{ speed }, 
	health { 3 },
	position{ position },
	direction{ direction },
	pickUpState{ false },
	throwState{ false },
	heldGift{ nullptr },
	throwForce{ 0.f },
	pickUpCooldown{ 0.f },
	invulnerableTimer{ 0.f }
{
}

static bool fadingIn = false; //for the blinking effect when the player is immune

void UpdatePlayer(Player & player, f32 deltaTime)
{
	u8 w{ AEInputCheckCurr(AEVK_W) }, a{ AEInputCheckCurr(AEVK_A) },
		s{ AEInputCheckCurr(AEVK_S) }, d{ AEInputCheckCurr(AEVK_D) };

	//Let player blink if invulnerable
	if (player.invulnerableTimer > 0.f)
	{
		if (!fadingIn)
		{
			player.sprite.color.a -= (0.5f * deltaTime) * 10;
			if (player.sprite.color.a <= 0.2f) fadingIn = true;
		}
		else
		{
			player.sprite.color.a += (0.5f * deltaTime) * 10;
			if (player.sprite.color.a >= 1.f) fadingIn = false;
		}

		player.invulnerableTimer -= deltaTime;
	}
	else if (player.sprite.color.a < 1.f)
	{
		fadingIn = false;
		player.sprite.color.a = 1.f;
	}

	//set the player's pick up to false if there is still time in the pickUpCooldown
	if (player.pickUpCooldown > 0.f)
	{
		player.pickUpCooldown -= deltaTime;
		if (player.pickUpCooldown < 0.f) player.pickUpState = false;
	}

	//let player move if they are not throwing
	if (!player.throwState)
	{
		//player movement
		f32 adjustedSpeed = player.speed * deltaTime;
		
		player.position.y += (static_cast<float>(w) * adjustedSpeed) -
			(static_cast<float>(s) * adjustedSpeed);
		player.position.x += (static_cast<float>(d) * adjustedSpeed) -
			(static_cast<float>(a) * adjustedSpeed);

		CollisionBoundary_Static(player.position, player.sprite.scale, 1600, 900);

		//set the player's sprite position to match its actual position
		player.sprite.position = player.position;
	}

	//if player is trying to move, set their direction
	if (w || a || s || d)
	{
		//player.direction.y = float(w - s);
		//player.direction.x = float(d - a);
		player.direction = Vector2{(d - a) ,(w - s)};

		//If the player has no direction, set it to face down
		if (player.direction == Vector2{ 0.f, 0.f }) player.direction = Vector2{ 0.f, -1.f };
	}

	//if the player is holding an item and they press space, 
	//let them charge their throw
	if (player.pickUpState && player.heldGift != nullptr && 
		AEInputCheckCurr(AEVK_SPACE) && player.pickUpCooldown <= 0.0f)
	{
		//reset the held gift's position after shaking
		(*player.heldGift).sprite.position = (*player.heldGift).position;

		(*player.heldGift).shakeState = true;

		//Calculate the player's throwing force
		player.throwState = true;
		player.throwForce += deltaTime * player.throwStrength;
		player.throwForce = AEClamp(player.throwForce, 2000.f, 10000.f);

		//Shake the gift
		Vector2 shakeVector{ (static_cast<float>(rand()) / static_cast<float>(RAND_MAX)) * 2 - 1 , 
			(static_cast<float>(rand()) / static_cast<float>(RAND_MAX)) * 2 - 1 };

		shakeVector = shakeVector.Normalized() * (player.throwForce / 700.f);
		(*player.heldGift).sprite.position += shakeVector;

	}

	//if the player released their spacebar and they are holding a gift
	else if (AEInputCheckReleased(AEVK_SPACE) && player.heldGift != nullptr && player.pickUpState)
	{
		//reset the held gift's position after shaking
		(*player.heldGift).sprite.position = (*player.heldGift).position;
		(*player.heldGift).shakeState = false;
		//put down the gift and give it a velocity depenedent 
		//on how long the player has been charging up for
		player.throwState = false;
		player.pickUpCooldown = 0.5f;
		(*player.heldGift).pickUpState = false;
		(*player.heldGift).velocity = (player.direction).Normalized() * player.throwForce;
		player.heldGift = nullptr;
		player.throwForce = 0.f;
	}
}

//function for player to take damage
void playerTakesDamage(Player& player)
{
	if (player.invulnerableTimer <= 0.f)
	{
		--(player.health);

		player.invulnerableTimer = 3.f;
	}
}





