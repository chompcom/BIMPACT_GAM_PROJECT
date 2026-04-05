/* Start Header ************************************************************************/
/*!
\file        ProjectileManager.h
\author     Brandon Choo, b.choo, 2501888
\par        b.choo@digipen.edu
\brief		This code declares the functions of each different types of projectile to use


Copyright (C) 2026 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited.
*/
/* End Header **************************************************************************/
#pragma once
#include "Projectile.h"
#include "RoomData.h"
#include "Sprite.h"
#include "Loaders/DataLoader.h"
#include "math.h"

 //----------------------------------------------------------------------------
//                      Declaration of different funcs of projectile types
//-----------------------------------------------------------------------------
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
