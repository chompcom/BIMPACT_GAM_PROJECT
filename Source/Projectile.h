/* Start Header ************************************************************************/
/*!
\file        Projectile.h
\author     Brandon Choo, 2501888
\par        b.choo@digipen.edu
\brief		This code declares the projectile type using enums for each type of projectile and the projectile class
			and the members and functions inside the class
			

Copyright (C) 2026 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited.
*/
/* End Header **************************************************************************/
#pragma once
#include "Sprite.h"
#include "Collision.h"
#include "Audio.h"
enum ProjectileType {
	BULLET, // normal shooting projectile
	FIREBALL,  
	AOE,  // shoots projectiles in a 360degree aoe 
	ROUNDING, // A swirl like projectile
	SCATTER,  // Projectile that shoots a scatter of projectile once its inital one died
	BOOMERANG
};
class Projectile {
public:
	Projectile(TexturedSprite projectileSprite,ProjectileType type, Vector2 velCurr, float lifeTime, int dmg,float rot = 0.0f,void* source = nullptr, bool fromFriend = false);
	~Projectile();
	void UpdateProjectile(f32 dt);
	void RenderProjectile();
	int GetDmg() const;
	Vector2 GetPosition() const;
	Vector2 GetScale() const;
	Vector2 GetVelocity() const;
	bool IsAlive() const;
	ProjectileType GetType() const;
	void RemoveProjectile();  // destroy projectile
	TexturedSprite GetSprite() const;

public: // flag for my projectiles
	bool isScatter = false;
	bool didScatter = false;

	const void* GetSource() const;

	Vector2 startPos;
	const bool friendProjectile; 
	bool boomerangSwitch = false;
private: 
	TexturedSprite projectileSprite;
	ProjectileType type;
	Vector2 velCurr;
	float lifeTime;
	int dmg;
	bool isAlive;	
	float rot;
	Vector2 startVel;
	const void* sourceShot;
	//Just a check if its shot from a friend, then it will hit enemies and not the player

	
};
