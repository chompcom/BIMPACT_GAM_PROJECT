#include "AEEngine.h"
#include <vector>
#include <string>
#include <iostream>
#include <sstream>
#include <unordered_map>
#include "Sprite.h"	// For render?
#pragma once


// ----------------------------------------------------------------------------
//
// collision flags - used for bitwise operations
//
// ----------------------------------------------------------------------------
const int	COLLISION_LEFT = 0x00000001;	//0001
const int	COLLISION_RIGHT = 0x00000002;	//0010
const int	COLLISION_TOP = 0x00000004;	//0100
const int	COLLISION_BOTTOM = 0x00000008;	//1000
const int COLLISION_DOOR_LEFT = 0x00000010;
const int COLLISION_DOOR_RIGHT = 0x00000020;
const int COLLISION_DOOR_TOP = 0x00000040;
const int COLLISION_DOOR_BOTTOM = 0x00000080;

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

	std::string spawnCategory{};
	std::string spawnName{};
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
	std::string GetTexturePath(std::string const& biome);

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
	int width, height, tileWidth, tileHeight;
	float offsetX, offsetY;
	std::vector<std::vector<GridTile>> tiles;
	std::vector<std::vector<int>> tiles_;
	float tileSizeX, tileSizeY;

	// Padding-html like to account for in the grid system 
	// (Top right bottom left)
	Padding pad = Padding{ 116, 139, 115, 131 };
	int padding[4]{ pad.top, pad.right, pad.bottom, pad.left};
	
	
public:
	/*
	Grid() : width(0), height(0), tileWidth(0), tileHeight(0), offsetX(0), offsetY(0) {}
	Grid(int w, int h, int tw, int th, float osX, float osY);
	int LoadFromFile(const char* fileName);
	GridType GetTile(int x, int y) const;
	float GetWorldWidth() const;
	float GetWorldHeight() const;
	bool IsEmpty(int x, int y) const;
	void SetTile(int x, int y, GridType type);
	void SetOffset(float osX, float osY);
	void PrintRetrievedInformation(void);
	void RenderGrid(AEGfxVertexList* mesh);
	int CheckInstanceBinaryMapCollision(float PosX, float PosY, float scaleX, float scaleY);
	bool IsDoor(int x, int y) const;
	//
	*/
	bool IsValid(int x, int y) const;
	int GetWidth() const;
	int GetHeight() const;

	// MJ HERE
	public:
		Grid();
		bool LoadRoomCSV(const std::string& filename);
		int  GetCell(int row, int col) const;
		int  GetCell(int idx) const;
		int  SetCell(int row, int col, int val);
		//Vector2 Grid::CellToWorldCenter(int row, int col) const;

		// Basically Binary Map collision but include other enums
		int CheckMapGridCollision(float PosX, float PosY, float scaleX, float scaleY, int prevCell) const;
	
		// For Tile Information
		bool LoadTileDataBase(std::string const&);
		static TileType const* QueryTileType(int id);
		bool QueryTileAllowedInBiome(int tileId, const std::string& biome);

		// Checking where da player is currently in tiles
		int WorldToCell(float x, float y) const;
		bool TestCollision(float x, float y, int prevCell) const;
		static std::vector<std::string> GetAllBiomes();

		static TileDataBase tileDB;

		Vector2 CellToWorldCenter(int row, int col) const;
		float GetTileWidth() const;
		float GetTileHeight() const;

		void RenderGrid(AEGfxVertexList* mesh, Vector2 playerPos, Vector2 playerScale, AEGfxRenderMode prevRender);

		// Support for content editor staticmethod
		static std::vector<TileType const*> GetTilesFromBiome(std::string const& biome);
		static std::string GetPathNameBiome(std::string const& biome);
	private:
		//std::vector<std::vector<int>> cells;
		//int height, width;

};




