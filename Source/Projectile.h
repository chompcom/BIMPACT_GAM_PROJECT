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
	Projectile(TexturedSprite projectileSprite,ProjectileType type, Vector2 velCurr, float lifeTime, int dmg, void* source = nullptr);
	~Projectile();
	void ProjectileUpdate(f32 dt);
	void ProjectileRender();
	int GetDmg() const;
	Vector2 GetPosition() const;
	Vector2 GetScale() const;
	Vector2 GetVelocity() const;
	bool IsAlive() const;
	ProjectileType GetType() const;
	void RemoveProjectile();

	const void* GetSource() const;

	
private: 
	TexturedSprite projectileSprite;
	ProjectileType type;
	Vector2 velCurr;
	float lifeTime;
	int dmg;
	bool isAlive;	
	const void* sourceShot;
};
