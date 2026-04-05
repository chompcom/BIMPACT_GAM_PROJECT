/* Start Header ************************************************************************/
/*!
\file		TestScene.cpp

\author     Yee Kiat Lim, yeekiat.lim, 2503993
\par        yeekiat.lim@digipen.edu

\author 	Hong Josiah Qin, hong.j, 2501239
\par  		hong.j@digipen.edu

\author		Lee Hng Hwee Celest, hnghweecelest.l, 2502385
\par		hnghweecelest.l@digipen.edu

\author     Brandon Choo, b.choo, 2501888
\par        b.choo@digipen.edu

\author		Ming Jun, m.quah, 2501182
\par		m.quah@digipen.edu

\brief		This scene is where the main game is played and the player is tested on their abilities.
The bulk of game's logic is located here.

Copyright (C) 2026 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited.
*/
/* End Header **************************************************************************/

#include "TestScene.h"

#include <algorithm>
#include <unordered_map>

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

AEGfxVertexList *sqMesh = nullptr;

AEGfxTexture *playerPng = nullptr;
AEGfxTexture *shadowPng = nullptr;
AEGfxTexture *hitboxPng = nullptr;
AEGfxTexture *heartPng = nullptr;
AEGfxTexture *almanacPng = nullptr;
AEGfxTexture *almanacLitUpPng = nullptr;
AEGfxTexture *pDoorTex = nullptr; // Door image
AEGfxTexture *arrowPng = nullptr;


std::vector<TexturedSprite> healthIcons;
TexturedSprite *almanacIcon = nullptr;
TexturedSprite *almanacLitUpIcon = nullptr;
TexturedSprite *arrowSprite = nullptr;
std::vector<TexturedSprite> gameOverButtons;

Sprite gameOverDarkScreen{};

Almanac almanac{};

s8 font = 0;

Player player{TexturedSprite(sqMesh, playerPng, Vector2(), Vector2(), Color{1, 1, 1, 1}), TexturedSprite(sqMesh, shadowPng, Vector2(), Vector2(), Color{1, 1, 1, 1}),  TexturedSprite(sqMesh, hitboxPng, Vector2(), Vector2(), Color{1, 1, 1, 1}), 2500.f, 100.f, Vector2(0, 0)};

mapRooms::Map gameMap; // Init var for map
static RoomData globalTransferData{};

LevelStates gameState;

static UIManager pauseUi;
static bool pauseUiInitialized = false;
static char const *pauseTipText = "[TIP]: Press `TAB` to resume";

static UIManager winUi;
static bool winUiInitialized = false;

static UIManager loseUi;
static bool loseUiInitialized = false;

static UIManager tutorialUi;

// Flags for my audio (brandon)
static bool fightMusicPlaying = false;
static bool loseAudioPlaying = false;
static bool winAudioPlaying = false;


// Confirmation window
static UIManager confirmUi;
static bool isConfirmPopupOpen = false;

enum class ConfirmAction
{
	None,
	RestartLevel,
	BackToMainMenu
};

static ConfirmAction pendingConfirmAction = ConfirmAction::None;


bool debugMode;

// WIN CONDITION STATISTICS
static std::unordered_map<std::string, int> winStatus{
	{"friends_made", 0},
	{"k_missed", 0},
	{"enemies_killed", 0}
};

static float levelElapsedSeconds = 0.0f;
static bool pendingWinStatusRefresh = false;

void TestLoad()
{
	DataLoader::Load();
	sqMesh = CreateSquareMesh();
	playerPng = AEGfxTextureLoad("Assets/player.png");
	shadowPng = AEGfxTextureLoad("Assets/shadow.png");
	hitboxPng = AEGfxTextureLoad("Assets/hitbox.png");
	heartPng = AEGfxTextureLoad("Assets/heart.png");
	almanacPng = AEGfxTextureLoad("Assets/almanac.png");
	almanacLitUpPng = AEGfxTextureLoad("Assets/almanacLitUp.png");
	arrowPng = AEGfxTextureLoad("Assets/arrow.png");
	gameOverDarkScreen = {sqMesh, Vector2(0.f, 0.f), Vector2(1600.f, 900.f), Color{0, 0, 0, 0.8f}};
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

	almanacIcon = new TexturedSprite(sqMesh, almanacPng, Vector2(640.f, -325.f), Vector2(128, 128), Color{1.0, 1.0, 1.0, 0.0});
	almanacLitUpIcon = new TexturedSprite(sqMesh, almanacLitUpPng, Vector2(640.f, -325.f), Vector2(128, 128), Color{1.0, 1.0, 1.0, 0.0});
	arrowSprite = new TexturedSprite(sqMesh, arrowPng, Vector2(645.f, -180.f), Vector2(128, 128), Color{1.0, 1.0, 1.0, 0.0});

	player.sprite = TexturedSprite(sqMesh, playerPng, Vector2(300, 300), Vector2(100, 100), Color{1, 1, 1, 0});
	player.shadow = TexturedSprite(sqMesh, shadowPng, Vector2(300, 255), Vector2(100, 100), Color{1, 1, 1, 0});
	player.hitbox = TexturedSprite(sqMesh, hitboxPng, Vector2(300, 300), Vector2(100, 100), Color{ 1, 1, 1, 0 });

	
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
	// RESTART STATE
	gameMap.DeleteMap();
	globalTransferData.enemyList.clear();
	globalTransferData.giftList.clear();
	globalTransferData.projectileList.clear();
	globalTransferData.player = nullptr;
	globalTransferData.particleSystem = nullptr;

	

	// INIT
	InitAudio();
	loseAudioPlaying = false;
	winAudioPlaying = false;
	fightMusicPlaying = false;
	PlayerInit(player);
	player.speed = 1.0f; // hope this works (I THINK THIS WORKS?)
	globalTransferData.player = &player;
	AlmanacInit(almanac);

	// WIN STATISTICS INIT
	winStatus["friends_made"] = 0;
	winStatus["k_missed"] = 0;
	winStatus["enemies_killed"] = 0;

	levelElapsedSeconds = 0.0f;
	pendingWinStatusRefresh = true;


	// Enable to allow for random values each run
	std::srand(static_cast<unsigned int>(std::time(nullptr))); // So based on number of seconds passed since Jan 1, 1970, this becomes our srand seed
	unsigned int curSeed = gameMap.RandInt(0, RAND_MAX - 1);
	gameMap.InitMap(globalTransferData, curSeed);

	player.position.x = player.position.y = 0;

	// Confirmation screen dialog
	confirmUi.LoadFromFilePopUp("Assets/UI/confirmation_popup.json", Vector2(0.0f, 0.0f), Vector2(560.0f, 240.0f));
	confirmUi.SetFont(font);


	confirmUi.BindOnClick("btn_yes", [](UIElement&)
		{
			// FAST CONFIRMATION ENUMS SETUP hah.
			switch (pendingConfirmAction)
			{
			case ConfirmAction::RestartLevel:
				ChangeState(GS_RESTART);
				break;

			case ConfirmAction::BackToMainMenu:
				ChangeState(GS_MAINMENU);
				break;

			default:
				break;
			}

			pendingConfirmAction = ConfirmAction::None;
			isConfirmPopupOpen = false;
		});

	confirmUi.BindOnClick("btn_no", [](UIElement&)
		{
			pendingConfirmAction = ConfirmAction::None;
			isConfirmPopupOpen = false;
		});

	// For pause screen;
	pauseUi.LoadFromFilePopUp("Assets/UI/pause_popup.json", Vector2(0.0f, 0.0f), Vector2(580.0f, 250.0f));
	
	pauseUi.BindOnClick("btn_restart", [](UIElement& self)
		{
			UNREFERENCED_PARAMETER(self);
			pendingConfirmAction = ConfirmAction::RestartLevel;
			isConfirmPopupOpen = true;

			UIElement* messageLabel = confirmUi.FindById("message");
			if (messageLabel) messageLabel->text = "Restart?";
		});
	pauseUi.BindOnClick("btn_mainmenu", [](UIElement& self)
		{
			UNREFERENCED_PARAMETER(self);
			pendingConfirmAction = ConfirmAction::BackToMainMenu;
			isConfirmPopupOpen = true;

			UIElement* messageLabel = confirmUi.FindById("message");
			if (messageLabel) messageLabel->text = "Return to menu?";
		});
	pauseUiInitialized = true;
	pauseUi.SetFont(font);

	winUi.LoadFromFilePopUp("Assets/UI/win_popup.json", Vector2(0.0f, 0.0f), Vector2(980.0f, 610.0f));

	winUi.BindOnClick("btn_restart", [](UIElement &self)
					  {
				UNREFERENCED_PARAMETER(self);
				pendingConfirmAction = ConfirmAction::RestartLevel;
				isConfirmPopupOpen = true;

				UIElement* messageLabel = confirmUi.FindById("message");
				if (messageLabel) messageLabel->text = "Restart?";
		});
	winUi.BindOnClick("btn_mainmenu", [](UIElement &self)
					  {
			UNREFERENCED_PARAMETER(self);
			pendingConfirmAction = ConfirmAction::BackToMainMenu;
			isConfirmPopupOpen = true;

			UIElement* messageLabel = confirmUi.FindById("message");
			if (messageLabel) messageLabel->text = "Return to menu?";
		});
	winUiInitialized = true;
	winUi.SetFont(font);

	loseUi.LoadFromFilePopUp("Assets/UI/lose_popup.json", Vector2(0.0f, 0.0f), Vector2(580.0f, 250.0f));
	loseUi.BindOnClick("btn_restart", [](UIElement &self)
		{
		UNREFERENCED_PARAMETER(self);
		pendingConfirmAction = ConfirmAction::RestartLevel;
		isConfirmPopupOpen = true;

		UIElement* messageLabel = confirmUi.FindById("message");
		if (messageLabel) messageLabel->text = "Restart?";
		});
	loseUi.BindOnClick("btn_mainmenu", [](UIElement &self)
		{
		UNREFERENCED_PARAMETER(self);
		pendingConfirmAction = ConfirmAction::BackToMainMenu;
		isConfirmPopupOpen = true;

		UIElement* messageLabel = confirmUi.FindById("message");
		if (messageLabel) messageLabel->text = "Return to menu?";
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
	

	gameMap.RenderCurrentRoom(DataLoader::GetMesh());


	AEGfxSetRenderMode(AE_GFX_RM_TEXTURE);

	// Draw objects: current-room objects + carried objects
	if (mapRooms::Room *room = gameMap.GetCurrentRoom())
	{
		RoomData &roomData = room->currentRoomData;
		RoomData &carryData = gameMap.GetTransferData();

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
				roomData.boss->sprite.RenderSprite(false, roomData.boss->sprite.currentSpriteUVOffsetX, roomData.boss->sprite.currentSpriteUVOffsetY);
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

	if (gameMap.GetCurrentRoom()->currentRoomData.boss) {
		if (gameMap.GetCurrentRoom()->currentRoomData.boss->healthbarInitialized) {
			gameMap.GetCurrentRoom()->currentRoomData.boss->healthbar.Draw();
		}
	}

	gameMap.RenderDebugMap(sqMesh); // Debug Map

	RenderPlayerLives(player, healthIcons, font);

	// Debug mode active
	if (debugMode)
	{
		// Render object hitboxes
		if (mapRooms::Room* room = gameMap.GetCurrentRoom())
		{
			RoomData& roomData = room->currentRoomData;
			RoomData& carryData = gameMap.GetTransferData();

			gameMap.GetCurrentRoom()->roomGrid.RenderGrid(DataLoader::GetMesh(), carryData.player->position, carryData.player->sprite.scale, AE_GFX_RM_TEXTURE);

			for (Gift* g : roomData.giftList)
			{
				if (g)
				{
					g->hitbox.RenderSprite();
				}
			}
			for (Enemy* e : roomData.enemyList)
			{
				if (e && e->isActive)
				{
					e->hitbox.RenderSprite();
				}
			}
			if (roomData.boss)
			{
				if (roomData.boss->currentHealth > 0)
				{
					roomData.boss->sprite.RenderSprite(false, roomData.boss->sprite.currentSpriteUVOffsetX, roomData.boss->sprite.currentSpriteUVOffsetY);
					roomData.boss->hitbox.RenderSprite();
				}
			}

			for (Gift* g : carryData.giftList)
			{
				if (g)
				{
					g->hitbox.RenderSprite();
				}
			}
			for (Enemy* e : carryData.enemyList)
			{
				if (e)
				{
					e->hitbox.RenderSprite();
				}
			}
		}

		player.hitbox.RenderSprite();

		// Display debug info
		char buffer[50];

		sprintf_s(buffer, 50, "DEBUG MODE ON");
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

	}

	RenderAlmanacIcon(almanac, *almanacIcon, *almanacLitUpIcon, *arrowSprite, player);

	RenderAlmanacPages(almanac, font);

	RenderTutorial();

	// Pause screen

	if (isConfirmPopupOpen) {
		AEGfxSetRenderMode(AE_GFX_RM_COLOR);
		AEGfxSetBlendMode(AE_GFX_BM_BLEND);
		AEGfxSetTransparency(1.0f);

		Sprite overlay(
			DataLoader::GetOrCreateSquareMesh(),
			Vector2(0.0f, 0.0f),
			Vector2(
				AEGfxGetWinMaxX() - AEGfxGetWinMinX(),
				AEGfxGetWinMaxY() - AEGfxGetWinMinY()
			),
			Color{ 0.0f, 0.0f, 0.0f, 0.55f }
		);
		overlay.RenderSprite(true);

		confirmUi.Draw();
	}

	else if (gameState == PAUSED)
	{
		AEGfxSetRenderMode(AE_GFX_RM_COLOR);
		AEGfxSetBlendMode(AE_GFX_BM_BLEND);

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
	AEGfxMeshFree(sqMesh);

	//#warning "there are icons being deleted, are we not using dataloader?"	
	delete almanacIcon;
	delete arrowSprite;
	delete almanacLitUpIcon;



	AEGfxTextureUnload(playerPng);
	AEGfxTextureUnload(shadowPng);
	AEGfxTextureUnload(hitboxPng);


	AEGfxTextureUnload(heartPng);
	AEGfxTextureUnload(almanacPng);
	AEGfxTextureUnload(almanacLitUpPng);
	AEGfxTextureUnload(arrowPng);

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

	allGiftTypes.clear();

	TutorialUnload();
}

void TestUpdate(float dt)
{

	levelElapsedSeconds += dt;

	if (gameState == RUNNING)
	{
		// Pause toggle
		if (AEInputCheckTriggered(AEVK_ESCAPE) || AEInputCheckTriggered(AEVK_TAB))
		{
			gameState = PAUSED;
		}

		// Get previous pos
		Vector2 prevPos{ player.position.x, player.position.y };


		//this has to be above checkIfAlmanacClicked or the arrows will bug
		AlmanacInputs(almanac);
		CheckIfAlmanacClicked(*almanacIcon, almanac);
		TutorialInput();
		// for now,
		//  Player update
		UpdatePlayer(player, dt);


		UpdateMobAudioCD(dt); // this is for the mob audio cooldown so it doesnt spam the mob sounds

		Vector2 playerHalfSize = player.sprite.scale * 0.5f;

		// Game map update
		gameMap.GetCurrentRoom()->Update(dt);

		mapRooms::Room* currentRoom = gameMap.GetCurrentRoom();
		RoomData& roomData = currentRoom->currentRoomData;
		RoomData& carryData = gameMap.GetTransferData();


		//std::cout << currentRoom->biome << std::endl;

		// Biome audio for the specific biome in the map
		static mapRooms::Room* lastRoom = nullptr;
		if (currentRoom != lastRoom && !almanac.isOpen)
		{
			lastRoom = currentRoom;
			fightMusicPlaying = false;  // if fight music is playing and u go out of the room it stops 
			ResetBGM();
			if (currentRoom->biome == "Green") ForestBiomeAudio();
			if (currentRoom->biome == "Ice") IceBiomeAudio();
			if (currentRoom->biome == "Ocean") OceanBiomeAudio();
			if (roomData.boss) BossBGMAudio();
		}

		bool isAngry = false; // check whether any mob is angry in the room
		for (Enemy* e : roomData.enemyList) {
			if (e && e->isActive && e->state == ES_ANGRY) {
				isAngry = true;
				break;
			}
		}

		if (!almanac.isOpen && !roomData.boss)
		{
			if (isAngry && !fightMusicPlaying) {  // whem there is an angry mob fight music will play
				ResetBGM();
				FightMusicAudio(); 
				fightMusicPlaying = true;
			}
			else if (!isAngry && fightMusicPlaying) {
				ResetBGM();
				fightMusicPlaying = false;
			}
			if (!fightMusicPlaying) RandomBGMAudio(dt); // if no fight music play the 3 random BGM sound 
		}
		// FOR DOOR
		int prevCell = currentRoom->roomGrid.WorldToCell(prevPos.x, prevPos.y);
		if (prevCell < 0 && currentRoom->lastValidCell >= 0) prevCell = currentRoom->lastValidCell;

		// TEST PLAYER COLLISION WITH MAP
		Vector2 moveDelta = player.position - prevPos;
		Vector2 moveDir = moveDelta.Normalized();
		int curCell = currentRoom->roomGrid.WorldToCell(player.position.x, player.position.y);
		if (curCell >= 0 && curCell != 0xffffff) currentRoom->lastValidCell = curCell;

		float collisionScaleX = player.sprite.scale.x * 0.8f;
		float collisionScaleY = player.sprite.scale.y * 0.8f;

		int colRes = currentRoom->roomGrid.CheckMapGridCollision(player.position.x, player.position.y, collisionScaleX, collisionScaleY, prevCell);

		float gridWidth = currentRoom->roomGrid.GetTileWidth();
		float gridHeight = currentRoom->roomGrid.GetTileHeight();

		constexpr float skinBoss{ 0.10f };

		if (prevCell >= 0)
		{
			Vector2 prevCellCenter = currentRoom->roomGrid.CellToWorldCenter(prevCell);
			if ((colRes & COLLISION_LEFT) &&	moveDir.x < - EPSILON)	player.position.x = prevCellCenter.x - gridWidth * 0.5f + collisionScaleX * 0.5f +  skinBoss;
			if ((colRes & COLLISION_RIGHT) &&	moveDir.x >   EPSILON)	player.position.x = prevCellCenter.x + gridWidth * 0.5f - collisionScaleX * 0.5f -  skinBoss;
			if ((colRes & COLLISION_TOP) &&		moveDir.y >   EPSILON)	player.position.y = prevCellCenter.y + gridHeight * 0.5f - collisionScaleY * 0.5f - skinBoss;
			if ((colRes & COLLISION_BOTTOM) &&	moveDir.y < - EPSILON)	player.position.y = prevCellCenter.y - gridHeight * 0.5f + collisionScaleY * 0.5f + skinBoss;
		}

		// sync sprite + shadow AFTER collision correction
		player.sprite.position = player.position;
		player.shadow.position = player.position - Vector2{ 0, 40 };
		player.hitbox.position = player.position;

		player.sprite.UpdateTransform();
		player.shadow.UpdateTransform();
		player.hitbox.UpdateTransform();


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
						CollisionIntersectionRectRect(e->sprite.position, e->sprite.scale, e->velocity, b->sprite.position, b->sprite.scale, b->velocity, ti)
						)
					{
						Vector2 dir = b->sprite.position - (e->sprite.position + Vector2(AERandFloat(), AERandFloat()));
						e->sprite.position -= dir.Normalized() * b->sprite.scale.x * 0.5f * dt;
						b->sprite.position += dir.Normalized() * b->sprite.scale.x * 0.5f * dt;
					}
				}

				e->hitbox.position = e->sprite.position;
				e->hitbox.UpdateTransform();


			}
		}


		for (Gift* g : roomData.giftList)
		{
			if (g)
			{
				Vector2 prevGiftPos = g->position;
				prevCell = currentRoom->roomGrid.WorldToCell(prevGiftPos.x, prevGiftPos.y);
				constexpr float skin = 0.10f;

				UpdateGift(*g, player, dt, currentRoom->roomGrid.GetBoundary() * 0.95f);

				Vector2 moveDeltaGift = g->position - prevGiftPos;
				Vector2 moveDirGift	  = moveDeltaGift.Normalized();

				float speed = sqrtf(g->velocity.x * g->velocity.x + g->velocity.y * g->velocity.y);

				// Increase scale as speed goes up hacky method
				float collisionScaleXGift = AEClamp(speed / 2000.0f * g->giftType.sprite.scale.x, g->giftType.sprite.scale.x, g->giftType.sprite.scale.x * 2.0f);
				float collisionScaleYGift = AEClamp(speed / 2000.0f * g->giftType.sprite.scale.y, g->giftType.sprite.scale.y, g->giftType.sprite.scale.y * 2.0f);
				int res = currentRoom->roomGrid.CheckMapGridCollision(g->position.x,g->position.y, collisionScaleXGift, collisionScaleYGift, prevCell);

				bool hitX = (res & COLLISION_LEFT) || (res & COLLISION_RIGHT);
				bool hitY = (res & COLLISION_TOP) || (res & COLLISION_BOTTOM);

				if (res && prevCell >= 0)
				{
					Vector2 prevCellCenter = currentRoom->roomGrid.CellToWorldCenter(prevCell);

					float rubberBand = 0.1f;
			
					// JOSIAH'S inspiration from enemies method (0.1 instead of 0.01 to prevent rubberband?)
					if ((res & COLLISION_LEFT)   &&	moveDirGift.x <  -rubberBand)  g->position.x = prevCellCenter.x - gridWidth * 0.5f +  collisionScaleXGift * 0.5f + skin;
					if ((res & COLLISION_RIGHT)  &&	moveDirGift.x >  rubberBand)  g->position.x = prevCellCenter.x + gridWidth * 0.5f -  collisionScaleXGift * 0.5f - skin;
					if ((res & COLLISION_BOTTOM) && moveDirGift.y < -rubberBand) g->position.y = prevCellCenter.y - gridHeight * 0.5f + collisionScaleYGift * 0.5f + skin;
					if ((res & COLLISION_TOP)	 &&	moveDirGift.y >    rubberBand)  g->position.y = prevCellCenter.y + gridHeight * 0.5f - collisionScaleYGift * 0.5f - skin;

					// If basically stopped, try prevent rubberbanding for long?
					if (moveDeltaGift.LengthSq() < (rubberBand*rubberBand))
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
				UpdateGift(*g, player, dt, Vector2{AEGfxGetWindowWidth(), AEGfxGetWindowHeight()});
				g->giftType.sprite.UpdateTransform();
				g->shadow.UpdateTransform();
				g->hitbox.UpdateTransform();
			}
		}

		// Check for projectile collision between player and the roomdata and uodates it
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
			int bossColRes = gameMap.GetCurrentRoom()->roomGrid.CheckMapGridCollision(roomData.boss->sprite.position.x, roomData.boss->sprite.position.y, roomData.boss->sprite.scale.x * 0.9f, roomData.boss->sprite.scale.y * 0.9f, -2);
			if ((bossColRes & COLLISION_LEFT || bossColRes & COLLISION_RIGHT) && roomData.boss->bossStateMachine->currentState != BOSS_JUMP) {
				roomData.boss->sprite.position.x = bossPrevPos.x; // Test for x collision
				roomData.boss->shadow.position.x = bossPrevPos.x;
				//roomData.boss->sprite.position.x = AEClamp(bossPrevPos.x, -650 + roomData.boss->sprite.scale.y / 2, 650 - roomData.boss->sprite.scale.y / 2);//bossPrevPos.x; // Test for x collision
				//roomData.boss->shadow.position.x = AEClamp(bossPrevPos.x, -650 + roomData.boss->sprite.scale.y / 2, 650 - roomData.boss->sprite.scale.y / 2);//bossPrevPos.x;
				roomData.boss->collideWall = true;
			}
			if ((bossColRes & COLLISION_TOP || bossColRes & COLLISION_BOTTOM) && roomData.boss->bossStateMachine->currentState != BOSS_JUMP) {
				roomData.boss->sprite.position.y = bossPrevPos.y; // Test for y collision
				roomData.boss->shadow.position.y = bossPrevPos.y - roomData.boss->shadowOffset;
				//roomData.boss->sprite.position.y = AEClamp(bossPrevPos.y, -350 + roomData.boss->sprite.scale.y / 2, 350 - roomData.boss->sprite.scale.y / 2);//bossPrevPos.y; // Test for y collision
				//roomData.boss->shadow.position.y = AEClamp(bossPrevPos.y, -350 + roomData.boss->sprite.scale.y / 2, 350 - roomData.boss->sprite.scale.y / 2) - roomData.boss->shadowOffset;//bossPrevPos.y - roomData.boss->shadowOffset;
				roomData.boss->collideWall = true;
			}
			
			

			//Code that sets the boss sprite to the direction its facing
			if (roomData.boss->direction.x < 0.f) 
				roomData.boss->sprite.scale = { -abs(roomData.boss->sprite.scale.x), roomData.boss->sprite.scale.y };
			else {
				roomData.boss->sprite.scale = { abs(roomData.boss->sprite.scale.x), roomData.boss->sprite.scale.y };
			}

			roomData.boss->sprite.UpdateTransform();
			roomData.boss->shadow.UpdateTransform();

			roomData.boss->hitbox.position = Vector2{ roomData.boss->sprite.position.x, roomData.boss->sprite.position.y - roomData.boss->shadowOffset + roomData.boss->hitbox.scale.y / 2};
			roomData.boss->hitbox.UpdateTransform();

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

		if (isConfirmPopupOpen)
		{
			if (AEInputCheckTriggered(AEVK_ESCAPE))
			{
				pendingConfirmAction = ConfirmAction::None;
				isConfirmPopupOpen = false;
			}

			confirmUi.Update();
		}
		else
		{
			if (AEInputCheckTriggered(AEVK_ESCAPE))
			{
				ResumeAllAudio();
				gameState = RUNNING;
			}

			pauseUi.Update();
		}

	}
	else if (gameState == WIN)
	{
		if (!winAudioPlaying)
		{
			StopAllAudio();
			GameWinAudio();
			winAudioPlaying = true;
		}

		if (pendingWinStatusRefresh)
		{
			RoomData& carryData = gameMap.GetTransferData();

			winStatus["friends_made"] = static_cast<int>(carryData.enemyList.size());
			winStatus["k_missed"] = 0;
			winStatus["enemies_killed"] = 0;

			int mapGridSize = gameMap.GetGridSize();

			for (int row = 0; row < mapGridSize; ++row)
			{
				for (int column = 0; column < mapGridSize; ++column)
				{
					mapRooms::Room* room = gameMap.GetRoom(column, row);
					if (!room) continue;

					for (Enemy* enemy : room->currentRoomData.enemyList)
					{
						if (!enemy) continue;

						if (enemy->isActive) ++winStatus["k_missed"];
						else ++winStatus["enemies_killed"];
					}
				}
			}
			

			UIElement* friendsMadeLabel = winUi.FindById("label_friends_made");
			if (friendsMadeLabel)
			{

				std::stringstream buf;
				buf << "Friends made: " << winStatus["friends_made"];
				friendsMadeLabel->text = buf.str();
			}

			UIElement* missedLabel = winUi.FindById("label_k_missed");
			if (missedLabel)
			{
				std::stringstream buf;
				buf << "Friends missed: " << winStatus["k_missed"];
				//sprintf_s(buffer, sizeof(buffer), "k missed: %d", );
				missedLabel->text = buf.str();
			}

			UIElement* enemiesKilledLabel = winUi.FindById("label_enemies_killed");
			if (enemiesKilledLabel)
			{
				std::stringstream buf;
				buf << "Enemies Killed: " << winStatus["enemies_killed"];

				enemiesKilledLabel->text = buf.str();
			}

			int totalSeconds = static_cast<int>(levelElapsedSeconds + 0.5f);
			int minutes = totalSeconds / 60;
			int seconds = totalSeconds % 60;

			UIElement* timeTakenLabel = winUi.FindById("label_time_taken");
			if (timeTakenLabel)
			{
				char buffer[128]{};
				sprintf_s(buffer, sizeof(buffer), "YOU TOOK %02d:%02d!!!", minutes, seconds);
				timeTakenLabel->text = buffer;
			}

			pendingWinStatusRefresh = false;
		}

		
		// Check if popup open else do win ui
		if (isConfirmPopupOpen)
		{
			if (AEInputCheckTriggered(AEVK_ESCAPE))
			{
				pendingConfirmAction = ConfirmAction::None;
				isConfirmPopupOpen = false;
			}

			confirmUi.Update();
		}
		else
		{
			if (AEInputCheckTriggered(AEVK_ESCAPE))
			{
				ResumeAllAudio();
				gameState = RUNNING;
			}

			winUi.Update();
		}

	}
	else if (gameState == LOSE)
	{


		// Check if popup open else do win ui
		if (isConfirmPopupOpen)
		{
			if (AEInputCheckTriggered(AEVK_ESCAPE))
			{
				pendingConfirmAction = ConfirmAction::None;
				isConfirmPopupOpen = false;
			}

			confirmUi.Update();
		}
		else
		{


			if (!loseAudioPlaying) {
				StopAllAudio();
				GameLoseAudio();
				loseAudioPlaying = true;
			}
			loseUi.Update();
		}

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
		if (AEInputCheckTriggered(AEVK_R) || AEInputCheckTriggered(AEVK_5)) {
			StopAllAudio();
			ChangeState(GS_RESTART);
		}
		player.speed *= 2;

		// to test damage
		if (AEInputCheckTriggered(AEVK_P))
			PlayerTakesDamage(player);
		if (AEInputCheckTriggered(AEVK_O))
			PlayerHealsDamage(player);

		if (AEInputCheckTriggered(AEVK_M))
			PlayerInit(player);
	}

	if (AEInputCheckTriggered(AEVK_F3) || AEInputCheckTriggered(AEVK_BACKQUOTE))
		debugMode = !debugMode;
}
