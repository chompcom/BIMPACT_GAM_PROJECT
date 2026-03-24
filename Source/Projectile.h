#pragma once
#include "Sprite.h"
#include "Collision.h"
#include "Audio.h"
enum ProjectileType {
	BULLET,
	FIREBALL,
	AOE,
	ROUNDING,
	SCATTER,
	BOOMERANG
};
class Projectile {
public:
	Projectile(TexturedSprite projectileSprite,ProjectileType type, Vector2 velCurr, float lifeTime, int dmg,float rot = 0.0f,void* source = nullptr);
	~Projectile();
	void UpdateProjectile(f32 dt);
	void RenderProjectile();
	int GetDmg() const;
	Vector2 GetPosition() const;
	Vector2 GetScale() const;
	Vector2 GetVelocity() const;
	bool IsAlive() const;
	ProjectileType GetType() const;
	void RemoveProjectile();
	TexturedSprite GetSprite() const;

public: // flag for my projectiles
	bool isScatter = false;
	bool didScatter = false;

	const void* GetSource() const;

	Vector2 startPos;
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

	
};
