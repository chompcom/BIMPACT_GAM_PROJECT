/* Start Header ************************************************************************/
/*!
\file     Player.h
\author   Lee Hng Hwee Celest, hnghweecelest.l, 2502385
\par      hnghweecelest.l@digipen.edu
\brief    This file declares some functions that implement the player

Copyright (C) 2026 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited.
*/
/* End Header **************************************************************************/


#pragma once

#include "Sprite.h"
#include "Audio.h"

//forward declaration, needed as Player.h needs the Gift class, 
//and Gift.h needs the player class
class Gift;

//The player class
class Player {
public:
	TexturedSprite sprite; 
	TexturedSprite shadow;
	TexturedSprite hitbox;
	f32 throwStrength; //throw strength modifier
	f32 speed; //movement speed modifier
	f32 baseSpeed; //movement speed modifier
	s16 health; 
	Vector2 position;
	Vector2 direction; //direction player is facing
	bool pickUpState; //true if the player is holding a gift
	bool throwState; //true is player is throwing a gift
	Gift * heldGift; //points to the gift the player is holding
	f32 throwForce; //amount of force that will be applied to the gift thrown
	f32 throwMin, throwMax; //minimum and maximum force to apply
	f32 throwShakeModifier; //modify how much the force will be reflected in the shake
	f32 pickUpCooldown; //counter for when the player can pick up a gift
	f32 invulnerableTimer;
	f32 baseInvulnerable;
	bool isTargetable; //When invulntimer is > 0, this is false
	bool fadingIn;
	
	//constructor
	Player(TexturedSprite playerSprite, TexturedSprite shadowSprite, TexturedSprite hitboxSprite, f32 throwStrength, f32 speed, Vector2 position = Vector2{ 0.f, 0.f },
		Vector2 direction = Vector2 {0.f, -1.f});
	//get the player's velocity (coded by Josiah)
	Vector2 GetVelocity() const;
};

//function to update the player
void UpdatePlayer(Player & player, f32 deltaTime);

//function for player to take damage
void PlayerTakesDamage(Player & player);

//function for player to heal damage if below 3 health
void PlayerHealsDamage(Player& player);

//function to initialise the player via values from json (coded by Josiah)
void PlayerInit(Player& player);
