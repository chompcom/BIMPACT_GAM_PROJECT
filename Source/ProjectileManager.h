#pragma once
#include "Projectile.h"
#include "RoomData.h"
#include "Sprite.h"
#include "Loaders/DataLoader.h"

       
    void ShootProjectile(TexturedSprite sprite, RoomData& roomData, Vector2 pos, Vector2 dir, float speed, float lifetime, int damage, Vector2 scale, Color color);
    void ShootAOE(TexturedSprite sprite, RoomData& roomData, Vector2 pos, float speed, float lifetime, int damage, Vector2 scale, Color color); // doesn't take in dir as dir is auto set
    void Update(RoomData& roomData, float dt);
    void CheckProjectileCollision(RoomData& roomData, Player& player);
    void Render(RoomData& roomData);
    void Clear(RoomData& roomData);

