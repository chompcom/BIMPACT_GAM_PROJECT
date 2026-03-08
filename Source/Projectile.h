#pragma once
#include "Sprite.h"
#include "Collision.h"
enum ProjectileType {
	BULLET,
	FIREBALL,
	AOE
};
class Projectile {
public:
	Projectile(TexturedSprite* projectileSprite,ProjectileType type, Vector2 velCurr, float lifeTime, int dmg);
	~Projectile();
	void ProjectileUpdate(f32 dt);
	void ProjectileRender();
	AABB GetCollision() const;
	int GetDmg() const;
	bool IsAlive() const;
	ProjectileType GetType() const;
	void RemoveProjectile();

	
private: 
	TexturedSprite* projectileSprite;
	ProjectileType type;
	Vector2 velCurr;
	float lifeTime;
	int dmg;
	bool isAlive;	
};
