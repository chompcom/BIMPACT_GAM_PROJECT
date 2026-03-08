#include "AEEngine.h"
#include <vector>
#include "Sprite.h"
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
	float offsetX, offsetY;
	std::vector<std::vector<GridTile>> tiles;
	
public:
	Grid() : width(0), height(0), tileSize(0), offsetX(0), offsetY(0) {}
	Grid(int w, int h, int ts, float osX, float osY);
	int LoadFromFile(const char* fileName);
	const GridType GetTile(int x, int y) const;
	int GetWidth() const;
	int GetHeight() const;
	bool IsValid(int x, int y) const;
	bool IsEmpty(int x, int y) const;
	void SetTile(int x, int y, GridType type);
	void SetOffset(float osX, float osY);
	void PrintRetrievedInformation(void);
	void RenderGrid(AEGfxVertexList* mesh);
	int CheckInstanceBinaryMapCollision(float PosX, float PosY, float scaleX, float scaleY);
	bool IsDoor(int x, int y) const;
};

