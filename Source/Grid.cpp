#include "AEEngine.h"
#include "Grid.h"
#include <fstream>

Grid::Grid(int w, int h, int ts) 
	: width(w), height(h), tileSize(ts) {
	tiles.resize(height);
	for (int i = 0; i < height; ++i) {
		tiles[i].resize(width);
		for (int j = 0; j < width; ++j) {
			tiles[i][j].type = GridType::EMPTY;
		}
	}
}
// take in Tile width and height and tile size
// if you want to set grid use Grid name(10,10,50) etc
int Grid::LoadFromFile(const char* filename) {
	std::fstream file(filename);
	if (!file.is_open())
		return 0;

	std::string temp;
	file >> temp >> width;
	file >> temp >> height;

	tiles.clear();
	tiles.resize(height);
	for (int i = 0; i < height; ++i) {
		tiles[i].resize(width);
		for (int j = 0; j < width; ++j) {
			int val;
			file >> val;
			switch (val) {
			case 1: 
				tiles[i][j].type = GridType::WALL;
				break;
			case 2:
				tiles[i][j].type = GridType::DOOR;
				break;

			default: tiles[i][j].type = GridType::EMPTY; break;
			}
		}
	}
	return 1;
}



const GridType Grid::GetTile(int x, int y) const {
	if (IsValid(x, y)) {
		return tiles[y][x].type;
	}
	return GridType::INVALID;
}
int Grid::GetHeight() const {
	return height;
}
int Grid::GetWidth() const {
	return width;
}
bool Grid::IsValid(int x, int y) const {
	return x >= 0 && x < width && y >= 0 && y < height;
}
bool Grid::IsEmpty(int x, int y) const {
	if (!IsValid(x, y)) return false;
	return tiles[y][x].type == GridType::EMPTY;
}

void Grid::SetTile(int x, int y, GridType type) {
	if (!IsValid(x, y)) return;
	tiles[y][x].type = type;
}

int Grid::CheckInstanceBinaryMapCollision(float PosX, float PosY, float scaleX, float scaleY) {
	int isTouch = 0;
	int rightx1, righty1, rightx2, righty2;
	// hotspot 1 
	rightx1 = static_cast<int>((PosX + scaleX / 2)/tileSize);
	righty1 = static_cast<int>((PosY + scaleY / 4)/tileSize);
	//hotspot 2 
	rightx2 = static_cast<int>((PosX + scaleX / 2)/tileSize);
	righty2 = static_cast<int>((PosY - scaleY / 4)/tileSize);
	if (GetTile((int)rightx1, (int)righty1) !=  GridType::EMPTY || GetTile((int)rightx2, (int)righty2) != GridType::EMPTY) {
		isTouch |= COLLISION_RIGHT; //if either hotspot on the right side is in a collision cell, set the COLLISION_RIGHT bit in isTouch
	}
	int leftx1, lefty1, leftx2, lefty2;
	//hotspot 1 
	leftx1 = static_cast<int>((PosX - scaleX / 2)/tileSize);
	lefty1 = static_cast<int>((PosY + scaleY / 4)/tileSize);
	//hotspot 2
	leftx2 = static_cast<int>((PosX - scaleX / 2)/tileSize);
	lefty2 = static_cast<int>((PosY - scaleY / 4)/tileSize);
	if (GetTile((int)leftx1, (int)lefty1) != GridType::EMPTY || GetTile((int)leftx2, (int)lefty2) != GridType::EMPTY) {
		isTouch |= COLLISION_LEFT; //if either hotspot on the left side is in a collision cell, set the COLLISION_LEFT bit in isTouch
	}
	int topx1, topy1, topx2, topy2; //hotspot 1 
	topx1 = static_cast<int>((PosX + scaleX / 4)/tileSize);
	topy1 = static_cast<int>((PosY + scaleY / 2)/tileSize);
	//hotspot 2 
	topx2 = static_cast<int>((PosX - scaleX / 4)/tileSize);
	topy2 = static_cast<int>((PosY + scaleY / 2)/tileSize);
	if (GetTile((int)topx1, (int)topy1) != GridType::EMPTY || GetTile((int)topx2, (int)topy2) != GridType::EMPTY) {
		isTouch |= COLLISION_TOP; //if either hotspot on the top side is in a collision cell, set the COLLISION_TOP bit in isTouch
	}
	int bottomx1, bottomy1, bottomx2, bottomy2; //hotspot 1
	bottomx1 = static_cast<int>((PosX + scaleX / 4)/tileSize);
	bottomy1 = static_cast<int>((PosY - scaleY / 2)/tileSize);
	//hotspot 2
	bottomx2 = static_cast<int>((PosX - scaleX / 4)/tileSize);
	bottomy2 = static_cast<int>((PosY - scaleY / 2)/tileSize);
	if (GetTile((int)bottomx1, (int)bottomy1) != GridType::EMPTY || GetTile((int)bottomx2, (int)bottomy2) != GridType::EMPTY) {
		isTouch |= COLLISION_BOTTOM; //if either hotspot on the bottom side is in a collision cell, set the COLLISION_BOTTOM bit in isTouch
	}
	return isTouch;
	return  tiles.at(y).at(x).type ==  GridType::EMPTY; // if i use at it is slower but check bounds compared to Tiles[w][h], see performance first and change if needed 
}
bool Grid::IsDoor(int x, int y) const {
	return  tiles.at(y).at(x).type == GridType::DOOR; // if i use at it is slower but check bounds compared to Tiles[w][h], see performance first and change if needed 
}