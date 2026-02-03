#include "TestScene.h"

#include "AEEngine.h"
#include "Sprite.h"

#include "Player.h"
#include "Gift.h"
#include "Utils/Utils.h"
#include "Enemy.h"
#include "rooms.h"
#include "Loaders/DataLoader.h"
#include "Collision.h"
#include "RoomData.h"

AEGfxVertexList* sqmesh = nullptr;

TexturedSprite* thing = nullptr;

AEGfxTexture* rockpng = nullptr;
AEGfxTexture* playerpng = nullptr;

Player player{ TexturedSprite(sqmesh,playerpng,Vector2(),Vector2(),Color{1,1,1,1}), 25000.f, 600.f, Vector2(0,0) };


//Gift gift{ "boat", {"happy"}, Sprite()};
//Gift gift2{"bad", {"sad"}, Sprite()};
//Gift gift{ "boat", {"happy"}, TexturedSprite(sqmesh,nullptr, Vector2(), Vector2(), Color{1,1,1,1}) };
//Gift gift2{"bad", {"sad"}, TexturedSprite(sqmesh, nullptr, Vector2(), Vector2())};

EnemyType rocktype{"rock",100,10,{"sad"},{"happy"},{"sad"}};
Enemy rock{rocktype, TexturedSprite(sqmesh,rockpng,Vector2(),Vector2(),Color{1,1,1,1})};
mapRooms::Map gameMap;          // Init var for map
static RoomData globalTransferData{};
/* PSUDEOCODE SONE

RoomData tobetranfered{};
vector<enemy*> enemyList
vector<gift*> gift;
*/

void TestLoad()
{
	DataLoader::Load();
	sqmesh = CreateSquareMesh();
	rockpng = AEGfxTextureLoad("Assets/poprocks.png");
	playerpng = AEGfxTextureLoad("Assets/player.png");
	thing = new TexturedSprite(sqmesh, rockpng, Vector2(0, 10), Vector2(100, 100), Color{ 1.0,1.0,1.0,0.0 });

	player.sprite = TexturedSprite(sqmesh, playerpng, Vector2(300, 300), Vector2(100, 100), Color{ 1,1,1,0 }
);
	//gift.sprite = DataLoader::CreateTexture("Assets/veggiefish.png");
	//gift2.sprite = DataLoader::CreateTexture("Assets/pattyfish.png");

	rock.sprite = *thing;


	rocktype.neutral = WalkLeft;
	rocktype.happy = WalkRight;
	rocktype.angry = WalkToTarget; 
	rock.ChangeState(EnemyStates::ES_NEUTRAL);

	// Global Data Here
	
	globalTransferData.enemyList.clear();
	globalTransferData.giftList.clear();
	globalTransferData.player = &player;

	gameMap.InitMap(globalTransferData, 0xA341311Cu);   // Seeded Run
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

	gameMap.RenderCurrentRoom(DataLoader::GetMesh());
	// Draw objects: current-room objects + carried objects
	if (mapRooms::Room* room = gameMap.GetCurrentRoom())
	{
		RoomData& roomData = room->currentRoomData;
		RoomData& carryData = gameMap.GetTransferData();

		for (Gift* g : roomData.giftList)   if (g) g->sprite.RenderSprite();
		for (Enemy* e : roomData.enemyList) if (e) e->sprite.RenderSprite();

		for (Gift* g : carryData.giftList)   if (g) g->sprite.RenderSprite();
		for (Enemy* e : carryData.enemyList) if (e) e->sprite.RenderSprite();
	}

	player.sprite.RenderSprite();
	//rock.sprite.RenderSprite();
	//gift.sprite.RenderSprite();
	//gift2.sprite.RenderSprite();
	gameMap.RenderDebugMap(sqmesh); // Debug Map

	//rock.sprite.RenderSprite();

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

	
	
	// Dellocate enemy and gift assets
	for (Enemy *e: globalTransferData.enemyList) {
		delete e;
	}
	globalTransferData.enemyList.clear();

	for (Gift* g : globalTransferData.giftList) {
		delete g;
	}
	globalTransferData.giftList.clear();
	
	globalTransferData.player = nullptr;

	DataLoader::Unload();
}

void TestUpdate(float dt)
{

	// Player update
	UpdatePlayer(player, dt);
	player.sprite.UpdateTransform();

	//std::cout << player.position.x << player.position.y;

	// Game map update
	gameMap.GetCurrentRoom()->Update(dt);		


	mapRooms::Room* currentRoom = gameMap.GetCurrentRoom();
	// Do stuff here
	RoomData& roomData = currentRoom->currentRoomData;
	RoomData& carryData = gameMap.GetTransferData();

	// Update Enemies (carryData version is only for "Friends")
	for (Enemy* e : roomData.enemyList) {
		if (e) {
			e->target = player.sprite.position;
			e->Update(dt);
		}
	}

	// Update Gifts (Must update both sides)
	for (Gift* g : roomData.giftList) {
		if (g) {
			UpdateGift(*g, player, dt);
			g->sprite.UpdateTransform();
		}
	}
	for (Gift* g : carryData.giftList) {
		if (g) {
			UpdateGift(*g, player, dt);
			g->sprite.UpdateTransform();
		}
	}


	// Gifts and Enemy Check
	for (Gift* gift : currentRoom->currentRoomData.giftList) {
		if (!(gift->velocity == Vector2())) {
			for (Enemy* e : currentRoom->currentRoomData.enemyList) {
				if (AreSquaresIntersecting(gift->sprite.position, gift->sprite.scale.x, e->sprite.position, e->sprite.scale.x)) {
					gift->velocity = -gift->velocity;
					e->AssessTraits(gift->traits);
				}
			}
		}
	}


	// 4) Transfer enemies to our carrylist if they are essentially happy
	for (size_t i = 0; i < roomData.enemyList.size(); )
	{
		Enemy* e = roomData.enemyList[i];
		if (e && e->state == EnemyStates::ES_HAPPY)	// This is the happy check ig?
		{
			carryData.enemyList.push_back(e);
			roomData.enemyList.erase(roomData.enemyList.begin() + static_cast<long>(i));
			continue;
		}
		++i;
	}

	// Idk how to check which gifts to pick up
	for (size_t i = 0; i < roomData.giftList.size(); )
	{
		Gift* g = roomData.giftList[i];
		if (g && g->pickUpState)	// If currently picked up, need to check pickupstate = false and remove such????
		{
			carryData.giftList.push_back(g);	// Transfer to carryData list (active list essentially)
			roomData.giftList.erase(roomData.giftList.begin() + static_cast<long>(i));	// Remove from current roomData
			break;
		}
		++i;
	}

	// Check if gifts are indeed pickedup
	// Idk how to check which gifts to pick up
	for (size_t i = 0; i < carryData.giftList.size(); )
	{
		Gift* g = carryData.giftList[i];
		if (g && g->pickUpState==false)	// If currently picked up, need to check pickupstate = false and remove such????
		{
			roomData.giftList.push_back(g);
			carryData.giftList.erase(carryData.giftList.begin() + static_cast<long>(i));	// Remove from current carryData
			continue;
		}
		++i;
	}


	// Update game map
	Vector2 playerHalfSize = player.sprite.scale * 0.5f;
	gameMap.UpdateMap(player.position, playerHalfSize,dt);
	
	// Legacy: TO BE COPIED INTO ROOM COLLISION DETECTION CLASS (BUT THERE'S NOTHING YET EVEN???) 
	//for (Gift* gift : roomData.giftList) {
	//	if (!(gift->velocity == Vector2())) {
	//		if (AreSquaresIntersecting(gift->sprite.position, gift->sprite.scale.x, rock.sprite.position, rock.sprite.scale.x)) {
	//			gift->velocity = -gift->velocity;
	//			rock.AssessTraits(gift->traits);
	//		}
	//	}
	//}


	//std::vector<Gift*> things{ &gift,&gift2 };

	
	//thing->position += Vector2(10,10) * dt;
	//thing->UpdateTransform();
	
	//rock.Update(dt);
	//rock.target = player.sprite.position;
	
	//UpdatePlayer(player, dt);
	//player.sprite.UpdateTransform();
	//UpdateGift(gift, player, dt);
	//gift.sprite.UpdateTransform();
	//UpdateGift(gift2, player, dt);
	//gift2.sprite.UpdateTransform();



	//thing->position += Vector2(10,10) * dt;
	//thing->UpdateTransform();
	/*
	UpdateGift(gift,player,dt);
	gift.sprite.UpdateTransform();
	UpdateGift(gift2,player,dt);
	gift2.sprite.UpdateTransform();
	rock.Update(dt);
	rock.target = player.sprite.position;

	gameMap.GetCurrentRoom()->Update(dt);

	std::vector<Gift*> things{ &gift,&gift2 };

	for (Gift* gift : things) {
		if (gift->velocity != Vector2(0,0)) {
			for (Enemy* enemy : gameMap.GetCurrentRoom()->currentRoomData.enemyList) {
				if (CollisionIntersection_RectRect_Static(AABB{ gift->sprite.position - gift->sprite.scale / 2, gift->sprite.position + gift->sprite.scale / 2 },
					AABB{ enemy->sprite.position - enemy->sprite.scale / 2, enemy->sprite.position + enemy->sprite.scale / 2})) {
					gift->velocity = Vector2(0, 0);
					enemy->AssessTraits(gift->traits);
				}

			}
				
		}
	}
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
