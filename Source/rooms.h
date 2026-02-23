#pragma once

#include <array>		// std::array
#include <vector>		// std::vector
#include <iostream>
#include <string>
#include <unordered_map>	// Maybe?
#include "Utils/Vector2.hpp"
#include "RoomData.h"
//#include "AEEngine.h"

struct AEGfxTexture;
struct AEGfxVertexList;

namespace mapRooms {


	enum class RoomType : unsigned char {
		Empty,
		Start,
		Normal,
		Boss
	};


	enum class Direction : unsigned char {
		Left,
		Right,
		Up,
		Down
	};

	// Essentially this class stores pointers to other rooms, like a graph essentially.
	class Room 
	{
	public:
		Room(RoomType type = RoomType::Empty);	// Constructor For Room Object
		~Room();

		// If they are nullptr, like left == nullptr, door to room does not exist. This is the essence of the concept
		Room* left;
		Room* right;
		Room* up;
		Room* down;

		RoomType rmType;

		// Additional(s):
		// Store Images of Rooms
		AEGfxTexture* roomTexture{ nullptr };
		std::string   roomTexturePath{};          // For debug purposes

		RoomData currentRoomData;
		RoomData* toBeTransferred;


		void Init(RoomType rmType = RoomType::Normal);
		void Update(float dt) ;
			



			//handle collisions
		

		// Should we store player position? Idk if this is the best place. (Donnid)
		
		// Enemy 
		// Enemy object, type, for sprite render
		
		// Array of objects such as enemies, obstacles (they should have their own collision data / function ptr?)
	};

	// Helper Function later on here to check collision and return "LEFT, RIGHT, UP, or DOWN"

	//extern Room* currentRoom;

	class Map{
	public:
		Map();		// Constructor
		~Map();		// Destructor


		// No copying across variables allowed so pointers are not lost
		Map(Map const&) = delete;
		Map& operator=(Map const&) = delete;	// Cannot use assign to copy

		// Level lifecycle
		void	InitMap(RoomData& globalSceneData, unsigned int seed);	// Grid size and other spawns based on seed.
		//void	UpdateMap();								// Idk
		void	UpdateMap(Vector2& playerPos, Vector2 playerHalfSize, float dt);
		void	DeleteMap();								// Reset this level stuff (tbh this kinda violates the game loop taught in GIT lol)

		int		GetGridSize() const;
		Room*	GetRoom(int x, int y);
		Room*	GetCurrentRoom();

		// Transition Scene for later ig???
		bool	MoveTo(Direction direction);

		// Draw background of current room
		void	RenderCurrentRoom(AEGfxVertexList* squareMesh) const; 

		// Draw a simple minimap for debugging
		void	RenderDebugMap(AEGfxVertexList* squareMesh) const;   

		RoomData&	GetTransferData();
	private:

		// Storage
		int		gridSize;
		std::vector<Room> rooms;							// Collection of Rooms

		int startX{}, startY{};								// Starting Positions
		int bossX{}, bossY{};

		void LinkRooms(Room* a, Room* b, Direction dirFromAToB);

		Room* currentRoom;									// Current Room

		// RNG things
		unsigned int rngState;
		unsigned int RandInt(int low, int high);			// Rand generator

		bool	InBounds(int x, int y) const;				// Check if (x, y) is in bounds
		int		GetRoomIdx(int x, int y) const;				// Converts x, y to array index of type [int]

		void	ResetRooms();
		void	GenerateRooms();							// Generate starting and boss room, generate path


		RoomData* transferData;
	
		// Room background loading ---
		std::vector<std::string> normalRoomFiles{};                  
		std::vector<std::string> bossRoomFiles{};                    
		std::unordered_map<std::string, AEGfxTexture*> textureCache; 

		void			LoadRoomArtLists();     
		AEGfxTexture*	GetOrLoadTexture(std::string const& path);
		void			AssignRoomArt();        

		// Door trigger cooldown (prevents re-trigger)
		float doorCooldown{ 0.0f };
	};
}
