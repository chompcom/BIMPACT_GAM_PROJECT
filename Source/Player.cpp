/* Start Header ************************************************************************/
/*!
\file     Player.cpp
\author   Lee Hng Hwee Celest, hnghweecelest.l, 2502385
\par      hnghweecelest.l@digipen.edu
\brief    This file defines some functions that implement the player

Copyright (C) 2026 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited.
*/
/* End Header **************************************************************************/

#include "Sprite.h"
#include "Player.h"
#include "Gift.h"
#include "AEEngine.h"
#include "BoundaryCollision.h"
#include "Collision.h"
#include <iostream>
#include "GameStateList.h"
#include "Loaders/DataLoader.h"

extern LV_STATES gameState;

//contructor for player class
Player::Player(TexturedSprite playerSprite, TexturedSprite shadowSprite, TexturedSprite hitboxSprite, f32 throwStrength, f32 _speed, Vector2 position, Vector2 direction) :
	//initialiser list
	sprite{ playerSprite },
	shadow{ shadowSprite },
	hitbox{hitboxSprite},
	throwStrength{ throwStrength },
	speed{ 1.f },
	baseSpeed{ _speed },
	health{ 3 },
	position{ position },
	direction{ direction },
	pickUpState{ false },
	throwState{ false },
	heldGift{ nullptr },
	throwForce{ 0.f },
	throwMin{}, throwMax{},
	throwShakeModifier{},
	pickUpCooldown{ 0.f },
	invulnerableTimer{ 0.f },
	baseInvulnerable{ 0.f },
	isTargetable { true },
	fadingIn{ false }
{
}

//get the player's velocity (coded by Josiah)
Vector2 Player::GetVelocity() const {
	return direction * speed * baseSpeed;
}

//for the blinking effect when the player is immune
static bool fadingIn = false; 

//function to update the player
void UpdatePlayer(Player & player, f32 deltaTime)
{
	//variables to check for W, A, S, D inputs 
	u8 w{ AEInputCheckCurr(AEVK_W) }, a{ AEInputCheckCurr(AEVK_A) },
		s{ AEInputCheckCurr(AEVK_S) }, d{ AEInputCheckCurr(AEVK_D) };

	//ensure that the player can be targeted by the enemies (coded by Josiah)
	player.isTargetable = true;

	//Let player blink if invulnerable
	if (player.invulnerableTimer > 0.f)
	{
		//ensure that the player cannot be targeted by the enemies when invulnerable (coded by Josiah)
		player.isTargetable = false;
		//let the player fade in and out when invulnerable
		if (!player.fadingIn)
		{
			//decrease alpha to fade the player out
			player.sprite.color.a -= (0.5f * deltaTime) * 10;
			if (player.sprite.color.a <= 0.2f) player.fadingIn = true;
		}
		else
		{
			//increase alpha to fade the player in
			player.sprite.color.a += (0.5f * deltaTime) * 10;
			if (player.sprite.color.a >= 1.f) player.fadingIn = false;
		}
		//decrease the invulnerable timer
		player.invulnerableTimer -= deltaTime;
	}
	//ensure the player is completely opaque when not invulnerable
	else if (player.sprite.color.a < 1.f)
	{
		player.fadingIn = false;
		player.sprite.color.a = 1.f;
	}

	//set the player's pick up to false if there is still time in the pickUpCooldown so they don't immediately pick up gifts
	if (player.pickUpCooldown > 0.f)
	{
		player.pickUpCooldown -= deltaTime;
		if (player.pickUpCooldown < 0.f) player.pickUpState = false;
	}

	//let player move if they are not throwing
	if (!player.throwState)
	{
		//calculate player's adjusted speed (coded by Josiah)
		f32 adjustedSpeed = player.baseSpeed * player.speed * deltaTime;
		
		//change the player's position based on their inputs and their speed (w for up, a for left, s for down, d for up)
		player.position.y += (static_cast<float>(w) * adjustedSpeed) -
			(static_cast<float>(s) * adjustedSpeed);
		player.position.x += (static_cast<float>(d) * adjustedSpeed) -
			(static_cast<float>(a) * adjustedSpeed);

		//handles the player's collision (coded by Yee Kiat)
		CollisionBoundary_Static(player.position, player.sprite.scale, 1600, 900);

		//set the player's sprite position to match its actual position
		player.sprite.position = player.position;
		//also sets the player's shadow (coded by Yee Kiat)
		player.shadow.position = player.position - Vector2{ 0, 40 };
	}

	//footstep timer for footstep audio (coded by Brandon)
	static float footstepTimer = 0.0f; 
	static float footstepDelay = 0.5f;

	//if player is trying to move, set their direction
	if (w || a || s || d)
	{
		player.direction = Vector2{(d - a) ,(w - s)};

		//decrease footstep timer (coded by Brandon)
		footstepTimer -= deltaTime;
		//when footstep timer is up, play the audio and reset the timer (coded by Brandon)
		if (footstepTimer <= 0.0f)
		{
			PlayerFootstepAudio();
			footstepTimer = footstepDelay;
		}

		//i9f the player has no direction, set it to face down to ensure player always has a direction
		if (player.direction == Vector2{ 0.f, 0.f }) player.direction = Vector2{ 0.f, -1.f };
	}

	//if the player is holding an item and they press space, 
	//let them charge their throw
	if (player.pickUpState && player.heldGift != nullptr && 
		AEInputCheckCurr(AEVK_SPACE) && player.pickUpCooldown <= 0.0f)
	{
		//audio for charging the throw (coded by Brandon)
		ChargingThrowAudio(deltaTime);
		//reset the held gift's position after shaking
		(*player.heldGift).giftType.sprite.position = (*player.heldGift).position;
		//set the gift's shake state to true
		(*player.heldGift).shakeState = true;

		//calculate the player's throwing force and set their throw state to true
		player.throwState = true;
		player.throwForce += deltaTime * player.throwStrength;
		//clamp their throwing force
		player.throwForce = AEClamp(player.throwForce, player.throwMin, player.throwMax);

		//shake the gift by calculating the random vector to shake the gift by, and applying it to the gift sprite 
		Vector2 shakeVector{ (static_cast<float>(rand()) / static_cast<float>(RAND_MAX)) * 2 - 1 , 
			(static_cast<float>(rand()) / static_cast<float>(RAND_MAX)) * 2 - 1 };
		shakeVector = shakeVector.Normalized() * (player.throwForce * player.throwShakeModifier);
		(*player.heldGift).giftType.sprite.position += shakeVector;

	}

	//if the player released their spacebar and they are holding a gift
	else if (AEInputCheckReleased(AEVK_SPACE) && player.heldGift != nullptr && player.pickUpState)
	{
		//Stop playing the charging audio and play the throw audio (coded by Brandon)
		StopChargingAudio();
		PlayerThrowAudio();
		//reset the held gift's position after shaking
		(*player.heldGift).giftType.sprite.position = (*player.heldGift).position;
		(*player.heldGift).shakeState = false;
		//put down the gift and give it a velocity depenedent 
		//on how long the player has been charging up for
		player.throwState = false;
		//setsthe pick up cooldown so player wont automaticallly pick up the gift again
		player.pickUpCooldown = 0.5f;
		(*player.heldGift).pickUpState = false;
		(*player.heldGift).velocity = (player.direction).Normalized() * player.throwForce;
		player.heldGift = nullptr;
		player.throwForce = 0.f;
	}
	//reset player spped to 1 (coded by Josiah)
	player.speed = 1.0f;
}

//function for player to take damage
void PlayerTakesDamage(Player& player)
{
	//let player take damage if they are not invulnerable
	if (player.invulnerableTimer <= 0.f)
	{
		//play the player damage audio when the player is hurt (coded by Brandon)
		PlayerDmgAudio();
		//decrease player health if its above 0
		if (player.health > 0) --player.health;

		//if the player has 0 or less health, change the game's state to the lose state (coded by Yee Kiat)
		if (player.health <= 0) gameState = LOSE;

		//set the player's invulnerableTimer
		player.invulnerableTimer = player.baseInvulnerable;
	}
}

//function for player to heal damage if below 3 health
void PlayerHealsDamage(Player& player)
{
	if (player.health < 3) ++player.health;
}

//function to initialise the player via values from json (coded by Josiah)
void PlayerInit(Player& player)
{
	Json::Value source = DataLoader::LoadJsonFile("Assets/entityInfo.json")["player"];

	//put down gift
	if (player.heldGift)
	{
		player.heldGift->shakeState = false;
		player.heldGift->pickUpState = false;
		player.heldGift = nullptr;
	}

	player.health = static_cast<s16>(source["maxHearts"].asInt());
	player.pickUpState = false;
	player.throwState = false;
	player.sprite = DataLoader::CreateTexture(source["sprite"].asString());
	player.sprite.scale = Vector2(1, 1) * source["scale"].asFloat();
	player.position = Vector2{ 0.f,0.f };
	player.direction = Vector2{ 0.f, 0.f };
	player.baseSpeed = source["speed"].asFloat();
	player.speed = 1.0f; 
	player.throwStrength = source["throwStrength"].asFloat();
	player.throwShakeModifier = source["throwShakeModifier"].asFloat();
	player.throwMin = source["throwMin"].asFloat();
	player.throwMax = source["throwMax"].asFloat();
	player.throwForce = 0.f;


	player.pickUpCooldown = 0.f;
	player.invulnerableTimer = 0.f;
	player.baseInvulnerable = source["invulnTimer"].asFloat();
}


