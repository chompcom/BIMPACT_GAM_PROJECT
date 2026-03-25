#include "projectile.h"
#include "AEEngine.h"
#include <iostream>
Projectile::Projectile(TexturedSprite projectileSprite, ProjectileType type, Vector2 velCurr, float lifeTime, int dmg,float rot, void* source, bool fromFriend)
	: projectileSprite(projectileSprite),type(type), velCurr(velCurr),startVel(velCurr), lifeTime(lifeTime), dmg(dmg), isAlive(true), rot(rot), sourceShot(source), startPos(projectileSprite.position), friendProjectile(fromFriend),
 boomerangSwitch (false) {}

Projectile::~Projectile() {
	
	
}

void Projectile::UpdateProjectile(f32 dt) {
	Vector2 prevPos = projectileSprite.position;
	if (rot != 0.0f && GetType() == ROUNDING) {
		float angle = AEDegToRad(rot) * dt;
		float roundVelX = velCurr.x * AECos(angle) - velCurr.y * AESin(angle);
		float roundVelY = velCurr.x * AESin(angle) + velCurr.y * AECos(angle);
		velCurr.x = roundVelX;
		velCurr.y = roundVelY;
	} 
	if (GetType() == BOOMERANG && !boomerangSwitch) {
		float dist = startPos.Distance(projectileSprite.position);
		float angleBoom = AEDegToRad(rot) * dt;
		float boomVelX = velCurr.x * AECos(angleBoom) - velCurr.y * AESin(angleBoom);
		float boomVelY = velCurr.x * AESin(angleBoom) + velCurr.y * AECos(angleBoom);
		velCurr.x = boomVelX;
		velCurr.y = boomVelY;
		if (dist > 400.0f) {
			velCurr = -velCurr;
			boomerangSwitch = true;
		}
	}
	projectileSprite.position.x += velCurr.x * dt + startVel.x * dt * 0.5f;
	projectileSprite.position.y += velCurr.y * dt + startVel.y * dt * 0.5f;

	
	
	std::cout << projectileSprite.position.x - prevPos.x << " " << prevPos.y << std::endl;

	lifeTime -= dt;
	if (lifeTime <= 0) {
		isAlive = false;
		//StopRoundingAudio();
	}
	projectileSprite.UpdateTransform();
}
void Projectile::RenderProjectile() {
	projectileSprite.RenderSprite();
}

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