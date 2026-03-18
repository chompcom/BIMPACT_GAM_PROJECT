#include "TestScene.h"

#include "AEEngine.h"
#include "../Sprite.h"

#include "../Player.h"
#include "../Gift.h"
#include "../Utils/Utils.h"
#include "../Enemy.h"
#include "../rooms.h"
#include "../Loaders/DataLoader.h"
#include "../Collision.h"
#include "../RoomData.h"
#include "Projectile.h"
#include "ProjectileManager.h"
#include "../HUD.h"
#include "../almanac.h"
#include "ParticleSystem.h"

AEGfxVertexList* sqmesh = nullptr;

TexturedSprite* thing = nullptr;
TexturedSprite* bulletSprite = nullptr;


AEGfxTexture* rockpng = nullptr;
AEGfxTexture* playerpng = nullptr;
AEGfxTexture* shadowpng = nullptr;
AEGfxTexture* bulletpng = nullptr;
AEGfxTexture* aoepng = nullptr;
AEGfxTexture* heartpng = nullptr;
AEGfxTexture* almanacpng = nullptr;
AEGfxTexture* pDoorTex = nullptr;	// Door image

//AEGfxTexture* almanacpagepng = nullptr;

std::vector<TexturedSprite> healthIcons;
TexturedSprite * almanacIcon = nullptr;

//TexturedSprite* almanacPage = nullptr;
Almanac almanac {};
//std::vector<TexturedSprite> almanacPageSprites;

s8 font = 0;

Player player{ TexturedSprite(sqmesh,playerpng,Vector2(),Vector2(),Color{1,1,1,1}), TexturedSprite(sqmesh,shadowpng,Vector2(),Vector2(),Color{1,1,1,1}), 25000.f, 600.f, Vector2(0,0) };
static ProjectileManager projManager;



//Gift gift{ "boat", {"happy"}, Sprite()};
//Gift gift2{"bad", {"sad"}, Sprite()};
//Gift gift{ "boat", {"happy"}, TexturedSprite(sqmesh,nullptr, Vector2(), Vector2(), Color{1,1,1,1}) };
//Gift gift2{"bad", {"sad"}, TexturedSprite(sqmesh, nullptr, Vector2(), Vector2())};

EnemyType rocktype{"rock",100,10,{"sad"},{"happy"},{"sad"}};
Enemy rock{rocktype, TexturedSprite(sqmesh,rockpng,Vector2(),Vector2(),Color{1,1,1,1}),TexturedSprite(sqmesh,rockpng,Vector2(),Vector2(),Color{1,1,1,1}) };
mapRooms::Map gameMap;          // Init var for map
static RoomData globalTransferData{};
/* PSUDEOCODE SONE

RoomData tobetranfered{};
vector<enemy*> enemyList
vector<gift*> gift;
*/

//almanac vector
//std::vector<AlmanacEntry> almanacVector;
//vector of all enemytypes for the almanac
//std::vector<std::string> enemyTypeNames {};

ParticleSystem testParticles = NULL;

void TestLoad()
{
	DataLoader::Load();
	sqmesh = CreateSquareMesh();
	rockpng = AEGfxTextureLoad("Assets/poprocks.png");
	playerpng = AEGfxTextureLoad("Assets/player.png");
	shadowpng = AEGfxTextureLoad("Assets/shadow.png");
	bulletpng = AEGfxTextureLoad("Assets/fireball.png");
	heartpng = AEGfxTextureLoad("Assets/heart.png");
	//heartpng = AEGfxTextureLoad("Assets/heart.png");
	almanacpng = AEGfxTextureLoad("Assets/almanac.png");

	font = AEGfxCreateFont("Assets/liberation-mono.ttf", 32);

	//pDoorTex = AEGfxTextureLoad("Assets/door.png");
	//pDoorTex = DataLoader::CreateTexture("Assets/door.png");


	//healthIcons[0] = TexturedSprite(sqmesh, heartpng, Vector2{ -600.5f,-350.f }, Vector2{ 64.f,64.f }, Color{ 1.f,1.f,1.f,1.f });
	//healthIcons[1] = TexturedSprite(sqmesh, heartpng, Vector2{ -500.5f,-350.f }, Vector2{ 64.f,64.f }, Color{ 1.f,1.f,1.f,1.f });
	//healthIcons[2] = TexturedSprite(sqmesh, heartpng, Vector2{ -400.5f,-350.f }, Vector2{ 64.f,64.f }, Color{ 1.f,1.f,1.f,1.f });

	//healthIcons[0] = DataLoader::CreateTexture("Assets/heart.png");
	//healthIcons[1] = DataLoader::CreateTexture("Assets/heart.png");
	//healthIcons[2] = DataLoader::CreateTexture("Assets/heart.png");

	//healthIcons[0].position = Vector2{ -600.5f,-350.f };
	//healthIcons[1].position = Vector2{ -500.5f,-350.f };
	//healthIcons[2].position = Vector2{ -400.5f,-350.f };

	//healthIcons[0].scale = Vector2{ 64.f,64.f };
	//healthIcons[1].scale = Vector2{ 64.f,64.f };
	//healthIcons[2].scale = Vector2{ 64.f,64.f };
	font = AEGfxCreateFont("Assets/Kenney Pixel.ttf", 64);

	healthIcons.push_back(DataLoader::CreateTexture("Assets/heart.png"));
	healthIcons.push_back(DataLoader::CreateTexture("Assets/heart.png"));
	healthIcons.push_back(DataLoader::CreateTexture("Assets/heart.png"));

	healthIcons[0].position = Vector2{ -600.5f,-350.f };
	healthIcons[1].position = Vector2{ -500.5f,-350.f };
	healthIcons[2].position = Vector2{ -400.5f,-350.f };

	healthIcons[0].scale = Vector2{ 64.f,64.f };
	healthIcons[1].scale = Vector2{ 64.f,64.f };
	healthIcons[2].scale = Vector2{ 64.f,64.f };

	//enemyTypes.push_back(rocktype);

	LoadAlmanacPages(almanac);
	almanac.pageSprites[0].scale = Vector2(1600.f, 900.f);
	LoadAlmanacEntries(almanac/*, enemyTypeNames*/);

	//almanacIcon = DataLoader::CreateTexture("Assets/almanac.png");

	//almanacIcon.position = Vector2{ 600.5f,-350.f };

	almanacIcon = new TexturedSprite(sqmesh, almanacpng, Vector2(640.f, -325.f), Vector2(128, 128), Color{ 1.0,1.0,1.0,0.0 });

	thing = new TexturedSprite(sqmesh, rockpng, Vector2(0, 10), Vector2(100, 100), Color{ 1.0,1.0,1.0,0.0 });

	player.sprite = TexturedSprite(sqmesh, playerpng, Vector2(300, 300), Vector2(100, 100), Color{ 1,1,1,0 });
	player.shadow = TexturedSprite(sqmesh, shadowpng, Vector2(300, 255), Vector2(100, 100), Color{ 1,1,1,0 });

	//gift.sprite = DataLoader::CreateTexture("Assets/veggiefish.png");

	//gift2.sprite = DataLoader::CreateTexture("Assets/pattyfish.png");


	rock.sprite = *thing;


	//rocktype.neutral = WalkLeft;
	//rocktype.happy = WalkRight;
	//rocktype.angry = WalkToTarget; 
	rock.ChangeState(EnemyStates::ES_NEUTRAL);

	// Global Data Here
	
	globalTransferData.enemyList.clear();
	globalTransferData.giftList.clear();
	globalTransferData.projectileList.clear();

	globalTransferData.player = &player;

	//square seed: 0xA341311Cu
	gameMap.InitMap(globalTransferData, 0xA341311Cu);   // Seeded Run
	projManager.InitFireball(sqmesh, bulletpng);
	projManager.InitAOE(sqmesh, aoepng);

	// Enable to allow for random values each run
	std::srand(static_cast<unsigned int>(std::time(nullptr))); // So based on number of seconds passed since Jan 1, 1970, this becomes our srand seed
	unsigned int curSeed = gameMap.RandInt(0, RAND_MAX - 1);
	gameMap.InitMap(globalTransferData, curSeed);
	std::cout << "Current Seed: " << curSeed << "\n";
	// Interesting ones: 32461, 32608, 31931, 18283, 31060
	// Too easy: 32702, 0xA341311Cu, 

	//gameMap.InitMap(globalTransferData, 0xA341311Cu);   // Seeded Run

	
	testParticles = ParticleSystem(sqmesh);


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
	
	
	//gameMap.RenderCurrentRoom(sqmesh);

	gameMap.RenderCurrentRoom(DataLoader::GetMesh());

	AEGfxSetRenderMode(AE_GFX_RM_COLOR);
	for (Particle particle : testParticles.particles) if (particle.isActive) particle.sprite.RenderSprite();
	AEGfxSetRenderMode(AE_GFX_RM_TEXTURE);

	// Draw objects: current-room objects + carried objects
	if (mapRooms::Room* room = gameMap.GetCurrentRoom())
	{
		RoomData& roomData = room->currentRoomData;
		RoomData& carryData = gameMap.GetTransferData();

		gameMap.GetCurrentRoom()->roomGrid.RenderGrid(DataLoader::GetMesh(), carryData.player->position, carryData.player->sprite.scale, AE_GFX_RM_TEXTURE);

		for (Gift* g : roomData.giftList) {
			if (g) {
				if (!g->pickUpState) g->shadow.RenderSprite();
				g->sprite.RenderSprite();
			}
		}
		for (Enemy* e : roomData.enemyList) {
			if (e) {
				e->shadow.RenderSprite();
				e->sprite.RenderSprite();
			}
		}
		if (roomData.boss) {
			roomData.boss->shadow.RenderSprite();
			roomData.boss->sprite.RenderSprite();

		}

		for (Gift* g : carryData.giftList) {
			if (g) {
				if (!g->pickUpState) g->shadow.RenderSprite();
				g->sprite.RenderSprite();
			}
		}
		for (Enemy* e : carryData.enemyList) {
			if (e) {
				e->shadow.RenderSprite();
				e->sprite.RenderSprite();
			}
		}
		
		for (Projectile* p : roomData.projectileList) if (p) p->ProjectileRender();
		for (Projectile* p : carryData.projectileList) if (p) p->ProjectileRender();
	}
	

	player.shadow.RenderSprite();
	player.sprite.RenderSprite();

	AEGfxSetRenderMode(AE_GFX_RM_COLOR);

	AEGfxSetRenderMode(AE_GFX_RM_TEXTURE);

	//rock.sprite.RenderSprite();
	//gift.sprite.RenderSprite();
	//gift2.sprite.RenderSprite();
	gameMap.RenderDebugMap(sqmesh); // Debug Map


	
	renderPlayerLives(player, healthIcons, font);
	(*almanacIcon).RenderSprite();
	RenderAlmanacPages(almanac, font);

	AlmanacInputs(almanac/*, sqmesh*/);

#if 0
	AEGfxSetRenderMode(AE_GFX_RM_COLOR);
	TexturedSprite thing = DataLoader::CreateTexture("Assets/poprocks.png");
	std::pair<int, int> yup{ 5,11 };
	float width = 110;
	thing.scale = Vector2{ width,width };
	//Vector2 offsetpos{ -(yup.second * width) / 2 , (yup.first * width) / 2 };
	Vector2 offsetpos{ -(yup.second * width) / 2, -(yup.first * width) / 2 + 50};
	for (int row = 0; row < yup.first; row++) {

		for (int col = 0; col < yup.second; col++) {
			thing.position = Vector2{ offsetpos.x + (1.1f*col * width), offsetpos.y + (1.1f*row * width) };
			thing.UpdateTransform();
			thing.RenderSprite();
		}

	}
	AEGfxSetRenderMode(AE_GFX_RM_TEXTURE);
	player.sprite.RenderSprite(true);
#endif

	

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
	delete almanacIcon;

	AEGfxTextureUnload(rockpng);
	AEGfxTextureUnload(playerpng);
	AEGfxTextureUnload(shadowpng);
	AEGfxTextureUnload(bulletpng);
	
	AEGfxTextureUnload(heartpng);
	//AEGfxTextureUnload(heartpng);
	AEGfxTextureUnload(almanacpng);

	AEGfxDestroyFont(font);
	
	// Dellocate enemy and gift assets
	for (Enemy *e: globalTransferData.enemyList) {
		delete e;
	}

	for (Gift* g : globalTransferData.giftList) {
		delete g;
	}

	for (Projectile* p : globalTransferData.projectileList) delete p;
	
	globalTransferData.player = nullptr;
	gameMap.DeleteMap();
	DataLoader::Unload();
	if (gameMap.GetCurrentRoom())
		projManager.Clear(gameMap.GetCurrentRoom()->currentRoomData);

}

void TestUpdate(float dt)
{

	// Get previous pos
	Vector2 prevPos{ player.position.x, player.position.y };
	
	UpdatePlayer(player, dt); // Player update
	Vector2 playerHalfSize = player.sprite.scale * 0.5f;

	// Print Current Grid
	std::cout << "Grid Current: " << gameMap.GetCurrentRoom()->roomGrid.WorldToCell(player.position.x, player.position.y) << "\n";
	for (int i = 0; i < 20; ++i) {
		for (int j = 0; j < 20; ++j) std::cout << gameMap.GetCurrentRoom()->roomGrid.GetCell(j, i) << " ";
		std::cout << '\n';
	}

	// Game map update
	gameMap.GetCurrentRoom()->Update(dt);

	mapRooms::Room* currentRoom = gameMap.GetCurrentRoom();
	RoomData& roomData = currentRoom->currentRoomData;
	RoomData& carryData = gameMap.GetTransferData();

	// Update game map
	gameMap.UpdateMap(player.position, playerHalfSize, dt);

	//Vector2 playerHalfSize = player.sprite.scale * 0.5f;
	Vector2 positionResetTest = player.position;

	// Test Player Collision with Map
	int curCell = gameMap.GetCurrentRoom()->roomGrid.WorldToCell(player.position.x, player.position.y);
	if (curCell >= 0 && curCell != 0xffffff) currentRoom->lastValidCell = curCell;
	int colRes = gameMap.GetCurrentRoom()->roomGrid.CheckMapGridCollision(player.position.x, player.position.y, player.sprite.scale.x, player.sprite.scale.y, curCell);
	if (colRes & COLLISION_LEFT || colRes & COLLISION_RIGHT) player.position.x = prevPos.x;		// Test for x collision
	if (colRes & COLLISION_TOP || colRes & COLLISION_BOTTOM) player.position.y = prevPos.y;		// Test for y collision

	// Finally reflect changes?
	player.sprite.UpdateTransform();
	player.shadow.UpdateTransform();
	

	//to test damage
	if (AEInputCheckTriggered(AEVK_P)) playerTakesDamage(player);
	if (AEInputCheckTriggered(AEVK_O)) playerHealsDamage(player);

	checkIfAlmanacClicked(*almanacIcon, almanac);

	

	//std::cout << player.position.x << player.position.y;



	// Update Enemies (carryData version is only for "Friends")
	for (Enemy* e : roomData.enemyList) {
		if (e) {
			//e->target.position = *player.sprite.position;
			e->Update(dt);
		}
	}

	for (Enemy* e : carryData.enemyList) {
		if (e) {
			//e->target.position = *player.sprite.position;
			e->Update(dt);
		}
	}

	// Update Gifts (Must update both sides)
	for (Gift* g : roomData.giftList) {
		if (g) {
			UpdateGift(*g, player, dt);
			g->sprite.UpdateTransform();
			g->shadow.UpdateTransform();
		}
	}
	for (Gift* g : carryData.giftList) {
		if (g) {
			UpdateGift(*g, player, dt);
			g->sprite.UpdateTransform();
			g->shadow.UpdateTransform();
		}
	}

	if (roomData.boss) {
		roomData.boss->Update(player, dt);
		roomData.boss->sprite.UpdateTransform();
		roomData.boss->shadow.UpdateTransform();
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
	Vector2 positionResetTest = player.position;
	gameMap.UpdateMap(player.position, playerHalfSize,testParticles,dt);



	// Room grid works! (To be removed)
	//std::cout << "Cell idx: " << currentRoom->roomGrid.WorldToCell(player.position.x, player.position.y) << std::endl;
	//std::cout << "x: " << player.position.x << "y: " << player.position.y;

	if (player.position != positionResetTest) {
		for (Enemy* e : carryData.enemyList) {
			e->sprite.position = player.position;
			e->roomData = &gameMap.GetCurrentRoom()->currentRoomData;
		}
		for (Projectile* p : roomData.projectileList) {
			p->RemoveProjectile();
		}
	}

	if (AEInputCheckTriggered(AEVK_SPACE))
		projManager.ShootFireball(roomData, player.position, player.direction,
			500.f, 2.f, 10, Vector2(200, 200), Color{ 1, 0.3f, 0, 1 });

		if (AEInputCheckTriggered(AEVK_Q))
			projManager.ShootAOE(roomData, player.position, player.direction,
				300.f, 2.f, 10, Vector2(50, 50), Color{ 1, 0, 0, 1 });

		projManager.Update(roomData, dt);  // updates + cleans dead projectiles


	if (testParticles.activeParticleCount < 20) testParticles.CreateParticles(10, 5.0f, Vector2{ 25.0f, 0.0f });
	//else testParticles.DestroyParticles();

	testParticles.UpdateParticles(dt);
	

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
				if (CollisionIntersection_RectRect_Static(
				
				
				
				
				
				{ gift->sprite.position - gift->sprite.scale / 2, gift->sprite.position + gift->sprite.scale / 2 },
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
