#include "rooms.h"
#include <random>
#include <cstdlib>
#include "AEEngine.h"

#include <algorithm>	// sort
#include <cctype>		// tolower
#include <Windows.h>	// Windows

#include "Sprite.h"		// TexturedSpirte for rendering room bg
#include "Enemy.h"
#include "Loaders/DataLoader.h"
#include "Collision.h"

namespace Config {
	// We are making an n x n grid with 1 and 0s
	static const int minGrid = 2, maxGrid = 5;

	// Legacy File System Scanning
	void ScanPngFolderWin32(std::string const& folder, std::vector<std::string>& outList)
	{
		outList.clear();	// Clear Lists

		// Pattern Example: Assets\Rooms\Normal\*.png
		std::string pattern = folder;
		if (!pattern.empty() && pattern.back() != '\\' && pattern.back() != '/')
			pattern += "\\";
		pattern += "*.png";

		// Idk how this works truly winapi stuff
		WIN32_FIND_DATAA data{};
		HANDLE hFind = FindFirstFileA(pattern.c_str(), &data);
		if (hFind == INVALID_HANDLE_VALUE)
			return;

		do
		{
			if (data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
				continue;

			// Full path to file
			std::string fullPath = folder;
			if (!fullPath.empty() && fullPath.back() != '\\' && fullPath.back() != '/')
				fullPath += "\\";
			fullPath += data.cFileName;

			outList.push_back(fullPath);	// Accumulate Files
		} while (FindNextFileA(hFind, &data));

		FindClose(hFind);

		std::sort(outList.begin(), outList.end());
	}


	// Absolute
	static float AbsF(float v) { return (v < 0.0f) ? -v : v; }

	// Overlap Algo rectangle
	static bool OverlapAabb(Vector2 aCenter, Vector2 aHalf, Vector2 bCenter, Vector2 bHalf)
	{
		float dx = AbsF(aCenter.x - bCenter.x);
		float dy = AbsF(aCenter.y - bCenter.y);
		return (dx <= (aHalf.x + bHalf.x)) && (dy <= (aHalf.y + bHalf.y));
	}



}



EnemyType somethingelse{"rock",100,10,{"sad"},{"happy"},{"sad"}};

namespace mapRooms
{
    Room::Room(RoomType roomType) :
        left{ nullptr }, right{ nullptr }, up{ nullptr }, down{ nullptr },
        rmType{ roomType }
		,toBeTransferred{nullptr},currentRoomData{}
    {}
	Room::~Room() {

		for (Enemy* i : currentRoomData.enemyList){
			delete i;
			i = nullptr;
		}
		currentRoomData.enemyList.clear();
	}

	void Room::Init() {
		rmType = RoomType::Normal;
		somethingelse.neutral = WalkLeft;
		somethingelse.happy = WalkToTarget;
		somethingelse.angry = WalkRight;
		currentRoomData.enemyList.push_back(new Enemy(somethingelse, DataLoader::CreateTexture("Assets/poprocks.png")));
		for (Enemy* i : currentRoomData.enemyList){
			i->ChangeState(EnemyStates::ES_NEUTRAL);
		}
	} 
	void Room::Update(float dt) {
		for (Enemy* i : currentRoomData.enemyList){
			i->Update(dt);
		}

	}



	Map::Map() :
		gridSize{ 0 },
		rooms{},
		currentRoom{ nullptr },
		rngState{ 0xA341316Cu }	// Some random seed
	{
		//Room x[2]{};
		//for (Room y : x) {
			//rooms.push_back(y);
		//}
	}

	Map::~Map(){
		DeleteMap();
	}

	void Map::LoadRoomArtLists()
	{
		normalRoomFiles.clear();
		bossRoomFiles.clear();

		std::string const normalDir = "Assets/Rooms/Normal";
		std::string const bossDir = "Assets/Rooms/Boss";

		Config::ScanPngFolderWin32(normalDir, normalRoomFiles);
		Config::ScanPngFolderWin32(bossDir,   bossRoomFiles);


	}

	AEGfxTexture* Map::GetOrLoadTexture(std::string const& path)
	{
		// If already loaded, reuse it
		auto it = textureCache.find(path);
		if (it != textureCache.end()) return it->second;

		// Load new texture if not found ig
		AEGfxTexture* tex = AEGfxTextureLoad(path.c_str());
		textureCache[path] = tex; // cache even if nullptr (prevents repeated attempts)
		return tex;
	}

	// Asset Code was here
	void Map::AssignRoomArt() {
		bool haveNormal = !normalRoomFiles.empty();
		bool haveBoss = !bossRoomFiles.empty();

		for (int y = 0; y < gridSize; ++y)
		{
			for (int x = 0; x < gridSize; ++x)
			{
				Room* rm = GetRoom(x, y);
				// Skip empty rooms lol
				if (!rm || rm->rmType == RoomType::Empty)
					continue;

				std::string chosenPath;

				if (rm->rmType == RoomType::Boss)
				{
					if (haveBoss) {
						int pick = static_cast<int>(RandInt(0, static_cast<int>(bossRoomFiles.size()) - 1));
						chosenPath = bossRoomFiles[pick];
					}
					else continue;
				}
				else if (rm->rmType == RoomType::Normal || rm->rmType == RoomType::Start) {
					if (haveNormal) {
						int pick = static_cast<int>(RandInt(0, static_cast<int>(bossRoomFiles.size()) - 1));
						chosenPath = normalRoomFiles[pick];

					}


				}

				rm->roomTexturePath = chosenPath;
				rm->roomTexture = (chosenPath.empty() ? nullptr : GetOrLoadTexture(chosenPath));

			}
		}
	}
		
	
	//InitMap(): Allocate / Reset Maps (Rooms)
	void Map::InitMap(unsigned int seed) {
		rngState = seed;
		srand(rngState);
		gridSize = RandInt(Config::minGrid, Config::maxGrid);

		rooms.clear();
		rooms.resize(gridSize * gridSize);	// Grid generated
		
		ResetRooms();						// Ensure rooms are nothing;
		GenerateRooms();
		// Probably generate other room types???

		// Load available Pngs
		LoadRoomArtLists();
		AssignRoomArt();

		// Somehow assign startX and startY into starting room coordinates value
		currentRoom = GetRoom(startX, startY);

		doorCooldown = 0.0f;
	}

	bool Map::InBounds(int x, int y) const
	{
		return (x >= 0 && x < gridSize && y >= 0 && y < gridSize);
	}

	void Map::ResetRooms() {
		for (int i = 0; i < gridSize * gridSize; ++i) {
			rooms[i] = Room(RoomType::Empty);
		}
	}

	int Map::GetRoomIdx(int x, int y) const
	{
		return ((y * gridSize) + x);
	}

	unsigned int Map::RandInt(int low, int high) {
		return (static_cast<unsigned int>(low + (std::rand() % (high - low + 1))));
	}

	//// UpdateMap()
	//void Map::UpdateMap(  )
	//{
	//	// Input (Get x and y of player)
	//	// Perhaps testing for collision
	//	

	//	MoveTo(Direction::Left);	// If collide with door area, use MoveTo(Direction direcci�n)
	//}

	// DeleteMap()?
	void Map::DeleteMap() 
	{
		rooms.clear();			// Clear room information
		gridSize = 0;			// Set gridsize to zero
		currentRoom = nullptr;	// Deference room item
		doorCooldown = 0.0f;	// Cooldown thingy

		for (auto& kv : textureCache) {
			if (kv.second) AEGfxTextureUnload(kv.second);
		}
		textureCache.clear();
		normalRoomFiles.clear();
		bossRoomFiles.clear();
	}

	void Map::LinkRooms(Room* a, Room* b, Direction dirFromAToB)
	{
		if (dirFromAToB == Direction::Right) { a->right = b; b->left = a; }
		if (dirFromAToB == Direction::Left) { a->left = b; b->right = a; }
		if (dirFromAToB == Direction::Up) { a->up = b; b->down = a; }
		if (dirFromAToB == Direction::Down) { a->down = b; b->up = a; }
	}

	 //OLD Generate Rooms (likely working)
	//void Map::GenerateRooms()
	//{
	//	// STEP 1: FIXED STARTING POINT (x=0, y=0)
	//	startX = 0;
	//	startY = 0;
	//	
	//	//int minDist = gridSize - 1; 
	//	int minDist = startX + 1;	// Should clamp this but we ball
	//	// STEP 2: Get End
	//	do
	//	{
	//		bossX = RandInt(0, gridSize - 1);
	//		bossY = RandInt(0, gridSize - 1);
	//		int dist = std::abs(bossX - startX) + std::abs(bossY - startY);
	//		if (bossX == startX && bossY == startY) continue;
	//		if (dist < minDist) continue;
	//		break;
	//	} while (true);
	//	// 3) Mark start/boss types
	//	GetRoom(startX, startY)->rmType = RoomType::Start;
	//	GetRoom(bossX, bossY)->rmType = RoomType::Boss;
	//	// 4) Carve a guaranteed path start -> boss
	//	int x = startX;
	//	int y = startY;
	//	while (x != bossX || y != bossY)
	//	{
	//		bool needX = (x != bossX);
	//		bool needY = (y != bossY);
	//		// choose whether to move in x or y if both are possible
	//		bool moveX = false;
	//		if (needX && needY) moveX = (RandInt(0, 1) == 0);
	//		else moveX = needX;
	//		int nx = x;
	//		int ny = y;
	//		if (moveX)
	//			nx += (bossX > x) ? 1 : -1;   // step toward boss in x
	//		else
	//			ny += (bossY > y) ? 1 : -1;   // step toward boss in y
	//		Room* a = GetRoom(x, y);
	//		Room* b = GetRoom(nx, ny);
	//		// Link pointers both ways (no bitmask needed)
	//		if (nx == x + 1) { a->right = b; b->left = a; }
	//		if (nx == x - 1) { a->left = b; b->right = a; }
	//		if (ny == y + 1) { a->down = b; b->up = a; }
	//		if (ny == y - 1) { a->up = b; b->down = a; }
	//		// Mark intermediate rooms as Normal (don�t overwrite Start/Boss)
	//		if (b->rmType == RoomType::Normal)
	//		{
	//			// already normal; ok
	//		}
	//		else if (b->rmType != RoomType::Boss)
	//		{
	//			b->rmType = RoomType::Normal;
	//		}
	//		x = nx;
	//		y = ny;
	//	}
	//}

	// Simple generation using random pathing. Alternative would be randomized DFS. Computationally intensive however.
	// https://en.wikipedia.org/wiki/File:Depth-First_Search_Animation.ogv explains how DFS works
	void Map::GenerateRooms() {
		int roomsVisit = RandInt(2, gridSize * gridSize);	// Set How many rooms to make available
		
		startX =		RandInt(0, gridSize - 1), startY = RandInt(0, gridSize - 1);
		int startIdx =	GetRoomIdx(startX, startY);

		// DFS Tracking for paths
		std::vector<bool> visited(gridSize * gridSize, false); // Init array of n x n size to false
		std::vector<int> stack, visitedIndices;


		visited[startIdx] = true;	// Mark start as visited
		int visitedCount = 1;
		stack.push_back(startIdx);
		visitedIndices.push_back(startIdx);
		GetRoom(startX, startY)->rmType = RoomType::Start;

		// Random DFS until roomVisits reached or out of expansion
		while (!stack.empty() && visitedCount < roomsVisit) {
			int currentIdx = stack.back();
			int curX = currentIdx % gridSize, curY = currentIdx / gridSize;

			int neighbourIdx[4]{}, neighbourX[4]{}, neighbourY[4]{}, neighCount{};

			// Go Left
			if (InBounds(curX - 1, curY))
			{
				int idxRoom = GetRoomIdx(curX - 1, curY);	
				if (!visited[idxRoom]) {
					neighbourIdx[neighCount] = idxRoom;
					neighbourX[neighCount] = -1;
					neighbourY[neighCount] = 0;
					++neighCount;
				}
			}

			// Go Right
			if (InBounds(curX + 1, curY))
			{
				int idxRoom = GetRoomIdx(curX + 1, curY);
				if (!visited[idxRoom]) {
					neighbourIdx[neighCount] = idxRoom;
					neighbourX[neighCount] = 1;
					neighbourY[neighCount] = 0;
					++neighCount;
				}
			}

			// Go Up
			if (InBounds(curX, curY - 1))
			{
				int idxRoom = GetRoomIdx(curX, curY - 1);
				if (!visited[idxRoom]) {
					neighbourIdx[neighCount] = idxRoom;
					neighbourX[neighCount] = 0;
					neighbourY[neighCount] = -1;
					++neighCount;
				}
			}

			// Go Down
			if (InBounds(curX, curY + 1))
			{
				int idxRoom = GetRoomIdx(curX, curY + 1);
				if (!visited[idxRoom]) {
					neighbourIdx[neighCount] = idxRoom;
					neighbourX[neighCount] = 0;
					neighbourY[neighCount] = 1;
					++neighCount;
				}
			}


			// No neighbours somehow
			if (neighCount == 0) {
				stack.pop_back();
				continue;
			}

			// Randomly pick 1 neighbour for next iteration
			int pick = RandInt(0, neighCount - 1);
			int nextIdx = neighbourIdx[pick];
			int nextX = nextIdx % gridSize, nextY = nextIdx / gridSize;
			
			Room* curRoom  = GetRoom(curX, curY);
			Room* nextRoom = GetRoom(nextX, nextY);

			// Link current room to next Room
			Direction linkDirection = Direction::Left;

			if (neighbourX[pick] == -1) {
				linkDirection = Direction::Left;
			}
			else if (neighbourX[pick] == 1) {
				linkDirection = Direction::Right;
			}
			else if (neighbourY[pick] == -1) {
				linkDirection = Direction::Up;
			}
			else if (neighbourY[pick] == 1) {
				linkDirection = Direction::Down;
			}

			// Link Rooms
			LinkRooms(curRoom, nextRoom, linkDirection);
			

			// Mark Visited
			visited[nextIdx] = true;
			stack.push_back(nextIdx);			// Track Depth in neighbours
			visitedIndices.push_back(nextIdx);	// Track visited indexes
			++visitedCount;

			// Mark next room for exist
			if (nextRoom != nullptr) {
				nextRoom->Init();
			}



		}

		
		// End Boss for Level
		startIdx = GetRoomIdx(startX, startY);
		std::vector<int> deadEnds;
		deadEnds.reserve(visitedIndices.size());

		for (int idx : visitedIndices) {
			if (idx == startIdx) continue;

			Room const& r = rooms[idx];
			int links = 0;
			if (r.left!=nullptr) ++links;
			if (r.right != nullptr) ++links;
			if (r.up != nullptr) ++links;
			if (r.down!= nullptr) ++links;

			if (links == 1) deadEnds.push_back(idx);
		}

		int bossIdx{startIdx};

		if (!deadEnds.empty()) {
			bossIdx = deadEnds.at(RandInt(0, static_cast<int>(deadEnds.size() - 1)));
		}
		else if (visitedIndices.size() > 1) {	// No dead ends
			bossIdx = visitedIndices.back();
			if (bossIdx == startIdx) bossIdx = visitedIndices.at(1);
		}

		// Mark Boss
		GetRoom(bossIdx% gridSize, bossIdx / gridSize)->rmType = RoomType::Boss;
	}

	int Map::GetGridSize() const {
		return gridSize;
	}

	void Map::RenderCurrentRoom(AEGfxVertexList* squaremesh) const {
		if (!currentRoom || !squaremesh || !currentRoom->roomTexture) return;	// possibly skip empty textures?

		float minX = AEGfxGetWinMinX();
		float maxX = AEGfxGetWinMaxX();
		float minY = AEGfxGetWinMinY();
		float maxY = AEGfxGetWinMaxY();

		float winW = (maxX - minX);
		float winH = (maxY - minY);

		AEGfxSetRenderMode(AE_GFX_RM_TEXTURE);
		AEGfxSetBlendMode(AEGfxBlendMode::AE_GFX_BM_BLEND);
		AEGfxSetTransparency(1.0f);

		TexturedSprite bg(
			squaremesh,
			currentRoom->roomTexture,
			Vector2{ 0.0f, 0.0f },                  // centered
			Vector2{ winW, winH },                  // fill screen
			Color{ 1.0f, 1.0f, 1.0f, 1.0f }          // no tint
		);

		bg.RenderSprite();
		for (Enemy* i : currentRoom->currentRoomData.enemyList)
			i->sprite.RenderSprite();

	}

	// FOR DEBUGGING PURPOSE
	void Map::RenderDebugMap(AEGfxVertexList* squareMesh) const
	{
		if (!squareMesh) return;
		if (gridSize <= 0) return;

		float minX = AEGfxGetWinMinX();
		float maxY = AEGfxGetWinMaxY();

		float cell = 18.0f;
		float gap = 4.0f;

		// Top-left corner anchor
		float startXPix = minX + 30.0f;
		float startYPix = maxY - 30.0f;

		AEGfxSetRenderMode(AE_GFX_RM_COLOR);
		AEGfxSetBlendMode(AE_GFX_BM_BLEND);
		AEGfxSetTransparency(1.0f);

		for (int y = 0; y < gridSize; ++y)
		{
			for (int x = 0; x < gridSize; ++x)
			{
				int idx = GetRoomIdx(x, y);
				Room const& rm = rooms[idx];

				if (rm.rmType == RoomType::Empty) continue;

				Color c{ 0.6f, 0.6f, 0.6f, 0.85f }; // Normal

				if (rm.rmType == RoomType::Start) c = Color{ 0.2f, 0.9f, 0.2f, 0.90f };
				if (rm.rmType == RoomType::Boss)  c = Color{ 0.9f, 0.2f, 0.2f, 0.90f };

				if (currentRoom == &rooms[idx])   c = Color{ 0.2f, 0.4f, 1.0f, 0.95f };

				float px = startXPix + x * (cell + gap) + cell * 0.5f;
				float py = startYPix - y * (cell + gap) - cell * 0.5f;

				Sprite tile(squareMesh, Vector2{ px, py }, Vector2{ cell, cell }, c);
				tile.RenderSprite();
			}
		}
	}


	Room* Map::GetRoom(int x, int y) {
		size_t idx = static_cast<size_t>(GetRoomIdx(x, y));
		return &(Map::rooms[idx]);
	}

	Room* Map::GetCurrentRoom() {
		return currentRoom;
	}

	bool Map::MoveTo(Direction direction) {
		if (Direction::Left == direction) {
			if (currentRoom->left != nullptr) {
				currentRoom = currentRoom->left;
				// Animation
				return true;
			}
		}

		else if (Direction::Up == direction) {
			if (currentRoom->up!= nullptr) {
				currentRoom = currentRoom->up;
				// Animation
				return true;
			}
		}

		else if (Direction::Down == direction) {
			if (currentRoom->down != nullptr) {
				currentRoom = currentRoom->down;
				// Animation
				return true;
			}
		}

		else if (Direction::Right == direction) {
			if (currentRoom->right != nullptr) {
				currentRoom = currentRoom->right;
				// Animation
				return true;
			}
		}

		return false;
	}

	// Update Loop
	void Map::UpdateMap(Vector2& playerPos, Vector2 playerHalfSize, float dt)
	{
		if (!currentRoom) return;

		// Cooldown prevents immediate bounce-back after warp
		if (doorCooldown > 0.0f) {
			doorCooldown -= dt;
			if (doorCooldown < 0.0f) doorCooldown = 0.0f;
			return;
		}

		float minX = AEGfxGetWinMinX();
		float maxX = AEGfxGetWinMaxX();
		float minY = AEGfxGetWinMinY();
		float maxY = AEGfxGetWinMaxY();

		// --- Door trigger tuning knobs ---
		// span: how "wide" the doorway is (along the wall)
		// depth: how "thick" the trigger is (into the room)
		// margin: how far inside the new room we spawn after switching
		float doorSpan = 220.0f;	// Wide doorway
		float doorDepth = 80.0f;
		float warpMargin = 120.0f;	// Distance padded when entering new room

		// Trigger half-sizes
		Vector2 halfLR{ doorDepth * 0.5f, doorSpan * 0.5f }; // left/right doors
		Vector2 halfUD{ doorSpan * 0.5f, doorDepth * 0.5f }; // up/down doors

		// Door trigger centers (middle of each wall)
		Vector2 leftDoorC{ minX + doorDepth * 0.5f, 0.0f };
		Vector2 rightDoorC{ maxX - doorDepth * 0.5f, 0.0f };
		Vector2 topDoorC{ 0.0f, maxY - doorDepth * 0.5f };
		Vector2 botDoorC{ 0.0f, minY + doorDepth * 0.5f };

		// LEFT
		if (currentRoom->left && CollisionIntersection_RectRect_Static(AABB{ playerPos - playerHalfSize, playerPos + playerHalfSize },
			AABB{ leftDoorC - halfLR, leftDoorC + halfLR }))
		{
			if (MoveTo(Direction::Left)) {
				// appear at RIGHT side of the next room, inside boundary
				playerPos.x = maxX - playerHalfSize.x - warpMargin;
				doorCooldown = 0.20f;
			}
			return;
		}

		// RIGHT
		if (currentRoom->right && CollisionIntersection_RectRect_Static(AABB{ playerPos - playerHalfSize, playerPos + playerHalfSize },
			AABB{ rightDoorC - halfLR, rightDoorC + halfLR }))
		{
			if (MoveTo(Direction::Right)) {
				playerPos.x = minX + playerHalfSize.x + warpMargin;
				doorCooldown = 0.20f;
			}
			return;
		}

		// UP
		if (currentRoom->up && CollisionIntersection_RectRect_Static(AABB{ playerPos - playerHalfSize, playerPos + playerHalfSize },
			AABB{ topDoorC - halfUD, topDoorC + halfUD }))
		{
			if (MoveTo(Direction::Up)) {
				playerPos.y = minY + playerHalfSize.y + warpMargin;
				doorCooldown = 0.20f;
			}
			return;
		}

		// DOWN
		if (currentRoom->down && CollisionIntersection_RectRect_Static(AABB{ playerPos - playerHalfSize, playerPos + playerHalfSize },
			AABB{ botDoorC - halfUD, botDoorC + halfUD }))
		{
			if (MoveTo(Direction::Down)) {
				playerPos.y = maxY - playerHalfSize.y - warpMargin;
				doorCooldown = 0.20f;
			}
			return;
		}
	}

}
