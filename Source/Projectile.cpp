#include "projectile.h"
#include "AEEngine.h"
Projectile::Projectile(TexturedSprite projectileSprite, ProjectileType type, Vector2 velCurr, float lifeTime, int dmg,float rot)
	: projectileSprite(projectileSprite),type(type), velCurr(velCurr),startVel(velCurr), lifeTime(lifeTime), dmg(dmg), isAlive(true), rot(rot) {}

Projectile::~Projectile() {
	
	
}

void Projectile::UpdateProjectile(f32 dt) {
	
	if (rot != 0.0f) {
		float angle = AEDegToRad(rot) * dt;
		float tmpVelX = velCurr.x * AECos(angle) - velCurr.y * AESin(angle);
		float tmpVelY = velCurr.x * AESin(angle) + velCurr.y * AECos(angle);
		velCurr.x = tmpVelX;
		velCurr.y = tmpVelY;
	} 
	projectileSprite.position.x += velCurr.x * dt + startVel.x * dt * 0.5f;
	projectileSprite.position.y += velCurr.y * dt + startVel.y * dt * 0.5f;
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
 
TexturedSprite Projectile::GetSprite() const {
	return projectileSprite;
}