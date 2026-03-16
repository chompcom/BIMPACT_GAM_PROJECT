#include "projectile.h"

Projectile::Projectile(TexturedSprite projectileSprite, ProjectileType type, Vector2 velCurr, float lifeTime, int dmg)
	: projectileSprite(projectileSprite),type(type), velCurr(velCurr), lifeTime(lifeTime), dmg(dmg), isAlive(true) {}

Projectile::~Projectile() {
	
	
}

void Projectile::ProjectileUpdate(f32 dt) {
	projectileSprite.position.x += velCurr.x * dt;
	projectileSprite.position.y += velCurr.y * dt;
	lifeTime -= dt;
	if (lifeTime <= 0) {
		isAlive = false;
	}

	projectileSprite.UpdateTransform();
}
void Projectile::ProjectileRender() {
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
 
