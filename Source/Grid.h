/* Start Header ***********************************************************************/
/*!
\file        rooms.cpp
\author		 Quah Ming Jun, m.quah
\par         m.quah@digipen.edu
\brief
			 This header defines the Grid system used for tile-based room layouts.
			 The Grid class loads CSV room data, stores tile IDs, performs collision
			 checks, converts between world and grid coordinates, and supports biome-
			 based tile filtering. It also exposes static access to a global TileDataBase
			 used to query tile attributes, allowed biomes, and asset paths.

			 This file also defines TileType, BiomeInfo, TileDataBase, and several
			 collision flags used throughout the engine.

Copyright (C) 2026 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited.
*/
/* End Header *************************************************************************/


#include "AEEngine.h"
#include <vector>
#include <string>
#include <iostream>
#include <sstream>
#include <unordered_map>
#include "Sprite.h"	// For render?
#pragma once


/* ----------------------------------------------------------------------------
   Collision flags used for bitwise operations
---------------------------------------------------------------------------- */
const int	COLLISION_LEFT =	0x00000001;		//0001
const int	COLLISION_RIGHT =	0x00000002;		//0010
const int	COLLISION_TOP =		0x00000004;		//0100
const int	COLLISION_BOTTOM =	0x00000008;		//1000

const int	COLLISION_DOOR_LEFT =	0x00000010;
const int	COLLISION_DOOR_RIGHT =	0x00000020;
const int	COLLISION_DOOR_TOP =	0x00000040;
const int	COLLISION_DOOR_BOTTOM =	0x00000080;

// High-level tile categories used for collision and logic.
enum class GridType {
	INVALID = -1,
	EMPTY,
	WALL,
	DOOR,
	NUM_TYPES

};

// Represents a single tile in the grid with a GridType classification.
struct GridTile {
	GridType type;
};

// Metadata describing a tile ID loaded from the tile database.
struct TileType
{
	int id{};
	std::string name;

	// Attributes
	bool blocked = false;
	// maybe movable?
	// maybe cloaking (for hiding)?
	// maybe animations?
	std::string asset;

	std::string spawnCategory{};
	std::string spawnName{};
};

// Defines a biome and the tile IDs allowed within it.
struct BiomeInfo
{
	// Each biome have allowed Tiles lol
	std::string name;
	std::vector<int> allowedTiles;
	std::string imagePath;	// For future use
};

/*!
\class TileDataBase
\brief
	Stores all tile definitions and biome information loaded from JSON.
	Provides lookup functions for tile metadata and biome filtering.
*/
class TileDataBase
{
public:
	bool  Load(const std::string& filename);												// Load tile and biome data from a JSON file..
	const TileType* GetTileType(int id) const;												// Retrieve tile metadata by ID.
	bool  TileAllowedInBiome(int TileId, const std::string& biome) const;					// Check if a tile ID is allowed in a given biome.
	std::vector<TileType const*> GetTilesFromBiome(const std::string& biomeName) const;		// Retrieve all TileType pointers allowed in a biome
	std::vector<std::string> GetAllBiomes() const;											// Retrieve all biome names.
	std::string GetTexturePath(std::string const& biome);									// Retrieve texture path for a biome.

private:
	std::unordered_map<int, TileType>tileTypes;												// For querying tiletypes
	std::unordered_map<std::string, BiomeInfo> biomes;										// For querying biomes (This enables access to allowTiles already)
};

struct Padding {
	// Paddings in Pixels of Rooms (fixed-size)
	int top		;
	int right	;
	int bottom	;
	int left	;
};


/*!
\class Grid
\brief
	Represents a 2D tile grid loaded from CSV. Provides collision checks,
	tile queries, world-to-grid conversion, and rendering support.
*/
class Grid {
private: 
	int width, height, tileWidth, tileHeight;
	float offsetX, offsetY;
	std::vector<std::vector<GridTile>> tiles;
	std::vector<std::vector<int>> tiles_;
	float tileSizeX, tileSizeY;

	// Padding-html like to account for in the grid system 
	// (Top right bottom left)
	Padding pad = Padding{ 112, 124, 102, 137};
	int padding[4]{ pad.top, pad.right, pad.bottom, pad.left};
	

public:
	bool IsValid(int x, int y) const;	// Check if (x, y) is within grid bounds.
	int GetWidth() const;				// Get grid width in tiles.
	int GetHeight() const;				// Get grid height in tiles.

	// MJ HERE
	public:
		Grid();		// Construct an empty grid.
		bool LoadRoomCSV(const std::string& filename);  // Load a room layout from a CSV file.
		int  GetCell(int row, int col) const;			// Retrieve tile ID at (row, col).
		int  GetCell(int idx) const;					// Retrieve tile ID by linear index.
		int  SetCell(int row, int col, int val);		// Set tile ID at (row, col).

		// Basically Binary Map collision but include other enums
		int CheckMapGridCollision(float PosX, float PosY, float scaleX, float scaleY, int prevCell) const;
	
		// For Tile Information
		bool LoadTileDataBase(std::string const&);
		static TileType const* QueryTileType(int id);	// Query tile metadata by ID.
		bool QueryTileAllowedInBiome(int tileId, const std::string& biome);	// Check if tile is allowed in biome.

		// Checking where da player is currently in tiles
		int WorldToCell(float x, float y) const;					// Convert world coordinates to tile index.
		bool TestCollision(float x, float y, int prevCell) const;	// Test collision at world coordinate.
		static std::vector<std::string> GetAllBiomes();				// Retrieve all biome names.

		// Global tile database shared across all grids.
		static TileDataBase tileDB;

		Vector2 CellToWorldCenter(int row, int col) const;			// Convert tile coordinates to world center position.
		Vector2 CellToWorldCenter(int curCell) const;				// Convert tile index to world center position.
		float GetTileWidth() const;									// Get tile width in world units.
		float GetTileHeight() const;								// Get tile height in world units.

		// Render grid for debugging or gameplay.
		void RenderGrid(AEGfxVertexList* mesh, Vector2 playerPos, Vector2 playerScale, AEGfxRenderMode prevRender);

		// Get world boundary of grid. (Support for room)
		Vector2 GetBoundary() const;

		// Support for content editor staticmethod
		static std::vector<TileType const*> GetTilesFromBiome(std::string const& biome);	// Retrieve all TileType pointers allowed in a biome.
		static std::string GetPathNameBiome(std::string const& biome);						// Retrieve texture path for biome.

};




