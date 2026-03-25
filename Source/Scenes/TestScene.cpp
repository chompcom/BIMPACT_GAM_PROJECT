#include "TestScene.h"

#include "AEEngine.h"
#include "../Sprite.h"

#include "../Player.h"
#include "../Gift.h"
#include "../Utils/Utils.h"
#include "../Enemy.h"
#include "../rooms.h"
#include "../Loaders/DataLoader.h"
#include "../BoundaryCollision.h"
#include "../Collision.h"
#include "../RoomData.h"
#include "Projectile.h"
#include "ProjectileManager.h"
#include "../HUD.h"
#include "../Almanac.h"
#include "ParticleSystem.h"
#include "../Screens/Ui.h"
#include "../GameOver.h"

AEGfxVertexList *sqmesh = nullptr;

TexturedSprite *bulletSprite = nullptr;

AEGfxTexture *rockpng = nullptr;
AEGfxTexture *playerpng = nullptr;
AEGfxTexture *shadowpng = nullptr;
AEGfxTexture *bulletpng = nullptr;
AEGfxTexture *aoepng = nullptr;
AEGfxTexture *heartpng = nullptr;
AEGfxTexture *almanacpng = nullptr;
AEGfxTexture *almanacLitUppng = nullptr;
AEGfxTexture *pDoorTex = nullptr; // Door image
AEGfxTexture *arrowpng = nullptr;

// AEGfxTexture* almanacpagepng = nullptr;

std::vector<TexturedSprite> healthIcons;
TexturedSprite *almanacIcon = nullptr;
TexturedSprite *almanacLitUpIcon = nullptr;
TexturedSprite *arrowSprite = nullptr;
std::vector<TexturedSprite> gameOverButtons;

Sprite gameOverDarkScreen{};

// TexturedSprite* almanacPage = nullptr;
Almanac almanac{};
// std::vector<TexturedSprite> almanacPageSprites;

s8 font = 0;
//Player player{ TexturedSprite(sqmesh,playerpng,Vector2(),Vector2(),Color{1,1,1,1}), TexturedSprite(sqmesh,shadowpng,Vector2(),Vector2(),Color{1,1,1,1}), 25000.f, 600.f, Vector2(0,0) };
//static ProjectileManager projManager;
Player player{TexturedSprite(sqmesh, playerpng, Vector2(), Vector2(), Color{1, 1, 1, 1}), TexturedSprite(sqmesh, shadowpng, Vector2(), Vector2(), Color{1, 1, 1, 1}), 25000.f, 600.f, Vector2(0, 0)};

EnemyType rocktype{"rock", 100, 10, {"sad"}, {"happy"}, {"sad"}};
Enemy rock{rocktype, TexturedSprite(sqmesh, rockpng, Vector2(), Vector2(), Color{1, 1, 1, 1}), TexturedSprite(sqmesh, rockpng, Vector2(), Vector2(), Color{1, 1, 1, 1})};
mapRooms::Map gameMap; // Init var for map
static RoomData globalTransferData{};

ParticleSystem testParticles = NULL;

LV_STATES gameState;

static UIManager pauseUi;
static bool pauseUiInitialized = false;
static char const *pauseTipText = "[TIP]: Press `TAB` to resume";

static UIManager winUi;
static bool winUiInitialized = false;
// static char const* winTipText = "[TIP]: Press `TAB` to resume";

static UIManager loseUi;
static bool loseUiInitialized = false;

bool debugMode;

void TestLoad()
{
	DataLoader::Load();
	sqmesh = CreateSquareMesh();
	rockpng = AEGfxTextureLoad("Assets/poprocks.png");
	playerpng = AEGfxTextureLoad("Assets/player.png");
	shadowpng = AEGfxTextureLoad("Assets/shadow.png");
	bulletpng = AEGfxTextureLoad("Assets/fireball.png");
	heartpng = AEGfxTextureLoad("Assets/heart.png");
	almanacpng = AEGfxTextureLoad("Assets/almanac.png");
	almanacLitUppng = AEGfxTextureLoad("Assets/almanacLitUp.png");
	arrowpng = AEGfxTextureLoad("Assets/arrow.png");

	gameOverDarkScreen = {sqmesh, Vector2(0.f, 0.f), Vector2(1600.f, 900.f), Color{0, 0, 0, 0.8f}};
	font = AEGfxCreateFont("Assets/Kenney Pixel.ttf", 64);

	healthIcons.clear();
	AlmanacFree(almanac);
	
	healthIcons.resize(3, DataLoader::CreateTexture("Assets/heart.png"));
	for (int i{0}; i < 3; ++i)
	{
		healthIcons[i].position = Vector2{-600.5f + (i * 100.f), -350.f};
		healthIcons[i].scale = Vector2{64.f, 64.f};
		healthIcons[i].UpdateTransform();
	}

	LoadAlmanacPages(almanac);
	almanac.pageSprites[0].scale = Vector2(1600.f, 900.f);
	LoadAlmanacEntries(almanac /*, enemyTypeNames*/);

	almanacIcon = new TexturedSprite(sqmesh, almanacpng, Vector2(640.f, -325.f), Vector2(128, 128), Color{1.0, 1.0, 1.0, 0.0});
	almanacLitUpIcon = new TexturedSprite(sqmesh, almanacLitUppng, Vector2(640.f, -325.f), Vector2(128, 128), Color{1.0, 1.0, 1.0, 0.0});
	arrowSprite = new TexturedSprite(sqmesh, arrowpng, Vector2(645.f, -200.f), Vector2(64, 64), Color{1.0, 1.0, 1.0, 0.0});

	player.sprite = TexturedSprite(sqmesh, playerpng, Vector2(300, 300), Vector2(100, 100), Color{1, 1, 1, 0});
	player.shadow = TexturedSprite(sqmesh, shadowpng, Vector2(300, 255), Vector2(100, 100), Color{1, 1, 1, 0});

	InitAudio();
	// Global Data Here
	// I moved these and the init map down to init so it'll reset when the game state is restarted

	//// square seed: 0xA341311Cu
	//// Interesting ones: 32461, 32608, 31931, 18283, 680
	//// Too easy: 32702, 0xA341311Cu,

	////gameMap.InitMap(globalTransferData, 0xA341311Cu);   // Seeded Run
}

void TestInit()
{
	PlayerInit(player);
	AlmanacInit(almanac);

	globalTransferData.enemyList.clear();
	globalTransferData.giftList.clear();
	globalTransferData.projectileList.clear();
	// carryData.enemyList.clear();

	globalTransferData.player = &player;

	// square seed: 0xA341311Cu
	gameMap.InitMap(globalTransferData, 0xA341311Cu); // Seeded Run

	// Enable to allow for random values each run
	std::srand(static_cast<unsigned int>(std::time(nullptr))); // So based on number of seconds passed since Jan 1, 1970, this becomes our srand seed
	unsigned int curSeed = gameMap.RandInt(0, RAND_MAX - 1);
	gameMap.InitMap(globalTransferData, curSeed);
	std::cout << "Current Seed: " << curSeed << "\n";

	testParticles = ParticleSystem(sqmesh);

	// For pause screen;
	pauseUi.LoadFromFilePopUp("Assets/UI/pause_popup.json", Vector2(0.0f, 0.0f), Vector2(580.0f, 250.0f));
	UIElement *tipText = pauseUi.FindById("tip_text");
	if (tipText)
		tipText->text = pauseTipText;
	pauseUiInitialized = true;
	pauseUi.SetFont(font);

	winUi.LoadFromFilePopUp("Assets/UI/win_popup.json", Vector2(0.0f, 0.0f), Vector2(580.0f, 250.0f));
	winUi.BindOnClick("btn_restart", [](UIElement &self)
					  {
						  UNREFERENCED_PARAMETER(self);
						  ChangeState(GS_RESTART); // Apparently game running must be changed too. I thought gsm would handle this lmao.
					  });
	winUi.BindOnClick("btn_mainmenu", [](UIElement &self)
					  {
						  UNREFERENCED_PARAMETER(self);
						  ChangeState(GS_MAINMENU); // Apparently game running must be changed too. I thought gsm would handle this lmao.
					  });
	winUiInitialized = true;
	winUi.SetFont(font);

	loseUi.LoadFromFilePopUp("Assets/UI/lose_popup.json", Vector2(0.0f, 0.0f), Vector2(580.0f, 250.0f));
	loseUi.BindOnClick("btn_restart", [](UIElement &self)
					   {
						   UNREFERENCED_PARAMETER(self);
						   ChangeState(GS_RESTART); // Apparently game running must be changed too. I thought gsm would handle this lmao.
					   });
	loseUi.BindOnClick("btn_mainmenu", [](UIElement &self)
					   {
						   UNREFERENCED_PARAMETER(self);
						   ChangeState(GS_MAINMENU); // Apparently game running must be changed too. I thought gsm would handle this lmao.
					   });
	loseUiInitialized = true;
	loseUi.SetFont(font);

	gameState = RUNNING;
	debugMode = false;
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


	AEGfxSetRenderMode(AE_GFX_RM_TEXTURE);

	// Draw objects: current-room objects + carried objects
	if (mapRooms::Room *room = gameMap.GetCurrentRoom())
	{
		RoomData &roomData = room->currentRoomData;
		RoomData &carryData = gameMap.GetTransferData();

		gameMap.GetCurrentRoom()->roomGrid.RenderGrid(DataLoader::GetMesh(), carryData.player->position, carryData.player->sprite.scale, AE_GFX_RM_TEXTURE);

		for (Gift *g : roomData.giftList)
		{
			if (g)
			{
				if (!g->pickUpState)
					g->shadow.RenderSprite();
				g->sprite.RenderSprite();
			}
		}
		for (Enemy *e : roomData.enemyList)
		{
			if (e && e->isActive)
			{

				e->shadow.RenderSprite();
				e->sprite.RenderSprite();
			}
		}
		if (roomData.boss)
		{
			if (roomData.boss->currentHealth > 0)
			{
				roomData.boss->shadow.RenderSprite();
				roomData.boss->sprite.RenderSprite();
			}
		}

		for (Gift *g : carryData.giftList)
		{
			if (g)
			{
				if (!g->pickUpState)
					g->shadow.RenderSprite();
				g->sprite.RenderSprite();
			}
		}
		for (Enemy *e : carryData.enemyList)
		{
			if (e)
			{
				e->shadow.RenderSprite();
				e->sprite.RenderSprite();
			}
		}

		for (Projectile *p : roomData.projectileList)
			if (p)
				p->RenderProjectile();
		for (Projectile *p : carryData.projectileList)
			if (p)
				p->RenderProjectile();
	}

	player.shadow.RenderSprite();
	player.sprite.RenderSprite(true);
	// rock.sprite.RenderSprite();
	// gift.sprite.RenderSprite();
	// gift2.sprite.RenderSprite();
	gameMap.RenderDebugMap(sqmesh); // Debug Map

	renderPlayerLives(player, healthIcons, font);
	//(*almanacIcon).RenderSprite();

	RenderAlmanacIcon(almanac, *almanacIcon, *almanacLitUpIcon, *arrowSprite);

	RenderAlmanacPages(almanac, font);

	// RenderGameOverScreen(font, gameOverButtons, gameOverDarkScreen, player);

#if 0
	AEGfxSetRenderMode(AE_GFX_RM_COLOR);
	TexturedSprite thing = DataLoader::CreateTexture("Assets/poprocks.png");
	std::pair<int, int> yup{ 5,11 };
	float width = 110;
	thing.scale = Vector2{ width,width };
	//Vector2 offsetpos{ -(yup.second * width) / 2 , (yup.first * width) / 2 };
	Vector2 offsetpos{ -(yup.second * width) / 2, -(yup.first * width) / 2 + 50};
	for (int row = 0; row < yup.first; row++) {

	//gift.sprite.RenderSprite();
	//gift2.sprite.RenderSprite();
	AEGfxSetRenderMode(AE_GFX_RM_COLOR);
	//grid.PrintRetrievedInformation();
	//grid.RenderGrid(sqmesh);

	AEGfxSetRenderMode(AE_GFX_RM_TEXTURE);
	player.sprite.RenderSprite(true);
		for (int col = 0; col < yup.second; col++) {
			thing.position = Vector2{ offsetpos.x + (1.1f*col * width), offsetpos.y + (1.1f*row * width) };
			thing.UpdateTransform();
			thing.RenderSprite();
		}

	}
	AEGfxSetRenderMode(AE_GFX_RM_TEXTURE);
	player.sprite.RenderSprite(true);
#endif

	// Pause screen
	if (gameState == PAUSED)
	{
		AEGfxSetRenderMode(AE_GFX_RM_COLOR);
		AEGfxSetBlendMode(AE_GFX_BM_BLEND);
		// AEGfxSetTransparency(0.1f);

		Sprite overlay(
			DataLoader::GetOrCreateSquareMesh(),
			Vector2(0.0f, 0.0f),
			Vector2(
				AEGfxGetWinMaxX() - AEGfxGetWinMinX(),
				AEGfxGetWinMaxY() - AEGfxGetWinMinY()),
			Color{0.0f, 0.0f, 0.0f, 0.35f});
		overlay.RenderSprite(true);

		pauseUi.Draw();
	}
	else if (gameState == WIN)
	{
		AEGfxSetRenderMode(AE_GFX_RM_COLOR);
		AEGfxSetBlendMode(AE_GFX_BM_BLEND);
		// AEGfxSetTransparency(0.1f);

		Sprite overlay(
			DataLoader::GetOrCreateSquareMesh(),
			Vector2(0.0f, 0.0f),
			Vector2(
				AEGfxGetWinMaxX() - AEGfxGetWinMinX(),
				AEGfxGetWinMaxY() - AEGfxGetWinMinY()),
			Color{0.0f, 0.0f, 0.0f, 0.35f});
		overlay.RenderSprite(true);

		winUi.Draw();
	}
	else if (gameState == LOSE)
	{
		AEGfxSetRenderMode(AE_GFX_RM_COLOR);
		AEGfxSetBlendMode(AE_GFX_BM_BLEND);
		// AEGfxSetTransparency(0.1f);

		Sprite overlay(
			DataLoader::GetOrCreateSquareMesh(),
			Vector2(0.0f, 0.0f),
			Vector2(
				AEGfxGetWinMaxX() - AEGfxGetWinMinX(),
				AEGfxGetWinMaxY() - AEGfxGetWinMinY()),
			Color{0.0f, 0.0f, 0.0f, 0.35f});
		overlay.RenderSprite(true);

		loseUi.Draw();
	}

	if (debugMode)
	{

		char buffer[50];

		sprintf_s(buffer, 50, "DEBUG MODE ON");
		// AEGfxGetPrintSize(font, buffer, 4.f, &textWidth, &textHeight);
		AEGfxPrint(font, buffer, static_cast<f32>(0.6), static_cast<f32>(0.9), static_cast<f32>(1), static_cast<f32>(1.f), static_cast<f32>(1.f), static_cast<f32>(1.f), static_cast<f32>(1.f));
		sprintf_s(buffer, 50, "GAME STATE: ");
		AEGfxPrint(font, buffer, static_cast<f32>(0.6), static_cast<f32>(0.8), static_cast<f32>(1), static_cast<f32>(1.f), static_cast<f32>(1.f), static_cast<f32>(1.f), static_cast<f32>(1.f));
		switch (gameState)
		{
		case RUNNING:
			sprintf_s(buffer, 50, "RUNNING");
			break;
		case PAUSED:
			sprintf_s(buffer, 50, "PAUSED");
			break;
		case WIN:
			sprintf_s(buffer, 50, "YOU WIN");
			break;
		case LOSE:
			sprintf_s(buffer, 50, "YOU LOSE");
			break;
		}
		AEGfxPrint(font, buffer, static_cast<f32>(0.6), static_cast<f32>(0.7), static_cast<f32>(1), static_cast<f32>(1.f), static_cast<f32>(1.f), static_cast<f32>(1.f), static_cast<f32>(1.f));
		// AEGfxGetPrintSize(font, buffer, 4.f, &textWidth, &textHeight);
	}
}

void TestFree()
{
	// RoomData& roomData = gameMap.GetCurrentRoom()->currentRoomData;
	// RoomData& carryData = gameMap.GetTransferData();
	//
	// roomData.enemyList.clear();
	// roomData.giftList.clear();
	// roomData.projectileList.clear();
	// carryData.enemyList.clear();
	// carryData.giftList.clear();

	// globalTransferData.player = &player;
	//  Dellocate enemy and gift assets
	for (Enemy *e : globalTransferData.enemyList)
	{
		delete e;
	}

	for (Gift *g : globalTransferData.giftList)
	{
		delete g;
	}

	for (Projectile *p : globalTransferData.projectileList)
		delete p;

	globalTransferData.player = nullptr;
	gameMap.DeleteMap();

	// if (gameMap.GetCurrentRoom())
	// projManager.Clear(gameMap.GetCurrentRoom()->currentRoomData);
}

void TestUnload()
{
	AEGfxMeshFree(sqmesh);

	delete almanacIcon;
	delete arrowSprite;
	delete almanacLitUpIcon;

	AEGfxTextureUnload(rockpng);
	AEGfxTextureUnload(playerpng);
	AEGfxTextureUnload(shadowpng);
	AEGfxTextureUnload(bulletpng);

	AEGfxTextureUnload(heartpng);
	// AEGfxTextureUnload(heartpng);
	AEGfxTextureUnload(almanacpng);
	AEGfxTextureUnload(almanacLitUppng);
	AEGfxTextureUnload(arrowpng);

	AEGfxDestroyFont(font);

	// moved to free to prevent mem leaks
	//// Dellocate enemy and gift assets
	// for (Enemy *e: globalTransferData.enemyList) {
	//	delete e;
	// }

	// for (Gift* g : globalTransferData.giftList) {
	//	delete g;
	// }

	// for (Projectile* p : globalTransferData.projectileList) delete p;
	//
	// globalTransferData.player = nullptr;
	// gameMap.DeleteMap();
	DataLoader::Unload();
	if (gameMap.GetCurrentRoom())
		ProjectileClear(gameMap.GetCurrentRoom()->currentRoomData);
	FreeAudio();

	pauseUi.Clear();
	pauseUiInitialized = false;

	winUi.Clear();
	winUiInitialized = false;

	loseUi.Clear();
	loseUiInitialized = false;
	// isPaused = false;
}

void TestUpdate(float dt)
{
	//if (player.health <= 0)
	//{
	//	//HandleGameOverInputs(gameOverButtons);
	//	return;
	//}

	if (gameState == RUNNING)
	{
		// Pause toggle
		if (AEInputCheckTriggered(AEVK_TAB))
		{
			gameState = PAUSED;
		}

		// Get previous pos
		Vector2 prevPos{player.position.x, player.position.y};

		// to test damage
		if (player.health > 0)
		{
			if (AEInputCheckTriggered(AEVK_P))
				playerTakesDamage(player);
			if (AEInputCheckTriggered(AEVK_O))
				playerHealsDamage(player);

			if (AEInputCheckTriggered(AEVK_M))
				PlayerInit(player);

			checkIfAlmanacClicked(*almanacIcon, almanac);

			AlmanacInputs(almanac);

			// for now,
			//  Player update
			UpdatePlayer(player, dt);
			player.sprite.UpdateTransform();
			player.shadow.UpdateTransform();
		}

	//winUI.Update();

	// Get previous pos
	//Vector2 prevPos{ player.position.x, player.position.y };
	
	//UpdatePlayer(player, dt); // Player update
		Vector2 playerHalfSize = player.sprite.scale * 0.5f;

	// Print Current Grid
	//std::cout << "Grid Current: " << gameMap.GetCurrentRoom()->roomGrid.WorldToCell(player.position.x, player.position.y) << "\n";
	//for (int i = 0; i < 9; ++i) {
	//	for (int j = 0; j < 12; ++j) std::cout << gameMap.GetCurrentRoom()->roomGrid.GetCell(j, i) << " ";
	//	std::cout << '\n';
	//}

		// Game map update
		gameMap.GetCurrentRoom()->Update(dt);

		mapRooms::Room *currentRoom = gameMap.GetCurrentRoom();
		RoomData &roomData = currentRoom->currentRoomData;
		RoomData &carryData = gameMap.GetTransferData();
		

		// Test Player Collision with Map
		int curCell = gameMap.GetCurrentRoom()->roomGrid.WorldToCell(player.position.x, player.position.y);
		if (curCell >= 0 && curCell != 0xffffff)
			currentRoom->lastValidCell = curCell;
		int colRes = gameMap.GetCurrentRoom()->roomGrid.CheckMapGridCollision(player.position.x, player.position.y, player.sprite.scale.x, player.sprite.scale.y, curCell);
		if (colRes & COLLISION_LEFT || colRes & COLLISION_RIGHT)
			player.position.x = prevPos.x; // Test for x collision
		if (colRes & COLLISION_TOP || colRes & COLLISION_BOTTOM)
			player.position.y = prevPos.y; // Test for y collision

		// Vector2 playerHalfSize = player.sprite.scale * 0.5f;

		// Finally reflect changes?
		player.sprite.UpdateTransform();
		player.shadow.UpdateTransform();

		// Game Map Update was here

		// to test damage
		if (AEInputCheckTriggered(AEVK_P))
			playerTakesDamage(player);
		if (AEInputCheckTriggered(AEVK_O))
			playerHealsDamage(player);

	checkIfAlmanacClicked(*almanacIcon, almanac);

	MoveArrow(*arrowSprite, almanac, dt);

		// std::cout << player.position.x << player.position.y;

		// Update Enemies (carryData version is only for "Friends")
		for (Enemy *e : roomData.enemyList)
		{
			if (e)
			{
				// e->target.position = *player.sprite.position;
				e->Update(dt);
				if (CollisionBoundary_Static(e->sprite.position, e->sprite.scale, 1400, 700))
					e->velocity = -e->velocity;
			}
		}

		//Friends of the player!
		for (Enemy *e : carryData.enemyList)
		{
			if (e)
			{
				e->Update(dt);

				//do collision test with each other friend
				for (Enemy* b : carryData.enemyList) {
					//If enemy is the same
					if (e == b) continue;
					float ti{};
					if (
						CollisionIntersection_RectRect(e->sprite.position, e->sprite.scale, e->velocity, b->sprite.position, b->sprite.scale, b->velocity, ti)
						) 
					{
						Vector2 dir = b->sprite.position - (e->sprite.position + Vector2(AERandFloat(),AERandFloat()));
						e->sprite.position -= dir.Normalized() * b->sprite.scale.x * 0.5f *dt;
						b->sprite.position += dir.Normalized() * b->sprite.scale.x * 0.5f * dt;
					}
				}

				// Test Player Collision with Map
				/*int enemyCurCell = gameMap.GetCurrentRoom()->roomGrid.WorldToCell(e->sprite.position.x, e->sprite.position.y);
				if (enemyCurCell >= 0 && enemyCurCell != 0xffffff)
					currentRoom->lastValidCell = enemyCurCell;
				int enemyColRes = gameMap.GetCurrentRoom()->roomGrid.CheckMapGridCollision(e->sprite.position.x, e->sprite.position.y, e->sprite.scale.x, e->sprite.scale.y, enemyCurCell);
				if (enemyColRes & COLLISION_LEFT || enemyColRes & COLLISION_RIGHT) {
					e->sprite.position.x = e->prevPos.x; // Test for x collision
					e->shadow.position.x = e->prevPos.x;
					e->velocity = -e->velocity;
				}
				if (enemyColRes & COLLISION_TOP || enemyColRes & COLLISION_BOTTOM) {
					e->sprite.position.y = e->prevPos.y; // Test for y collision
					e->shadow.position.y = e->prevPos.y;
					e->velocity = -e->velocity;
				}*/
				if (CollisionBoundary_Static(e->sprite.position, e->sprite.scale, 1400, 700))
					e->velocity = -e->velocity;
			}
		}

		// Update Gifts (Must update both sides)
		for (Gift *g : roomData.giftList)
		{
			if (g)
			{
				UpdateGift(*g, player, dt);
				g->sprite.UpdateTransform();
				g->shadow.UpdateTransform();
			}
		}
		for (Gift *g : carryData.giftList)
		{
			if (g)
			{
				UpdateGift(*g, player, dt);
				g->sprite.UpdateTransform();
				g->shadow.UpdateTransform();
			}
		}

		CheckProjectileCollision(roomData, *roomData.player);
		UpdateProjectiles(roomData, dt);

		if (roomData.boss)
		{
			Vector2 bossPrevPos = roomData.boss->sprite.position;

			roomData.boss->Update(player, dt);

			roomData.boss->collideWall = false;

			// Test Player Collision with Map
			int bossCurCell = gameMap.GetCurrentRoom()->roomGrid.WorldToCell(roomData.boss->sprite.position.x, roomData.boss->sprite.position.y);
			if (bossCurCell >= 0 && bossCurCell != 0xffffff)
				currentRoom->lastValidCell = bossCurCell;
			int bossColRes = gameMap.GetCurrentRoom()->roomGrid.CheckMapGridCollision(roomData.boss->sprite.position.x, roomData.boss->sprite.position.y, roomData.boss->sprite.scale.x, roomData.boss->sprite.scale.y, bossCurCell);
			if ((bossColRes & COLLISION_LEFT || bossColRes & COLLISION_RIGHT) && roomData.boss->bossStateMachine->currentState != BOSS_JUMP) {
				roomData.boss->sprite.position.x = bossPrevPos.x; // Test for x collision
				roomData.boss->shadow.position.x = bossPrevPos.x;
				roomData.boss->collideWall = true;
			}
			if ((bossColRes & COLLISION_TOP || bossColRes & COLLISION_BOTTOM) && roomData.boss->bossStateMachine->currentState != BOSS_JUMP) {
				roomData.boss->sprite.position.y = bossPrevPos.y; // Test for y collision
				roomData.boss->shadow.position.y = bossPrevPos.y - 35;
				roomData.boss->collideWall = true;
			}

			roomData.boss->sprite.UpdateTransform();
			roomData.boss->shadow.UpdateTransform();
		}

		// Gifts and Enemy Check
		for (Gift *gift : currentRoom->currentRoomData.giftList)
		{
			if (!(gift->velocity == Vector2()))
			{
				for (Enemy *e : currentRoom->currentRoomData.enemyList)
				{
					if (!e->isActive) continue;
					if (AreSquaresIntersecting(gift->sprite.position, gift->sprite.scale.x, e->sprite.position, e->sprite.scale.x))
					{
						gift->velocity = -gift->velocity;
						e->AssessTraits(gift->traits);
					}
				}
				//if (CollisionBoundary_Static(gift->sprite.position, gift->sprite.scale, 1200, 600))
					//gift->velocity = -gift->velocity;
			}
		}

		// 4) Transfer enemies to our carrylist if they are essentially happy
		for (size_t i = 0; i < roomData.enemyList.size();)
		{
			Enemy *e = roomData.enemyList[i];
			if (e && e->state == EnemyStates::ES_HAPPY) // This is the happy check ig?
			{
				carryData.enemyList.push_back(e);
				roomData.enemyList.erase(roomData.enemyList.begin() + static_cast<long>(i));
				continue;
			}
			++i;
		}

		// Idk how to check which gifts to pick up
		for (size_t i = 0; i < roomData.giftList.size();)
		{
			Gift *g = roomData.giftList[i];
			if (g && g->pickUpState) // If currently picked up, need to check pickupstate = false and remove such????
			{
				carryData.giftList.push_back(g);										   // Transfer to carryData list (active list essentially)
				roomData.giftList.erase(roomData.giftList.begin() + static_cast<long>(i)); // Remove from current roomData
				break;
			}
			++i;
		}

		// Check if gifts are indeed pickedup
		// Idk how to check which gifts to pick up
		for (size_t i = 0; i < carryData.giftList.size();)
		{
			Gift *g = carryData.giftList[i];
			if (g && g->pickUpState == false) // If currently picked up, need to check pickupstate = false and remove such????
			{
				roomData.giftList.push_back(g);
				carryData.giftList.erase(carryData.giftList.begin() + static_cast<long>(i)); // Remove from current carryData
				continue;
			}
			++i;
		}

		// Room grid works! (To be removed)
		// std::cout << "Cell idx: " << currentRoom->roomGrid.WorldToCell(player.position.x, player.position.y) << std::endl;
		// std::cout << "x: " << player.position.x << "y: " << player.position.y;

		// Update game map
		Vector2 positionResetTest = player.position;
		gameMap.UpdateMap(player.position, playerHalfSize, testParticles, dt);
		/* IF player position is changed to other screen, reset enemies to player position */
		if (player.position != positionResetTest)
		{
			for (Enemy *e : carryData.enemyList)
			{
				e->sprite.position = player.position;
				e->roomData = &gameMap.GetCurrentRoom()->currentRoomData;
				// assess friend traits when entering a new room!
				for (Enemy* enemy : gameMap.GetCurrentRoom()->currentRoomData.enemyList) {
					enemy->AssessTraits(e->type.traits, false);
				}
			}
			for (Projectile *p : roomData.projectileList)
			{
				p->RemoveProjectile();
			}
		}
		if (AEInputCheckTriggered(AEVK_2)) {
			ShootRounding(DataLoader::CreateTexture("Assets/fireball.png"), roomData, { 30,30 }// player.position
				, player.direction,
				100.0f, 7.0f, 10, Vector2(30, 30), Color{ 1, 0.3f, 0, 1 });
		}
		if (AEInputCheckTriggered(AEVK_3)) {
			ShootScatter(DataLoader::CreateTexture("Assets/fireball.png"), roomData, { 30,30 }// player.position
				, player.direction,
				500.f, 0.5f, 10, Vector2(50, 50), Color{ 1, 0.3f, 0, 1 });
		}
		if (AEInputCheckTriggered(AEVK_4)) {
			ShootBoomerang(DataLoader::CreateTexture("Assets/fireball.png"), roomData, { 30,30 }// player.position
				, player.direction,
				500.f, 2.0f, 10, Vector2(50, 50), Color{ 1, 0.3f, 0, 1 });
		}
		/*
		if (AEInputCheckTriggered(AEVK_SPACE))
			projManager.ShootFireball(roomData, player.position, player.direction,
				500.f, 2.f, 10, Vector2(200, 200), Color{ 1, 0.3f, 0, 1 });

			if (AEInputCheckTriggered(AEVK_Q))
				projManager.ShootAOE(roomData, player.position, player.direction,
					300.f, 2.f, 10, Vector2(50, 50), Color{ 1, 0, 0, 1 });

			projManager.Update(roomData, dt);  // updates + cleans dead projectiles
			*/

		// testParticles.UpdateParticles(dt);

		// Legacy: TO BE COPIED INTO ROOM COLLISION DETECTION CLASS (BUT THERE'S NOTHING YET EVEN???)
		// for (Gift* gift : roomData.giftList) {
		//	if (!(gift->velocity == Vector2())) {
		//		if (AreSquaresIntersecting(gift->sprite.position, gift->sprite.scale.x, rock.sprite.position, rock.sprite.scale.x)) {
		//			gift->velocity = -gift->velocity;
		//			rock.AssessTraits(gift->traits);
		//		}
		//	}
		//}

		// std::vector<Gift*> things{ &gift,&gift2 };

		// thing->position += Vector2(10,10) * dt;
		// thing->UpdateTransform();

		// rock.Update(dt);
		// rock.target = player.sprite.position;

		// UpdatePlayer(player, dt);
		// player.sprite.UpdateTransform();
		// UpdateGift(gift, player, dt);
		// gift.sprite.UpdateTransform();
		// UpdateGift(gift2, player, dt);
		// gift2.sprite.UpdateTransform();

		// thing->position += Vector2(10,10) * dt;
		// thing->UpdateTransform();
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


		Room {
			roomdata* toBeTransfered;
			roomdata currentRoomData;

		}

		gameMap.currentRoom.


		//if its at a door, roomdata gets transferred




		*/
	}
	else if (gameState == PAUSED)
	{
		// When paused:
		// - update only the pause UI
		if (AEInputCheckTriggered(AEVK_TAB))
		{
			gameState = RUNNING;
		}

		pauseUi.Update();
	}
	else if (gameState == WIN)
	{
		winUi.Update();
	}
	else if (gameState == LOSE)
	{
		loseUi.Update();
	}

	if (debugMode)
	{
		if (AEInputCheckTriggered(AEVK_1))
			gameState = RUNNING;
		if (AEInputCheckTriggered(AEVK_2))
			gameState = PAUSED;
		if (AEInputCheckTriggered(AEVK_3))
			gameState = WIN;
		if (AEInputCheckTriggered(AEVK_4))
			gameState = LOSE;
	}

	if (AEInputCheckTriggered(AEVK_F3))
		debugMode = !debugMode;
}
