/* Start Header ************************************************************************/
/*!
\file        ProjectileManager.cpp
\author     Brandon Choo, b.choo, 2501888
\par        b.choo@digipen.edu
\brief		This code implements the spawning functions for all projectile types, and handles updates the projectile every frame.
            Collision detection of projectile against enemies/players with projectile, and Josiah's particle effects too.

Copyright (C) 2026 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited.
*/
/* End Header **************************************************************************/
#include "ProjectileManager.h"

#include "ParticleSystem.h"

// Spawns a standard fireball projectile in the given direction
void ShootProjectile(TexturedSprite sprite, RoomData& roomData, Vector2 pos, Vector2 dir, float speed, float lifetime, int damage, Vector2 scale, Color color, void* source, bool isFriend) {
	sprite.position = pos;
	sprite.scale = scale;
	sprite.color = color;
	Projectile* fireball = new Projectile(sprite, FIREBALL, dir * speed, lifetime, damage,0.0f, source, isFriend);
    roomData.projectileList.push_back(fireball);
    ProjectileAudio();
}

// Spawns a 360 spread of projectiles
void ShootAOE(TexturedSprite sprite, RoomData& roomData, Vector2 pos, float speed, float lifetime, int damage, Vector2 scale, Color color, void* source, bool isFriend) {
    int numProjectiles = 10;
    float angleAround = 360.0f / numProjectiles;  

    for (int i = 0; i < numProjectiles; i++) {
        float angle = AEDegToRad(angleAround * i);

        Vector2 shotDir;      
        shotDir.x = AECos(angle);
        shotDir.y = AESin(angle);
		sprite.scale = scale;
		sprite.position = pos;
		sprite.color = color;

        Vector2 velocity = shotDir * speed;  // uses shotDir
        Projectile* aoe = new Projectile(sprite, AOE, velocity, lifetime, damage,0.0f, source, isFriend);
        roomData.projectileList.push_back(aoe);
    }
}

// Spawns a projectile that curves like a spiral with the given rotation rate
void ShootRounding(TexturedSprite sprite, RoomData& roomData, Vector2 pos, Vector2 dir, float speed, float lifetime, int damage, Vector2 scale, Color color, void* source, bool isFriend, float rot) {
    sprite.position = pos;
    sprite.scale = scale;
    sprite.color = color;
  
    Projectile* rounding = new Projectile(sprite, ROUNDING, dir * speed, lifetime, damage,rot,source, isFriend);
    roomData.projectileList.push_back(rounding);
    RoundingProjectileAudio();
}

// Spawns a scatter projectile that explodes into AOE projectile once the inital projectile dies
void ShootScatter(TexturedSprite sprite, RoomData& roomData, Vector2 pos, Vector2 dir, float speed, float lifetime, int damage, Vector2 scale, Color color,void* source, bool isFriend) {
    sprite.position = pos;
    sprite.scale = scale;
    sprite.color = color;

    Projectile* scatter = new Projectile(sprite, SCATTER, dir * speed, lifetime, damage,0.0f, source, isFriend);
    scatter->isScatter = true;
    roomData.projectileList.push_back(scatter);  
    
}

// Spawns a boomerang projectile
void ShootBoomerang(TexturedSprite sprite, RoomData& roomData, Vector2 pos, Vector2 dir, float speed, float lifetime, int damage, Vector2 scale, Color color, void* source, bool isFriend) {
    sprite.position = pos;
    sprite.scale = scale;
    sprite.color = color;
    Projectile* boomerang = new Projectile(sprite, BOOMERANG, dir * speed, lifetime, damage, 30.0f, source, isFriend);
    roomData.projectileList.push_back(boomerang);

}

// Updates all projectiles each frame and handles scatter expansion and removes dead projectiles
void UpdateProjectiles(RoomData& roomData, float dt) {
    std::vector<Projectile*> rmData;

    for (auto it = roomData.projectileList.begin(); it != roomData.projectileList.end();) {
        (*it)->UpdateProjectile(dt);
        if (!(*it)->IsAlive()) {
            // Scatter projectile spawns AOE on death
            if ((*it)->isScatter && !(*it)->didScatter) {
                int numProjectiles = 10;
                float angleAround = 360.0f / numProjectiles;
                for (int i = 0; i < numProjectiles; i++) {
                    float angle = AEDegToRad(angleAround * i);
                    Vector2 shotDir = { AECos(angle), AESin(angle) };
                    TexturedSprite spr = (*it)->GetSprite(); 
                    Projectile* aoe = new Projectile(spr, AOE, shotDir * 200.0f, 1.0f, (*it)->GetDmg());
                    rmData.push_back(aoe);
                }
            }
                delete* it;
                it = roomData.projectileList.erase(it);
            }
            else {
                ++it;
            }
        }
    for(Projectile *p : rmData)
       roomData.projectileList.push_back(p);

    }


// Check for collision for each projectile
void CheckProjectileCollision(RoomData& roomData, Player& player) {
    for (auto it = roomData.projectileList.begin(); it != roomData.projectileList.end();) {
        float tFirst = 0.0f;
        bool hit = false;

        bool enemyHit = false;
        //The projectile is shot by a friend
        if ((*it)->friendProjectile) {
            for (Enemy* guy : roomData.enemyList) {
                if (!guy->isActive) continue;
                if (static_cast<void*>(guy) == (*it)->GetSource()) continue;
                if (CollisionIntersectionRectRect(
                    (*it)->GetPosition(), (*it)->GetScale(), (*it)->GetVelocity(),
                    guy->sprite.position, guy->sprite.scale, guy->velocity, tFirst)
                    ) {
                    guy->currentHealth -= (*it)->GetDmg();
                    enemyHit = true;

                }
            }
        }
        else {

            //deliberate assignment
            hit = CollisionIntersectionRectRect(
                (*it)->GetPosition(),
                (*it)->GetScale(),
                (*it)->GetVelocity(),
                player.sprite.position,
                player.sprite.scale,
                Vector2{ 0, 0 },
                tFirst);
            if  (hit) {
                if ((*it)->GetDmg() < 0.f) {
                    // If dmg is neg then it heals 
                    PlayerHealsDamage(player);
                }
                else if ( (*it)->GetDmg() > 0.f)
                    PlayerTakesDamage(player);


            }
        }

        if (hit || enemyHit) {
            
            ProjectileParticleExplode(roomData, *(*it));
            delete* it;
            it = roomData.projectileList.erase(it);

        }
        else {
            ++it;
        }
    }
}

void RenderProjectile(RoomData& roomData) {
    for (Projectile* p : roomData.projectileList)
        if (p) p->RenderProjectile();
}

void ProjectileClear(RoomData& roomData) {
    for (Projectile* p : roomData.projectileList) delete p;
    roomData.projectileList.clear();
}

void ProjectileParticleExplode(RoomData& roomData, Projectile const& projectile)
{
    //generate death particles

    float projSpeed = projectile.GetVelocity().Length();
    projSpeed = projSpeed <= EPSILON ? 500.f : projSpeed;


    int numRotations = (rand() % 10) + 6;
    float particleSize = 2500.f;
    int massOfProj = static_cast<int>((projectile.GetScale().x * projectile.GetScale().x) / particleSize) / numRotations;
    massOfProj = massOfProj <= 0 ? 1 : massOfProj;

    for (int i = 0; i < numRotations; i++) {
        float degree = static_cast<float>(i) / static_cast<float>(numRotations) * 360.f;

        for (int pi = 0; pi < (massOfProj); pi++) {

            float randDegree = degree + (AERandFloat() * 30.f) - 15.f;

            float radian = randDegree * (PI / 180.f);
            Vector2 dir = Vector2(cosf(radian), -sinf(radian));
            roomData.particleSystem.CreateParticles(1, 0.5f, dir * projSpeed, projectile.GetPosition() + dir * projectile.GetScale().x * AERandFloat() * 0.25f,
               projectile.GetSprite().color);

        }
        //Add some randomness

    }
}
