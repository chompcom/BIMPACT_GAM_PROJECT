#include "AEEngine.h"
#include <vector>
#pragma once
enum class GridType {
	EMPTY,
	WALL,
	DOOR

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
	GridTile& GetTile(int x, int y);
	int GetWidth() const;
	int GetHeight() const;
	bool IsValid(int x, int y) const;
};

