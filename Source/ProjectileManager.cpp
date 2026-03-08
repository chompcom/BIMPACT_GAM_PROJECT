#include "ProjectileManager.h"


void ProjectileManager::InitFireball(AEGfxVertexList* mesh, AEGfxTexture* texture) {
    this->mesh = mesh;
    this->fireballTex = texture;
}

void ProjectileManager::InitAOE(AEGfxVertexList* mesh, AEGfxTexture* texture) {
    this->mesh = mesh;
    this->aoeTex = texture;
}
void ProjectileManager::ShootFireball(RoomData& roomData, Vector2 pos, Vector2 dir, float speed, float lifetime, int damage, Vector2 scale, Color color) {
    TexturedSprite* bulletSprite = new TexturedSprite(
        mesh,
        fireballTex,
        pos,
        scale,
        color
    );

    Vector2 velocity = dir.Normalized() * speed;
    Projectile* bullet = new Projectile(bulletSprite, BULLET, velocity, lifetime, damage);
    roomData.projectileList.push_back(bullet);
}

void ProjectileManager::ShootAOE(RoomData& roomData, Vector2 pos, Vector2 dir, float speed, float lifetime, int damage, Vector2 scale, Color color) {
    int numProjectiles = 10;
    float angleAround = 360.0f / numProjectiles;  

    for (int i = 0; i < numProjectiles; i++) {
        float angle = AEDegToRad(angleAround * i);

        Vector2 shotDir;      
        shotDir.x = AECos(angle);
        shotDir.y = AESin(angle);

        TexturedSprite* aoeSprite = new TexturedSprite(
            mesh,
            aoeTex,
            pos,
            scale,
            color
        );

        Vector2 velocity = shotDir * speed;  // uses shotDir
        Projectile* aoe = new Projectile(aoeSprite, AOE, velocity, lifetime, damage);
        roomData.projectileList.push_back(aoe);
    }
}

void ProjectileManager::Update(RoomData& roomData, float dt) {
    for (auto it = roomData.projectileList.begin(); it != roomData.projectileList.end();) {
        (*it)->ProjectileUpdate(dt);

        if (!(*it)->IsAlive()) {
            delete* it;
            it = roomData.projectileList.erase(it);
        }
        else {
            ++it;
        }
    }
}

void ProjectileManager::Render(RoomData& roomData) {
    for (Projectile* p : roomData.projectileList)
        if (p) p->ProjectileRender();
}

void ProjectileManager::Clear(RoomData& roomData) {
    for (Projectile* p : roomData.projectileList) delete p;
    roomData.projectileList.clear();
}