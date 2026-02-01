#include "TestScene.h"

#include "AEEngine.h"
#include "Sprite.h"

#include "Player.h"
#include "Gift.h"
#include "Utils/Utils.h"
#include "Enemy.h"
AEGfxVertexList* sqmesh = nullptr;

TexturedSprite* thing = nullptr;

AEGfxTexture* rockpng = nullptr;
AEGfxTexture* playerpng = nullptr;

Player player{ TexturedSprite(sqmesh,playerpng,Vector2(),Vector2(),Color{1,1,1,1}), 25000.f, 600.f, Vector2(0,0) };


Gift gift{ "boat", {"happy","sad"}, Sprite()};

void TestLoad()
{
	sqmesh = CreateSquareMesh();
	rockpng = AEGfxTextureLoad("Assets/poprocks.png");
	playerpng = AEGfxTextureLoad("Assets/player.png");
	thing = new TexturedSprite(sqmesh, rockpng, Vector2(0, 10), Vector2(100, 100), Color{ 1.0,1.0,1.0,0.0 });

	player.sprite = TexturedSprite(sqmesh, playerpng, Vector2(300, 300), Vector2(100, 100), Color{ 1,1,1,0 }
);
	gift.sprite = Sprite(sqmesh, Vector2(0, 0), Vector2(100, 100), Color{ 1,0,0,0 });
}

void TestInit()
{
}

void TestDraw()
{
    AEGfxSetBackgroundColor(1.0f, 1.0f, 1.0f);
    AEGfxSetRenderMode(AE_GFX_RM_TEXTURE);
    AEGfxSetBlendMode(AE_GFX_BM_BLEND);
    AEGfxSetTransparency(1.0f);
	thing->RenderSprite();
	player.sprite.RenderSprite();
	AEGfxSetRenderMode(AE_GFX_RM_COLOR);
	gift.sprite.RenderSprite();
}

void TestFree()
{
}

void TestUnload()
{
	AEGfxMeshFree(sqmesh);
	if (thing) {
		delete thing;
		thing = nullptr;
	}
	AEGfxTextureUnload(rockpng);
	AEGfxTextureUnload(playerpng);
}

void TestUpdate(float dt)
{
	UpdatePlayer(player, dt);
	player.sprite.UpdateTransform();
	thing->position += Vector2(10,10) * dt;
	thing->UpdateTransform();
	UpdateGift(gift,player,dt);
	gift.sprite.UpdateTransform();


	if (AreSquaresIntersecting(gift.sprite.position, gift.sprite.scale.x, thing->position, thing->scale.x) ){
		gift.velocity = -gift.velocity;
	}
}
