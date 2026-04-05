/* Start Header ************************************************************************/
/*!
\file        Projectile.cpp
\author     Brandon Choo, b.choo, 2501888
\par        b.choo@digipen.edu
\brief		This code implements the Projectile class, handling movement updates for standard,
			 rounding, and boomerang projectile types, and also rendering, projectile lifetime management, 
			 and accessor methods for projectile properties.

Copyright (C) 2026 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited.
*/
/* End Header **************************************************************************/
#include "Projectile.h"
#include "AEEngine.h"

Projectile::Projectile(TexturedSprite projectileSprite, ProjectileType type, Vector2 velCurr, float lifeTime, int dmg,float rot, void* source, bool fromFriend)
	: projectileSprite(projectileSprite),type(type), velCurr(velCurr),startVel(velCurr), lifeTime(lifeTime), dmg(dmg), isAlive(true), rot(rot), sourceShot(source), startPos(projectileSprite.position), friendProjectile(fromFriend),
 boomerangSwitch (false) {}

Projectile::~Projectile() {
	
	
}

void Projectile::UpdateProjectile(f32 dt) {
	Vector2 prevPos = projectileSprite.position;
	// if my projectile is a rounding projectile and its rotation is 0 rotate vel for my projectile 

	if (rot != 0.0f && GetType() == ROUNDING) {
		float angle = AEDegToRad(rot) * dt;
		float roundVelX = velCurr.x * AECos(angle) - velCurr.y * AESin(angle);
		float roundVelY = velCurr.x * AESin(angle) + velCurr.y * AECos(angle); 
		// basically this equation causes a spiral like rotation for my projectile 
		velCurr.x = roundVelX;
		velCurr.y = roundVelY;
	} 
	if (GetType() == BOOMERANG && !boomerangSwitch) { 
		// boomerang projectile where once it reaches the max distance it will reverse back like a boomerang
		float dist = startPos.Distance(projectileSprite.position);
		float angleBoom = AEDegToRad(rot) * dt;
		float boomVelX = velCurr.x * AECos(angleBoom) - velCurr.y * AESin(angleBoom);
		float boomVelY = velCurr.x * AESin(angleBoom) + velCurr.y * AECos(angleBoom);
		velCurr.x = boomVelX;
		velCurr.y = boomVelY;
		if (dist > 400.0f) { // set the max distance before it switches direction 
			velCurr = -velCurr;
			boomerangSwitch = true;
		}
	}
	projectileSprite.position.x += velCurr.x * dt + startVel.x * dt * 0.5f;
	projectileSprite.position.y += velCurr.y * dt + startVel.y * dt * 0.5f;

	lifeTime -= dt;
	if (lifeTime <= 0) {
		isAlive = false;
	}
	projectileSprite.UpdateTransform();
}
void Projectile::RenderProjectile() {
	//add true so it can be transparent
	projectileSprite.RenderSprite(true);
}
//-----------------------------------------------------------
//           My accessors for projectile
//-----------------------------------------------------------
int Projectile::GetDmg() const {
	return dmg;
}
Vector2 Projectile::GetPosition() const { 
	return projectileSprite.position; 
}
Vector2 Projectile::GetScale()    const {
	return projectileSprite.scale; 
}
Vector2 Projectile::GetVelocity() const { 
	return velCurr; 
}
bool Projectile::IsAlive() const {
	return isAlive;	
}

ProjectileType Projectile::GetType() const {
	return type;
}

void Projectile::RemoveProjectile() {
	isAlive = false;
	
}
const void* Projectile::GetSource() const{
	return sourceShot;
}
 
TexturedSprite Projectile::GetSprite() const {
	return projectileSprite;
}