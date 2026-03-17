#include "AEEngine.h"
#include <vector>
#include <string>
#include <iostream>
#include <sstream>
#include <unordered_map>
#pragma once


// ----------------------------------------------------------------------------
//
// collision flags - used for bitwise operations
//
// ----------------------------------------------------------------------------
const int	COLLISION_LEFT		= 0x00000001;	//0001
const int	COLLISION_RIGHT		= 0x00000002;	//0010
const int	COLLISION_TOP		= 0x00000004;	//0100
const int	COLLISION_BOTTOM	= 0x00000008;	//1000


enum class GridType {
	INVALID = -1,
	EMPTY,
	WALL,
	DOOR,
	NUM_TYPES

};


struct GridTile {
	GridType type;
};


// FOR INTERWORKINGNESS?
struct TileType
{
	int id;
	std::string name;

	// Attributes
	bool blocked = false;
	// maybe movable?
	// maybe cloaking (for hiding)?
	// maybe animations?
	std::string asset;
};

struct BiomeInfo
{
	// Each biome have allowed Tiles lol
	std::string name;
	std::vector<int> allowedTiles;
	std::string imagePath;	// For future use
};

class TileDataBase
{
public:
	//TileDataBase(const std::string& filename);
	bool  Load(const std::string& filename);
	const TileType* GetTileType(int id) const;
	bool  TileAllowedInBiome(int TileId, const std::string& biome) const;
	std::vector<TileType const*> GetTilesFromBiome(const std::string& biomeName) const;
	std::vector<std::string> GetAllBiomes() const;

private:
	std::unordered_map<int, TileType>tileTypes;				// For querying tiletypes
	std::unordered_map<std::string, BiomeInfo> biomes;		// For querying biomes (This enables access to allowTiles already)
	//std::unordered_map<BiomeInfo, std::vector<TileType>> biomeTiles;	// For querying tiles from BiomeInfo (For RoomInit)
};

struct Padding {
	// Paddings in Pixels of Rooms (fixed-size)
	int top		;
	int right	;
	int bottom	;
	int left	;
};

class Grid {
private: 
	int width, height, tileSize;
	std::vector<std::vector<GridTile>> tiles;
	std::vector<std::vector<int>> tiles_;
	float tileSizeX, tileSizeY;

	// Padding-html like to account for in the grid system 
	// (Top right bottom left)
	Padding pad = Padding{ 116, 139, 115, 131 };
	int padding[4]{ pad.top, pad.right, pad.bottom, pad.left};
	
	
public:
	Grid(int w, int h, int ts);	
	int LoadFromFile(const char* fileName);
	const GridType GetTile(int x, int y) const;

	int GetWidth() const;
	int GetHeight() const;
	
	bool IsValid(int x, int y) const;
	bool IsEmpty(int x, int y) const;
	void SetTile(int x, int y, GridType type);
	int CheckInstanceBinaryMapCollision(float PosX, float PosY, float scaleX, float scaleY);
	bool IsDoor(int x, int y) const;


	// MJ HERE
	public:
		Grid();
		bool LoadRoomCSV(const std::string& filename, const std::string& biome);
		int  GetCell(int row, int col) const;
		int  GetCell(int idx) const;

		// Basically Binary Map collision but include other enums
		int CheckMapGridCollision(float PosX, float PosY, float scaleX, float scaleY) const;
	
		// For Tile Information
		bool LoadTileDataBase(std::string const&);
		static TileType const* QueryTileType(int id);
		bool QueryTileAllowedInBiome(int tileId, const std::string& biome);

		// Checking where da player is currently in tiles
		int WorldToCell(float x, float y) const;
		bool TestCollision(float x, float y) const;
		static std::vector<std::string> GetAllBiomes();

		static TileDataBase tileDB;
	private:
		//std::vector<std::vector<int>> cells;
		//int height, width;

};




