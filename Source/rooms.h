#pragma once

/* Start Header ************************************************************************/
/*!
\file        rooms.h
\author		 Quah Ming Jun, m.quah
\par         m.quah@digipen.edu
\brief       This header defines the Room and Map classes used to represent and
			 manage a grid-based dungeon layout. A Room stores connections to
			 neighboring rooms, visual and gameplay metadata, and a Grid instance
			 used for collision and layout. The Map class owns all rooms, handles
			 procedural generation, manages transitions between rooms, and provides
			 rendering and utility functions.

Copyright (C) 2026 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited.
*/
/* End Header **************************************************************************/

#include <array>		// std::array
#include <vector>		// std::vector
#include <iostream>
#include <string>
#include <unordered_map>	// Maybe?
#include "Utils/Vector2.hpp"
#include "RoomData.h"
#include "Utils/Utils.h"
#include "ParticleSystem.h"
#include "Grid.h"

struct AEGfxTexture;
struct AEGfxVertexList;

namespace mapRooms {

	/*!
	\enum	RoomType
	\brief	Identifies the type of a room.
	*/
	enum class RoomType : unsigned char {
		Empty,
		Start,
		Normal,
		Boss
	};

	/*!
	\enum Direction
	\brief
		Represents directions used for room linking and movement.
	*/
	enum class Direction : unsigned char {
		Left,
		Right,
		Up,
		Down
	};

	/*!
	\class Room
	\brief
		Represents a single room in the dungeon. A Room stores pointers to
		adjacent rooms, visual metadata, gameplay data, and a Grid instance
		used for collision and layout. Rooms may be linked to form a graph
		structure representing the dungeon.

		TDLR: Essentially this class stores pointers to other rooms, like a graph essentially.

	\details
		A Room may contain:
		* Pointers to neighboring rooms (left, right, up, down)
		* Room type and visited state
		* Texture and biome information
		* A Grid instance for tile-based layout
		* RoomData used for gameplay logic and transitions
	*/
	class Room 
	{
	
	
	// Construct a room
	public:
		Room(RoomType type = RoomType::Empty);	// Constructor For Room Object
		~Room();	// Destructor for Room

		// If they are nullptr, like left == nullptr, door to room does not exist. This is the essence of the concept.
		Room* left;
		Room* right;
		Room* up;
		Room* down;

		RoomType rmType;
		bool visited;
		std::string themeTag{ "Default" };			// For thematic purposes

		// Additional(s):
		// Store Images of Rooms
		AEGfxTexture* roomTexture{ nullptr };
		std::string   roomTexturePath{};          // For debug purposes

		// Gameplay data
		RoomData currentRoomData;
		RoomData* toBeTransferred;

		// Initialize room state and metadata
		void Init(RoomType rmType = RoomType::Normal);

		// Update room logic.
		void Update(float dt) ;


		// Integration with grid.cpp
		std::string biome;
		std::string layoutFile;
		Grid roomGrid;
		int lastValidCell{-1};
			
	private:
		//handle collisions
		void PatchDoorCells();	// Hacky way for patching doors after init
	};

	/*!
	\class Map
	\brief
		Manages a grid of Room objects, procedural generation, room linking,
		transitions, and rendering. The Map owns all rooms and controls the
		current active room.

	\details
		Essentially:
		* Generating dungeon layout using a seed
		* Linking rooms and assigning biomes
		* Handling player movement between rooms
		* Rendering room backgrounds and minimap
		* Managing texture caching
	*/
	class Map{
	public:
		Map();		// Constructor
		~Map();		// Destructor


		// No copying across variables allowed so pointers are not lost
		Map(Map const&) = delete;
		Map& operator=(Map const&) = delete;	// Cannot use assign to copy

		// Level lifecycle
		// Initialize the map using global scene data and a seed
		void	InitMap(RoomData& globalSceneData, unsigned int seed);	// Grid size and other spawns based on seed.
		void	UpdateMap(Vector2& playerPos, Vector2 playerHalfSize, ParticleSystem& particleSystem, float dt);	// Update the map and current room.
		void	DeleteMap();								// Reset this level stuff (tbh this kinda violates the game loop taught in GIT lol)

		int		GetGridSize() const;	// Get the grid size of the room (not map btw)
		Room*	GetRoom(int x, int y);	// Retrieve a room
const	Room*	GetRoom(int x, int y) const; // Retrieve a room (const)
		Room*	GetCurrentRoom();		// Get the currently active room.


		/*!
		\brief Attempt to move to an adjacent room.
		\return True if movement succeeded.
		*/
		bool	MoveTo(Direction direction);

		// Draw background of current room
		void	RenderCurrentRoom(AEGfxVertexList* squareMesh) const; 

		// Draw a simple minimap for debugging
		void	RenderDebugMap(AEGfxVertexList* squareMesh) const;   

		// Retrieve or load a texture from disk.
		AEGfxTexture* GetOrLoadTexture(std::string const& path);

		// Retrieve transfer data for room transitions.
		RoomData&	GetTransferData();

		// Generate a random integer in [low, high].
		unsigned int RandInt(int low, int high);			// Rand generator
	private:

		// Storage
		int		gridSize;
		std::vector<Room> rooms;							// Collection of Rooms

		int startX{}, startY{};								// Starting Positions
		int bossX{}, bossY{};

		// Link two rooms in a given direction.
		void LinkRooms(Room* a, Room* b, Direction dirFromAToB);

		Room* currentRoom;									// Current Room

		// RNG things
		unsigned int rngState;

		bool	InBounds(int x, int y) const;				// Check if (x, y) is in bounds
		int		GetRoomIdx(int x, int y) const;				// Converts x, y to array index of type [int]

		void	ResetRooms();								// Reset all rooms to initial state.
		void	GenerateRooms();							// Generate starting and boss room, generate path


		RoomData* transferData;
	
		// Room background loading ---
		std::unordered_map<std::string, std::vector<std::string>> biomeRoomFiles{};
		std::vector<std::string> normalRoomFiles{};                  
		std::vector<std::string> bossRoomFiles{};                    
		std::unordered_map<std::string, AEGfxTexture*> textureCache; 

		// Load lists of room art files for each biome.
		void			LoadRoomArtLists();     
		void			AssignRoomArt();        // Assign background art to each room.

		// Door trigger cooldown (prevents re-trigger)
		float doorCooldown{ 0.0f };

		// Temp Door
		AEGfxTexture* doorTex{ nullptr };

		// Render door textures for the current room.
		void RenderRoomDoors(AEGfxVertexList* squareMesh, AEGfxTexture* doorTexture) const;


	};


}
