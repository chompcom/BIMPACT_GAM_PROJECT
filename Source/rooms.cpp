#include "rooms.h"
#include <random>
#include <cstdlib>
#include "AEEngine.h"

namespace Config {
	// We are making an n x n grid with 1 and 0s
	static const int minGrid = 5, maxGrid = 10;
}


namespace mapRooms
{
    Room::Room(RoomType roomType) :
        left{ nullptr }, right{ nullptr }, up{ nullptr }, down{ nullptr },
        rmType{ roomType }
    {}

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

		// Somehow assign startX and startY into starting room coordinates value
		currentRoom = GetRoom(startX, startY);
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
		return (static_cast<unsigned int>(low + (std::rand() % (high - low - 1))));
	}

	// UpdateMap()
	void Map::UpdateMap(  )
	{
		// Input (Get x and y of player)
		// Perhaps testing for collision
		

		MoveTo(Direction::Left);	// If collide with door area, use MoveTo(Direction dirección)
	}

	// DeleteMap()?
	void Map::DeleteMap() 
	{
		rooms.clear();			// Clear room information
		gridSize = 0;			// Set gridsize to zero
		currentRoom = nullptr;	// Deference room item
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
	//		// Mark intermediate rooms as Normal (don’t overwrite Start/Boss)
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
				nextRoom->rmType = RoomType::Normal;
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

	Room* Map::GetRoom(int x, int y) {
		size_t idx = static_cast<size_t>(GetRoomIdx(x, y));
		return &(Map::rooms[idx]);
	}

	Room* Map::GetCurrentRoom() {
		return currentRoom;
	}

	bool Map::MoveTo(Direction dirección) {
		if (Direction::Left == dirección) {
			if (currentRoom->left != nullptr) {
				currentRoom = currentRoom->left;
				// Animation
				return true;
			}
		}

		else if (Direction::Up == dirección) {
			if (currentRoom->up!= nullptr) {
				currentRoom = currentRoom->up;
				// Animation
				return true;
			}
		}

		else if (Direction::Down == dirección) {
			if (currentRoom->down != nullptr) {
				currentRoom = currentRoom->down;
				// Animation
				return true;
			}
		}

		else if (Direction::Right == dirección) {
			if (currentRoom->right != nullptr) {
				currentRoom = currentRoom->right;
				// Animation
				return true;
			}
		}

		return false;
	}
}
