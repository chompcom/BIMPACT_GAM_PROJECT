#pragma once
#include "Projectile.h"
#include "RoomData.h"
#include "Sprite.h"

class ProjectileManager {
public:
    ProjectileManager() = default;

    void InitFireball(AEGfxVertexList* mesh, AEGfxTexture* texture);
    void InitAOE(AEGfxVertexList* mesh, AEGfxTexture* texture);
    void ShootFireball(RoomData& roomData, Vector2 pos, Vector2 dir, float speed, float lifetime, int damage, Vector2 scale, Color color);
    void ShootAOE(RoomData& roomData, Vector2 pos, Vector2 dir, float speed, float lifetime, int damage, Vector2 scale, Color color);
    void Update(RoomData& roomData, float dt);
    void Render(RoomData& roomData);
    void Clear(RoomData& roomData);

private:
    AEGfxVertexList* mesh = nullptr;
    AEGfxTexture* fireballTex = nullptr;
    AEGfxTexture* aoeTex = nullptr;

    float speed = 0.0f;
    float lifetime = 0.0f;
    int   damage = 0;
    Vector2 scale = { 1.0f, 1.0f };
};