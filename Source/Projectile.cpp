#include "projectile.h"

Projectile::Projectile(TexturedSprite* projectileSprite, ProjectileType type, Vector2 velCurr, float lifeTime, int dmg)
	: projectileSprite(projectileSprite),type(type), velCurr(velCurr), lifeTime(lifeTime), dmg(dmg), isAlive(true) {}

Projectile::~Projectile() {
		delete projectileSprite;
		projectileSprite = nullptr;
	
}

void Projectile::ProjectileUpdate(f32 dt) {
	projectileSprite -> position.x += velCurr.x * dt;
	projectileSprite->position.y += velCurr.y * dt;
	lifeTime -= dt;
	if (lifeTime <= 0) {
		isAlive = false;
	}

	projectileSprite->UpdateTransform();
}
void Projectile::ProjectileRender() {
	projectileSprite->RenderSprite();
}

AABB Projectile::GetCollision() const {
	Vector2 half = projectileSprite->scale * 0.5f;
	return { projectileSprite->position - half, projectileSprite->position + half };
}

int Projectile::GetDmg() const {
	return dmg;
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
 
