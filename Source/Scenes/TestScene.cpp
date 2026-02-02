#include "TestScene.h"

#include "AEEngine.h"
#include "Sprite.h"

#include "Player.h"
#include "Gift.h"
#include "Utils/Utils.h"
#include "Enemy.h"
#include "rooms.h"
AEGfxVertexList* sqmesh = nullptr;

TexturedSprite* thing = nullptr;

AEGfxTexture* rockpng = nullptr;
AEGfxTexture* playerpng = nullptr;

Player player{ TexturedSprite(sqmesh,playerpng,Vector2(),Vector2(),Color{1,1,1,1}), 25000.f, 600.f, Vector2(0,0) };


Gift gift{ "boat", {"happy"}, Sprite()};
Gift gift2{"bad", {"sad"}, Sprite()};

EnemyType rocktype{"rock",100,10,{"sad"},{"happy"},{"sad"}};
Enemy rock{rocktype, TexturedSprite(sqmesh,rockpng,Vector2(),Vector2(),Color{1,1,1,1})};
mapRooms::Map gameMap;          // Init var for map
/* PSUDEOCODE SONE

RoomData tobetranfered{};
vector<enemy*> enemyList
vector<gift*> gift;
*/

void TestLoad()
{
	sqmesh = CreateSquareMesh();
	rockpng = AEGfxTextureLoad("Assets/poprocks.png");
	playerpng = AEGfxTextureLoad("Assets/player.png");
	thing = new TexturedSprite(sqmesh, rockpng, Vector2(0, 10), Vector2(100, 100), Color{ 1.0,1.0,1.0,0.0 });

	player.sprite = TexturedSprite(sqmesh, playerpng, Vector2(300, 300), Vector2(100, 100), Color{ 1,1,1,0 }
);
	gift.sprite = Sprite(sqmesh, Vector2(200, 500), Vector2(100, 100), Color{1.f,0.f,0.f,1.f});
	gift2.sprite = Sprite(sqmesh, Vector2(-200, 500), Vector2(100, 100), Color{1.f,1.f,0.f,1.f});

	rock.sprite = *thing;


	rocktype.neutral = WalkLeft;
	rocktype.happy = WalkRight;
	rocktype.angry = WalkToTarget; 
	rock.ChangeState(EnemyStates::ES_NEUTRAL);
	gameMap.InitMap(0xA341312Cu);   // Seeded Run
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

	AEGfxSetRenderMode(AE_GFX_RM_COLOR);
	gameMap.RenderCurrentRoom(sqmesh);


	player.sprite.RenderSprite();
	//rock.sprite.RenderSprite();
	gameMap.RenderDebugMap(sqmesh); // Debug Map

	//gift.sprite.RenderSprite();
	//gift2.sprite.RenderSprite();
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
	/*
	thing->position += Vector2(10,10) * dt;
	thing->UpdateTransform();
	UpdateGift(gift,player,dt);
	gift.sprite.UpdateTransform();
	UpdateGift(gift2,player,dt);
	gift2.sprite.UpdateTransform();
	rock.Update(dt);
	rock.target = player.sprite.position;

	std::vector<Gift*> things{ &gift,&gift2 };

	for (Gift* gift : things) {
		if (!(gift->velocity == Vector2())) {
			if (AreSquaresIntersecting(gift->sprite.position, gift->sprite.scale.x, rock.sprite.position, rock.sprite.scale.x)) {
				gift->velocity = -gift->velocity;
				rock.AssessTraits(gift->traits);
			}
		}
	}
	*/
	UpdatePlayer(player, dt);
	player.sprite.UpdateTransform();

	Vector2 playerHalfSize = player.sprite.scale * 0.5f;

	gameMap.GetCurrentRoom()->Update(dt);
	gameMap.UpdateMap(player.position, playerHalfSize,dt);

	/* PSUEDOCODE ZONE

	Room {
		roomdata* toBeTransfered;
		roomdata currentRoomData;
		
	}	
	
	gameMap.currentRoom.

	//takes player position and checks if its at a door?
	gameMap.UpdateMap(player.position, playerHalfSize,dt);
	//if its at a door, roomdata gets transferred




	*/
}
