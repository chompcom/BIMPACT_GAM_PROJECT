#include "ProjectileManager.h"


void ShootProjectile(TexturedSprite sprite, RoomData& roomData, Vector2 pos, Vector2 dir, float speed, float lifetime, int damage, Vector2 scale, Color color, void* source) {
	sprite.position = pos;
	sprite.scale = scale;
	sprite.color = color;
	//sprite.direction = dir;
	Projectile* fireball = new Projectile(sprite, FIREBALL, dir * speed, lifetime, damage,0.0f, source);
    roomData.projectileList.push_back(fireball);
    ProjectileAudio();
}

void ShootAOE(TexturedSprite sprite, RoomData& roomData, Vector2 pos, float speed, float lifetime, int damage, Vector2 scale, Color color, void* source) {
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
        Projectile* aoe = new Projectile(sprite, AOE, velocity, lifetime, damage,0.0f, source);
        roomData.projectileList.push_back(aoe);
    }
}

void ShootRounding(TexturedSprite sprite, RoomData& roomData, Vector2 pos, Vector2 dir, float speed, float lifetime, int damage, Vector2 scale, Color color,float rot, void* source) {
    sprite.position = pos;
    sprite.scale = scale;
    sprite.color = color;
    
  //  Vector2 perpendicularVel = { dir.y, -dir.x };

  //  Vector2 swirlVel = (dir * speed) + (perpendicularVel * speed * 50.0f);

    Projectile* rounding = new Projectile(sprite, ROUNDING, dir * speed, lifetime, damage,rot,source);
    roomData.projectileList.push_back(rounding);
    RoundingProjectileAudio();
}

void ShootScatter(TexturedSprite sprite, RoomData& roomData, Vector2 pos, Vector2 dir, float speed, float lifetime, int damage, Vector2 scale, Color color,void* source) {
    sprite.position = pos;
    sprite.scale = scale;
    sprite.color = color;

    //sprite.direction = dir;
    Projectile* scatter = new Projectile(sprite, SCATTER, dir * speed, lifetime, damage,0.0f, source);
    scatter->isScatter = true;
    roomData.projectileList.push_back(scatter);  
    
}

void ShootBoomerang(TexturedSprite sprite, RoomData& roomData, Vector2 pos, Vector2 dir, float speed, float lifetime, int damage, Vector2 scale, Color color, void* source) {
    sprite.position = pos;
    sprite.scale = scale;
    sprite.color = color;
    Projectile* boomerang = new Projectile(sprite, BOOMERANG, dir * speed, lifetime, damage, 30.0f, source);
    roomData.projectileList.push_back(boomerang);

}

void UpdateProjectiles(RoomData& roomData, float dt) {
    std::vector<Projectile*> rmdata;

    for (auto it = roomData.projectileList.begin(); it != roomData.projectileList.end();) {
        (*it)->UpdateProjectile(dt);
        if (!(*it)->IsAlive()) {
            if ((*it)->isScatter && !(*it)->didScatter) {
                int numProjectiles = 10;
                float angleAround = 360.0f / numProjectiles;
                for (int i = 0; i < numProjectiles; i++) {
                    float angle = AEDegToRad(angleAround * i);
                    Vector2 shotDir = { AECos(angle), AESin(angle) };
                    TexturedSprite spr = (*it)->GetSprite(); 
                    Projectile* aoe = new Projectile(spr, AOE, shotDir * 200.0f, 1.0f, (*it)->GetDmg());
                    rmdata.push_back(aoe);
                }
            }
                delete* it;
                it = roomData.projectileList.erase(it);
            }
            else {
                ++it;
            }
        }
    for(Projectile *p : rmdata)
       roomData.projectileList.push_back(p);

    }


void CheckProjectileCollision(RoomData& roomData, Player& player) {
    for (auto it = roomData.projectileList.begin(); it != roomData.projectileList.end();) {
        float tFirst = 0.0f;
        bool hit = CollisionIntersection_RectRect(
            (*it)->GetPosition(),
            (*it)->GetScale(),
            (*it)->GetVelocity(),
            player.sprite.position,
            player.sprite.scale,
            Vector2{ 0, 0 },
            tFirst
        );

        bool enemyHit = false;
        for (Enemy* guy : roomData.enemyList) {

            if (static_cast<void*>(guy) == (*it)->GetSource()) continue;
            if ( CollisionIntersection_RectRect(
                    (*it)->GetPosition(), (*it)->GetScale(), (*it)->GetVelocity(),
                    guy->sprite.position, guy->sprite.scale, guy->velocity, tFirst) 
            ) {
                guy->currentHealth -= (*it)->GetDmg();
                std::cout << (*it)->GetSource() << " HIT " << guy << "!!\n";
                enemyHit = true;
            }
        }
        
        if (hit) {
            playerTakesDamage(player);
        }

        if (hit || enemyHit) {
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