#include "TestScene.h"

#include <algorithm>

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
#include "GameStateList.h"
#include "GameStateManager.h"
#include "../Screens/Tutorial.h"

AEGfxVertexList *sqmesh = nullptr;

TexturedSprite *bulletSprite = nullptr;

//AEGfxTexture *rockpng = nullptr;
AEGfxTexture *playerpng = nullptr;
AEGfxTexture *shadowpng = nullptr;
AEGfxTexture* hitboxpng = nullptr;
//AEGfxTexture *bulletpng = nullptr;
//AEGfxTexture *aoepng = nullptr;
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
Player player{TexturedSprite(sqmesh, playerpng, Vector2(), Vector2(), Color{1, 1, 1, 1}), TexturedSprite(sqmesh, shadowpng, Vector2(), Vector2(), Color{1, 1, 1, 1}),  TexturedSprite(sqmesh, hitboxpng, Vector2(), Vector2(), Color{1, 1, 1, 1}), 2500.f, 100.f, Vector2(0, 0)};

//EnemyType rocktype{"rock", 100, 10, {"sad"}, {"happy"}, {"sad"}};
//Enemy rock{rocktype, TexturedSprite(sqmesh, rockpng, Vector2(), Vector2(), Color{1, 1, 1, 1}), TexturedSprite(sqmesh, rockpng, Vector2(), Vector2(), Color{1, 1, 1, 1})};
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

static UIManager tutorialUi;

static bool fightMusicPlaying = false;
static bool loseAudioPlaying = false;
static bool WinAudioPlaying = false;

bool debugMode;

void TestLoad()
{
	DataLoader::Load();
	sqmesh = CreateSquareMesh();
	//rockpng = AEGfxTextureLoad("Assets/poprocks.png");
	playerpng = AEGfxTextureLoad("Assets/player.png");
	shadowpng = AEGfxTextureLoad("Assets/shadow.png");
	hitboxpng = AEGfxTextureLoad("Assets/hitbox.png");
	//bulletpng = AEGfxTextureLoad("Assets/fireball.png");
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
	arrowSprite = new TexturedSprite(sqmesh, arrowpng, Vector2(645.f, -180.f), Vector2(128, 128), Color{1.0, 1.0, 1.0, 0.0});

	player.sprite = TexturedSprite(sqmesh, playerpng, Vector2(300, 300), Vector2(100, 100), Color{1, 1, 1, 0});
	player.shadow = TexturedSprite(sqmesh, shadowpng, Vector2(300, 255), Vector2(100, 100), Color{1, 1, 1, 0});
	player.hitbox = TexturedSprite(sqmesh, hitboxpng, Vector2(300, 300), Vector2(100, 100), Color{ 1, 1, 1, 0 });

	
	// Global Data Here
	// I moved these and the init map down to init so it'll reset when the game state is restarted

	//// square seed: 0xA341311Cu
	//// Interesting ones: 32461, 32608, 31931, 18283, 680
	//// Too easy: 32702, 0xA341311Cu,

	////gameMap.InitMap(globalTransferData, 0xA341311Cu);   // Seeded Run
	//load the gift types if they arent loaded
	if (allGiftTypes.empty()) LoadGiftTypes();
}

void TestInit()
{
	InitAudio();
	loseAudioPlaying = false;
	WinAudioPlaying = false;
	fightMusicPlaying = false;
	PlayerInit(player);
	AlmanacInit(almanac);

	globalTransferData.enemyList.clear();
	globalTransferData.giftList.clear();
	globalTransferData.projectileList.clear();
	// carryData.enemyList.clear();

	globalTransferData.player = &player;

	// square seed: 0xA341311Cu
	//gameMap.InitMap(globalTransferData, 0xA341311Cu); // Seeded Run

	// Enable to allow for random values each run
	std::srand(static_cast<unsigned int>(std::time(nullptr))); // So based on number of seconds passed since Jan 1, 1970, this becomes our srand seed
	unsigned int curSeed = gameMap.RandInt(0, RAND_MAX - 1);
	gameMap.InitMap(globalTransferData, curSeed);
	//std::cout << "Current Seed: " << curSeed << "\n";

	testParticles = ParticleSystem(sqmesh);

	// For pause screen;
	pauseUi.LoadFromFilePopUp("Assets/UI/pause_popup.json", Vector2(0.0f, 0.0f), Vector2(580.0f, 250.0f));
	//UIElement *tipText = pauseUi.FindById("tip_text");
	//if (tipText)
		//tipText->text = pauseTipText;
	pauseUi.BindOnClick("btn_restart", [](UIElement& self)
		{
			UNREFERENCED_PARAMETER(self);
			ChangeState(GS_RESTART); // Apparently game running must be changed too. I thought gsm would handle this lmao.
		});
	pauseUi.BindOnClick("btn_mainmenu", [](UIElement& self)
		{
			UNREFERENCED_PARAMETER(self);
			ChangeState(GS_MAINMENU); // Apparently game running must be changed too. I thought gsm would handle this lmao.
		});
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

	InitTutorial();

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

		//gameMap.GetCurrentRoom()->roomGrid.RenderGrid(DataLoader::GetMesh(), carryData.player->position, carryData.player->sprite.scale, AE_GFX_RM_TEXTURE);

		for (Gift *g : roomData.giftList)
		{
			if (g)
			{
				if (!g->pickUpState)
					g->shadow.RenderSprite();
				g->giftType.sprite.RenderSprite();
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
				roomData.boss->sprite.RenderSprite(false, roomData.boss->sprite.current_sprite_uv_offset_x, roomData.boss->sprite.current_sprite_uv_offset_y);
			}
		}

		for (Gift *g : carryData.giftList)
		{
			if (g)
			{
				if (!g->pickUpState)
					g->shadow.RenderSprite();
				g->giftType.sprite.RenderSprite();
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
	AEGfxSetRenderMode(AE_GFX_RM_COLOR);
	for (Particle& particle : 
	gameMap.GetCurrentRoom()->currentRoomData.particleSystem.particles) 
	{
		
		if (particle.isActive) {
			Sprite darker = particle.sprite;

			float colorOffset = 0.3f;

			darker.color.r -= colorOffset;
			darker.color.g -= colorOffset;
			darker.color.b -= colorOffset;

			AEClamp(darker.color.r, 0.0f, 1.0f);
			AEClamp(darker.color.g, 0.0f, 1.0f);
			AEClamp(darker.color.b, 0.0f, 1.0f);

			darker.scale *= 1.5f;
			darker.UpdateTransform();
			darker.RenderSprite();
			particle.sprite.RenderSprite();	
		}
	}
	

	AEGfxSetRenderMode(AE_GFX_RM_TEXTURE);

	if (gameMap.GetCurrentRoom()->currentRoomData.boss) {//gameMap.GetCurrentRoom()->currentRoomData.boss->hpBar.RenderSprite();
		if (gameMap.GetCurrentRoom()->currentRoomData.boss->healthbarInitialized) {
			//std::cout << "true\n";
			gameMap.GetCurrentRoom()->currentRoomData.boss->healthbar.Draw();
		}
	}
	// rock.sprite.RenderSprite();
	// gift.sprite.RenderSprite();
	// gift2.sprite.RenderSprite();
	gameMap.RenderDebugMap(sqmesh); // Debug Map

	renderPlayerLives(player, healthIcons, font);
	//(*almanacIcon).RenderSprite();

	RenderAlmanacIcon(almanac, *almanacIcon, *almanacLitUpIcon, *arrowSprite, player);

	RenderAlmanacPages(almanac, font);

	RenderTutorial();

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
	if (debugMode)
	{
		if (mapRooms::Room* room = gameMap.GetCurrentRoom())
		{
			RoomData& roomData = room->currentRoomData;
			RoomData& carryData = gameMap.GetTransferData();

			gameMap.GetCurrentRoom()->roomGrid.RenderGrid(DataLoader::GetMesh(), carryData.player->position, carryData.player->sprite.scale, AE_GFX_RM_TEXTURE);

			for (Gift* g : roomData.giftList)
			{
				if (g)
				{
					/*if (!g->pickUpState)
						g->shadow.RenderSprite();
					g->giftType.sprite.RenderSprite();*/
					g->hitbox.RenderSprite();
				}
			}
			for (Enemy* e : roomData.enemyList)
			{
				if (e && e->isActive)
				{

					/*e->shadow.RenderSprite();
					e->sprite.RenderSprite();*/
					e->hitbox.RenderSprite();
				}
			}
			if (roomData.boss)
			{
				if (roomData.boss->currentHealth > 0)
				{
					/*roomData.boss->shadow.RenderSprite();
					roomData.boss->sprite.RenderSprite(false, roomData.boss->sprite.current_sprite_uv_offset_x, roomData.boss->sprite.current_sprite_uv_offset_y);*/\
						roomData.boss->hitbox.RenderSprite();
				}
			}

			for (Gift* g : carryData.giftList)
			{
				if (g)
				{
					/*if (!g->pickUpState)
						g->shadow.RenderSprite();
					g->giftType.sprite.RenderSprite();*/
					g->hitbox.RenderSprite();
				}
			}
			for (Enemy* e : carryData.enemyList)
			{
				if (e)
				{
					/*e->shadow.RenderSprite();
					e->sprite.RenderSprite();*/
					e->hitbox.RenderSprite();
				}
			}
		}

		player.hitbox.RenderSprite();

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

		sprintf_s(buffer, 50, "%.2f", AEFrameRateControllerGetFrameRate());
		AEGfxPrint(font, buffer, static_cast<f32>(0.6), static_cast<f32>(0.6), static_cast<f32>(1), static_cast<f32>(1.f), static_cast<f32>(1.f), static_cast<f32>(1.f), static_cast<f32>(1.f));

		// AEGfxGetPrintSize(font, buffer, 4.f, &textWidth, &textHeight);
	}

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

}

void TestUnload()
{
	AEGfxMeshFree(sqmesh);

	//#warning "there are icons being deleted, are we not using dataloader?"	
	delete almanacIcon;
	delete arrowSprite;
	delete almanacLitUpIcon;

	//#warning "GET RID OF THE TEST PNGS"
	//AEGfxTextureUnload(rockpng);
	AEGfxTextureUnload(playerpng);
	AEGfxTextureUnload(shadowpng);
	AEGfxTextureUnload(hitboxpng);
	//AEGfxTextureUnload(bulletpng);

	AEGfxTextureUnload(heartpng);
	AEGfxTextureUnload(almanacpng);
	AEGfxTextureUnload(almanacLitUppng);
	AEGfxTextureUnload(arrowpng);

	AEGfxDestroyFont(font);

	
	DataLoader::Unload();
	if (gameMap.GetCurrentRoom())
		ProjectileClear(gameMap.GetCurrentRoom()->currentRoomData);
	

	pauseUi.Clear();
	pauseUiInitialized = false;

	winUi.Clear();
	winUiInitialized = false;

	loseUi.Clear();
	loseUiInitialized = false;
	// isPaused = false;

	allGiftTypes.clear();

	TutorialUnload();
}

void TestUpdate(float dt)
{

	if (gameState == RUNNING)
	{
		// Pause toggle
		if (AEInputCheckTriggered(AEVK_ESCAPE) || AEInputCheckTriggered(AEVK_TAB))
		{
			gameState = PAUSED;
		}

		// Get previous pos
		Vector2 prevPos{ player.position.x, player.position.y };

		//// to test damage
		//if (player.health > 0)
		//{
		//	//this has to be above checkIfAlmanacClicked or the arrows will bug
		//	AlmanacInputs(almanac);
		//	checkIfAlmanacClicked(*almanacIcon, almanac);

		//	// for now,
		//	//  Player update
		//	UpdatePlayer(player, dt);
		//	player.sprite.UpdateTransform();
		//	player.shadow.UpdateTransform();
		//}

		//this has to be above checkIfAlmanacClicked or the arrows will bug
		AlmanacInputs(almanac);
		checkIfAlmanacClicked(*almanacIcon, almanac);
		TutorialInput();
		// for now,
		//  Player update
		UpdatePlayer(player, dt);
		//player.sprite.UpdateTransform();
		//player.shadow.UpdateTransform();

		UpdateMobAudioCD(dt);
		//winUI.Update();

		Vector2 playerHalfSize = player.sprite.scale * 0.5f;

		// Game map update
		gameMap.GetCurrentRoom()->Update(dt);

		mapRooms::Room* currentRoom = gameMap.GetCurrentRoom();
		RoomData& roomData = currentRoom->currentRoomData;
		RoomData& carryData = gameMap.GetTransferData();


		//std::cout << currentRoom->biome << std::endl;
		static mapRooms::Room* lastRoom = nullptr;
		if (currentRoom != lastRoom)
		{
			lastRoom = currentRoom;
			ResetBGM();
			if (currentRoom->biome == "Green") ForestBiomeAudio();
			if (currentRoom->biome == "Ice") IceBiomeAudio();
			if (currentRoom->biome == "Ocean") OceanBiomeAudio();
			if (currentRoom->biome == "Normal");
			if (roomData.boss) {
				BossBGMAudio();
				//return;

			}
		}

		bool isAngry = false;
		for (Enemy* e : roomData.enemyList) {
			if (e && e->isActive && e->state == ES_ANGRY) {
				isAngry = true;
				break;
			}
		}
		if (isAngry && !fightMusicPlaying) {
			ResetBGM();
			FightMusicAudio();
			fightMusicPlaying = true;
		}
		else if (!isAngry && fightMusicPlaying) {
			ResetBGM();
			fightMusicPlaying = false;
		}
		if (!roomData.boss && !fightMusicPlaying) RandomBGMAudio(dt);

		// FOR DOOR
		int prevCell = currentRoom->roomGrid.WorldToCell(prevPos.x, prevPos.y);
		if (prevCell < 0 && currentRoom->lastValidCell >= 0) prevCell = currentRoom->lastValidCell;

		// TEST PLAYER COLLISION WITH MAP
		Vector2 moveDelta = player.position - prevPos;
		Vector2 moveDir = moveDelta.Normalized();
		int curCell = currentRoom->roomGrid.WorldToCell(player.position.x, player.position.y);
		if (curCell >= 0 && curCell != 0xffffff) currentRoom->lastValidCell = curCell;

		// Scale size 0.8f
		float collisionScaleX = player.sprite.scale.x * 0.8f;
		float collisionScaleY = player.sprite.scale.y * 0.8f;

		int colRes = currentRoom->roomGrid.CheckMapGridCollision(player.position.x, player.position.y, collisionScaleX, collisionScaleY, prevCell);

		float gridWidth = currentRoom->roomGrid.GetTileWidth();
		float gridHeight = currentRoom->roomGrid.GetTileHeight();
		constexpr float skin = 0.10f;

		if (prevCell >= 0)
		{
			Vector2 prevCellCenter = currentRoom->roomGrid.CellToWorldCenter(prevCell);
			if ((colRes & COLLISION_LEFT) &&	moveDir.x < - EPSILON)	player.position.x = prevCellCenter.x - gridWidth * 0.5f + collisionScaleX * 0.5f + skin;
			if ((colRes & COLLISION_RIGHT) &&	moveDir.x >   EPSILON)	player.position.x = prevCellCenter.x + gridWidth * 0.5f - collisionScaleX * 0.5f - skin;
			if ((colRes & COLLISION_TOP) &&		moveDir.y >   EPSILON)	player.position.y = prevCellCenter.y + gridHeight * 0.5f - collisionScaleY * 0.5f - skin;
			if ((colRes & COLLISION_BOTTOM) &&	moveDir.y < - EPSILON)	player.position.y = prevCellCenter.y - gridHeight * 0.5f + collisionScaleY * 0.5f + skin;
		}

		// sync sprite + shadow AFTER collision correction
		player.sprite.position = player.position;
		player.shadow.position = player.position - Vector2{ 0, 40 };
		player.hitbox.position = player.position;

		player.sprite.UpdateTransform();
		player.shadow.UpdateTransform();
		player.hitbox.UpdateTransform();

		// Game Map Update was here

		//checkIfAlmanacClicked(*almanacIcon, almanac);

		MoveArrow(*arrowSprite, almanac, dt);

		// Update Enemies (carryData version is only for "Friends")
		for (Enemy* e : roomData.enemyList)
		{
			if (e)
			{
				e->Update(dt);

				e->hitbox.position = e->sprite.position;
				e->hitbox.UpdateTransform();
			}
		}

		//Friends of the player!
		for (Enemy* e : carryData.enemyList)
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
						Vector2 dir = b->sprite.position - (e->sprite.position + Vector2(AERandFloat(), AERandFloat()));
						e->sprite.position -= dir.Normalized() * b->sprite.scale.x * 0.5f * dt;
						b->sprite.position += dir.Normalized() * b->sprite.scale.x * 0.5f * dt;
					}
				}

				e->hitbox.position = e->sprite.position;
				e->hitbox.UpdateTransform();

			
				//if (CollisionBoundary_Static(e->sprite.position, e->sprite.scale, 1600, 800))
					//e->velocity = -e->velocity;
			}
		}


		for (Gift* g : roomData.giftList)
		{
			if (g)
			{
				Vector2 prevGiftPos = g->position;
				prevCell = currentRoom->roomGrid.WorldToCell(prevGiftPos.x, prevGiftPos.y);

				float gridWidth = currentRoom->roomGrid.GetTileWidth();
				float gridHeight = currentRoom->roomGrid.GetTileHeight();
				constexpr float skin = 0.10f;

				UpdateGift(*g, player, dt, currentRoom->roomGrid.GetBoundary() * 0.99f, currentRoom);

				Vector2 moveDelta = g->position - prevGiftPos;
				Vector2 moveDir = moveDelta.Normalized();

				float speed = sqrtf(g->velocity.x * g->velocity.x + g->velocity.y * g->velocity.y);

				// Increase scale as speed goes up hacky method
				float collisionScaleX = AEClamp(speed / 2000.0f * g->giftType.sprite.scale.x, g->giftType.sprite.scale.x, g->giftType.sprite.scale.x * 2.0f);
				float collisionScaleY = AEClamp(speed / 2000.0f * g->giftType.sprite.scale.y, g->giftType.sprite.scale.y, g->giftType.sprite.scale.y * 2.0f);
				int res = currentRoom->roomGrid.CheckMapGridCollision(g->position.x,g->position.y, collisionScaleX, collisionScaleY,prevCell);

				bool hitX = (res & COLLISION_LEFT) || (res & COLLISION_RIGHT);
				bool hitY = (res & COLLISION_TOP) || (res & COLLISION_BOTTOM);

				if (res && prevCell >= 0)
				{
					Vector2 prevCellCenter = currentRoom->roomGrid.CellToWorldCenter(prevCell);

					float rubberBand = 0.1f;
			
					// JOSIAH'S inspiration from enemies method (0.1 instead of 0.01 to prevent rubberband?)
					if ((res & COLLISION_LEFT) && moveDir.x <  -rubberBand)  g->position.x = prevCellCenter.x - gridWidth * 0.5f + collisionScaleX * 0.5f + skin;
					if ((res & COLLISION_RIGHT) && moveDir.x >  rubberBand)  g->position.x = prevCellCenter.x + gridWidth * 0.5f - collisionScaleX * 0.5f - skin;
					if ((res & COLLISION_BOTTOM) && moveDir.y < -rubberBand) g->position.y = prevCellCenter.y - gridHeight * 0.5f + collisionScaleY * 0.5f + skin;
					if ((res & COLLISION_TOP) && moveDir.y >    rubberBand)  g->position.y = prevCellCenter.y + gridHeight * 0.5f - collisionScaleY * 0.5f - skin;

					// If basically stopped, try prevent rubberbanding for long?
					if (moveDelta.LengthSq() < (rubberBand*rubberBand))
					{
						if (hitX) g->velocity.x = 0.0f;
						if (hitY) g->velocity.y = 0.0f;
					}
					else
					{
						// Bounce once per axis
						if (hitX) g->velocity.x *= -1.0f;
						if (hitY) g->velocity.y *= -1.0f;

						// Dampen sequence
						if (hitX && hitY)
						{
							g->velocity /= 1.2f;
						}
						else if (hitX)
						{
							g->velocity.x /= 1.1f;
							g->velocity.y /= 1.5f;
						}
						else if (hitY)
						{
							g->velocity.y /= 1.1f;
							g->velocity.x /= 1.5f;
						}
					}
				}
				else if (res)
				{
					// Fallback if prevCell is invalid (idk what else to fallback alr)
					g->position = prevGiftPos;
					g->velocity = Vector2{ 0.0f, 0.0f };
				}

				g->giftType.sprite.position = g->position;
				g->shadow.position = g->position - Vector2{ 0, 40 };
				g->hitbox.position = g->position;

				g->giftType.sprite.UpdateTransform();
				g->shadow.UpdateTransform();
				g->hitbox.UpdateTransform();
			}
		}

		for (Gift* g : carryData.giftList)
		{
			if (g)
			{
				//int prevCell = currentRoom->roomGrid.WorldToCell(g->position.x, g->position.y);
				UpdateGift(*g, player, dt, Vector2{AEGfxGetWindowWidth(), AEGfxGetWindowHeight()}, currentRoom);
				g->giftType.sprite.UpdateTransform();
				g->shadow.UpdateTransform();
			}
		}


		CheckProjectileCollision(roomData, *roomData.player);
		UpdateProjectiles(roomData, dt);

		if (roomData.boss)
		{
		
			Vector2 bossPrevPos = roomData.boss->sprite.position;

			roomData.boss->Update(player, dt);
			//std::cout << roomData.boss->sprite.current_animation_index << roomData.boss->sprite.current_sprite_index << '\n';
			//std::cout << roomData.boss->sprite.current_sprite_uv_offset_x << roomData.boss->sprite.current_sprite_uv_offset_y << '\n';

			roomData.boss->collideWall = false;

			// Test Player Collision with Map			
			int prevCellBoss = roomData.grid.WorldToCell(roomData.boss->sprite.position.x, roomData.boss->sprite.position.y);


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
				roomData.boss->shadow.position.y = bossPrevPos.y - roomData.boss->shadowOffset;
				roomData.boss->collideWall = true;
			}
			
			
			//gonna try to make it dynamic..
			/*
			float gridWidth = roomData.grid.GetTileWidth();
			float gridHeight = roomData.grid.GetTileHeight();
			if (CollisionBoundary_Static(roomData.boss->sprite.position, roomData.boss->sprite.scale, roomData.grid.GetBoundary().x * 0.99f, roomData.grid.GetBoundary().y * 0.99f)) {

				if (collisionResBoss & COLLISION_RIGHT && roomData.boss->bossStateMachine->currentState != BOSS_JUMP) {
					roomData.boss->sprite.position.x = roomData.grid.CellToWorldCenter(prevCellBoss).x + gridWidth * 0.5f - roomData.boss->sprite.scale.x * 0.5f - 0.10f;

					roomData.boss->collideWall = true;
				}
				if (collisionResBoss & COLLISION_LEFT && roomData.boss->bossStateMachine->currentState != BOSS_JUMP) {
					roomData.boss->sprite.position.x = roomData.grid.CellToWorldCenter(prevCellBoss).x - gridWidth * 0.5f + roomData.boss->sprite.scale.x * 0.5f + 0.10f;
					roomData.boss->collideWall = true;

				}
				if (collisionResBoss & COLLISION_BOTTOM && roomData.boss->bossStateMachine->currentState != BOSS_JUMP) {
					roomData.boss->sprite.position.y = roomData.grid.CellToWorldCenter(prevCellBoss).y - gridHeight * 0.5f + roomData.boss->sprite.scale.y * 0.5f + 0.10f;
					roomData.boss->collideWall = true;

				}
				if (collisionResBoss & COLLISION_TOP && roomData.boss->bossStateMachine->currentState != BOSS_JUMP) {
					roomData.boss->sprite.position.y = roomData.grid.CellToWorldCenter(prevCellBoss).y + gridHeight * 0.5f - roomData.boss->sprite.scale.y * 0.5f - 0.10f;
					roomData.boss->collideWall = true;

				}
			}*/
			std::cout << roomData.boss->collideWall << std::endl;

			//Code that sets the boss sprite to the direction its facing
			if (roomData.boss->direction.x < 0.f) 
				roomData.boss->sprite.scale = { -abs(roomData.boss->sprite.scale.x), roomData.boss->sprite.scale.y };
			else {
				roomData.boss->sprite.scale = { abs(roomData.boss->sprite.scale.x), roomData.boss->sprite.scale.y };
			}

			roomData.boss->sprite.UpdateTransform();
			roomData.boss->shadow.UpdateTransform();

			roomData.boss->hitbox.position = roomData.boss->sprite.position;
			roomData.boss->hitbox.UpdateTransform();
			//roomData.boss->hpBar.UpdateTransform();

			roomData.boss->sprite.UpdateAnimation(dt);
		}

		// Gifts and Enemy Check
		for (Gift*& gift : currentRoom->currentRoomData.giftList)
		{
			if (!(gift->velocity == Vector2()))
			{
				for (Enemy* e : currentRoom->currentRoomData.enemyList)
				{
					if (!e->isActive) continue;
					if (AreSquaresIntersecting(gift->giftType.sprite.position, gift->giftType.sprite.scale.x, e->sprite.position, e->sprite.scale.x))
					{
						Vector2 dirBtwnEnemyGift = e->sprite.position - gift->position;
						gift->velocity = -dirBtwnEnemyGift.Normalized() * gift->velocity.Length();
						Labels traitsCheck = gift->giftType.traits;
						//Include your friends in the traits check, because you can't friend those who judge yours
						for (Enemy* friendly : carryData.enemyList){
							traitsCheck.insert( friendly->type.traits.begin(), friendly->type.traits.end() );
						}

						e->AssessTraits(traitsCheck);
						
						//If the enemy becomes happy now, we shall delete the gift.
						if (e->state == ES_HAPPY) {
							delete gift;
							gift = nullptr;
							break;
						}

						// Ok, now gift shall snap to front of enemy while moving back
						gift->position = e->sprite.position - (dirBtwnEnemyGift).Normalized() * gift->giftType.sprite.scale.x;
					}
				}
			}
		}
		
		//erase any nullptrs
		std::vector<Gift*>& roomGiftListReference = currentRoom->currentRoomData.giftList;	
		roomGiftListReference.erase(
			std::remove(roomGiftListReference.begin(), roomGiftListReference.end(), nullptr),
			roomGiftListReference.end());
		

		// 4) Transfer enemies to our carrylist if they are essentially happy
		for (size_t i = 0; i < roomData.enemyList.size();)
		{
			Enemy* e = roomData.enemyList[i];
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
			Gift* g = roomData.giftList[i];
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
			Gift* g = carryData.giftList[i];
			if (g && g->pickUpState == false) // If currently picked up, need to check pickupstate = false and remove such????
			{
				roomData.giftList.push_back(g);
				carryData.giftList.erase(carryData.giftList.begin() + static_cast<long>(i)); // Remove from current carryData
				continue;
			}
			++i;
		}

		currentRoom->currentRoomData.particleSystem.UpdateParticles(dt);

		// Update game map
		Vector2 positionResetTest = player.position;
		gameMap.UpdateMap(player.position, playerHalfSize, currentRoom->currentRoomData.particleSystem, dt);
		/* IF player position is changed to other screen, reset enemies to player position */
		if (player.position != positionResetTest)
		{
			for (Enemy* enemy : gameMap.GetCurrentRoom()->currentRoomData.enemyList) {
				enemy->ChangeState(ES_NEUTRAL);
			}
			for (Enemy* e : carryData.enemyList)
			{
				e->sprite.position = player.position;
				e->roomData = &gameMap.GetCurrentRoom()->currentRoomData;
				// assess friend traits when entering a new room!

				for (Enemy* enemy : gameMap.GetCurrentRoom()->currentRoomData.enemyList) {
					
					enemy->AssessTraits(e->type.traits, false);
				}
			}
			for (Projectile* p : roomData.projectileList)
			{
				p->RemoveProjectile();
			}
		}

	}
	else if (gameState == PAUSED)
	{
		PauseAllAudio();
		// When paused:
		// - update only the pause UI
		if (AEInputCheckTriggered(AEVK_ESCAPE))
		{
			ResumeAllAudio();
			gameState = RUNNING;
		}

		pauseUi.Update();
	}
	else if (gameState == WIN)
	{
		if (!WinAudioPlaying) {
			StopAllAudio();
			GameWinAudio();
			WinAudioPlaying = true;
		}
		winUi.Update();
	}
	else if (gameState == LOSE)
	{
		if (!loseAudioPlaying) {		
			StopAllAudio();
			GameLoseAudio();
			loseAudioPlaying = true;
		}
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
		if (AEInputCheckTriggered(AEVK_R) || AEInputCheckTriggered(AEVK_5))
			ChangeState(GS_RESTART);

		player.speed *= 2;

		// to test damage
		if (AEInputCheckTriggered(AEVK_P))
			playerTakesDamage(player);
		if (AEInputCheckTriggered(AEVK_O))
			playerHealsDamage(player);

		if (AEInputCheckTriggered(AEVK_M))
			PlayerInit(player);
	}

	if (AEInputCheckTriggered(AEVK_F3) || AEInputCheckTriggered(AEVK_BACKQUOTE))
		debugMode = !debugMode;
}
