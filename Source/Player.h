#pragma once

#include "Sprite.h"
#include "Audio.h"

//forward declaration, needed as Player.h needs the Gift class, 
//and Gift.h needs the player class
class Gift;

class Player {
public:
	TexturedSprite sprite; 
	TexturedSprite shadow;
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
	Player(TexturedSprite playerSprite, TexturedSprite shadowSprite, f32 throwStrength, f32 speed, Vector2 position = Vector2{ 0.f, 0.f },
		Vector2 direction = Vector2 {0.f, -1.f});
		
	Vector2 GetVelocity() const;
};

void UpdatePlayer(Player & player, f32 deltaTime);

void playerTakesDamage(Player & player);

void playerHealsDamage(Player& player);

void PlayerInit(Player& player/*, mapRooms::Room* currentRoom*/);
