#pragma once
#include "Projectile.h"
#include "RoomData.h"
#include "Sprite.h"
#include "Loaders/DataLoader.h"
#include "math.h"

       
    void ShootProjectile(TexturedSprite sprite, RoomData& roomData, Vector2 pos, Vector2 dir, float speed, float lifetime, int damage, Vector2 scale, Color color, void* source = nullptr, bool isFriend = false);
    void ShootAOE(TexturedSprite sprite, RoomData& roomData, Vector2 pos, float speed, float lifetime, int damage, Vector2 scale, Color color,void* source = nullptr, bool isFriend = false); // doesn't take in dir as dir is auto set
    void ShootRounding(TexturedSprite sprite, RoomData& roomData, Vector2 pos, Vector2 dir, float speed, float lifetime, int damage, Vector2 scale, Color color, void* source = nullptr, bool isFriend = false, float rot = (60.f));
    void ShootScatter(TexturedSprite sprite, RoomData& roomData, Vector2 pos, Vector2 dir, float speed, float lifetime, int damage, Vector2 scale, Color color, void* source = nullptr, bool isFriend = false);
    void ShootBoomerang(TexturedSprite sprite, RoomData& roomData, Vector2 pos, Vector2 dir, float speed, float lifetime, int damage, Vector2 scale, Color color, void* source = nullptr, bool isFriend = false);
    void UpdateProjectiles(RoomData& roomData, float dt);
    void CheckProjectileCollision(RoomData& roomData, Player& player);
    void RenderProjectile(RoomData& roomData);
    void ProjectileClear(RoomData& roomData);

    void ProjectileParticleExplode(RoomData& roomData, Projectile const& proj);
