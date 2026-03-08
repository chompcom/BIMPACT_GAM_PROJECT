#include "AEEngine.h"
#include <vector>
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

class Grid {
private: 
	int width, height,tileSize;
	std::vector<std::vector<GridTile>> tiles;
	
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
};

