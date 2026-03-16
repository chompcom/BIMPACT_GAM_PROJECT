#include "AEEngine.h"
#include "Grid.h"
#include <fstream>
#include <iostream>
#include <sstream>

Grid::Grid(int w, int h, int tw, int th,float osX, float osY)
	: width(w), height(h), tileWidth(tw), tileHeight(th), offsetX(osX), offsetY(osY) {
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

	std::string line;

	std::getline(file, line);
	std::stringstream(line.substr(line.find(',') + 1)) >> width;

	std::getline(file, line);
	std::stringstream(line.substr(line.find(',') + 1)) >> height;

	std::getline(file, line);
	std::stringstream(line.substr(line.find(',') + 1)) >> tileWidth;

	std::getline(file, line);
	std::stringstream(line.substr(line.find(',') + 1)) >> tileHeight;

	tiles.clear();
	tiles.resize(height);
	for (int i = 0; i < height; ++i) {
		tiles[i].resize(width);
		std::getline(file, line);
		std::stringstream temp(line);
		std::string value;
		int j = 0;
		while (std::getline(temp, value, ',') && j < width){
			int val = std::stoi(value);
			//file >> val;
			switch (val) {
			case 1: 
				tiles[i][j].type = GridType::WALL;
				break;
			case 2:
				tiles[i][j].type = GridType::DOOR;
				break;

			default: tiles[i][j].type = GridType::EMPTY;
				break;
			}
			j++;
		}
	}
	return 1;
}

int Grid::GetTileWidth() const {
	return tileWidth;
}
int Grid::GetTileHeight() const {
	return tileHeight;
}

GridType Grid::GetTile(int x, int y) const {
	if (!IsValid(x, y))
		return GridType::EMPTY;
	return tiles[y][x].type;
}
int Grid::GetHeight() const {
	return height;
}
int Grid::GetWidth() const {
	return width;
}
float Grid::GetWorldWidth() const {
	return static_cast<float>(width * tileWidth);
}
float Grid::GetWorldHeight() const {
	return static_cast<float>(height * tileHeight);
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

void Grid::SetOffset(float osX, float osY) {
	offsetX = osX;
	offsetY = osY;
}

void Grid::PrintRetrievedInformation(void) // prints the width and height of the map, then prints the contents of MapData in a grid format
{
	std::cout << "Width " << GetWidth() << std::endl;
	std::cout << "Height " << GetHeight() << std::endl;
	for (int i = 0; i < GetHeight(); i++) {
		for (int j = 0; j < GetWidth(); j++) {
			std::cout << static_cast<int>(GetTile(j, i)) << " ";
		}
		std::cout << std::endl;
	}
	std::cout << std::endl;
}

void Grid::RenderGrid(AEGfxVertexList* mesh) {
	AEGfxSetRenderMode(AE_GFX_RM_COLOR);

	for (int i = 0; i < height; ++i) {
		for (int j = 0; j < width; ++j) {
			GridType type = tiles[i][j].type;
			float x = offsetX + j * tileWidth + tileWidth / 2.0f;
			float y = offsetY - i * tileHeight - tileHeight / 2.0f;

			Color color;
			if (type == GridType::WALL)
				color = Color{ 1.0f, 1.0, 1.0f, 1.0f };  
			else if (type == GridType::DOOR)
				color = Color{ 1.0f, 0, 0, 1.0f };  
			else if( type == GridType::EMPTY)
				color = Color{ 0, 1.0f, 1.0f, 1.0f };  
			else
				continue;

			Sprite tile(mesh, Vector2(x, y), Vector2((float)tileWidth, (float)tileHeight), color);
			tile.RenderSprite();
		}
	}
}

int Grid::CheckInstanceBinaryMapCollision(float PosX, float PosY, float scaleX, float scaleY) {
	int isTouch = 0;
	int rightx1, righty1, rightx2, righty2;
	// hotspot 1 
	rightx1 = static_cast<int>((PosX + scaleX / 2 - offsetX) / tileWidth);
	righty1 = static_cast<int>((offsetY - (PosY + scaleY / 4)) / tileHeight);
	//hotspot 2 
	rightx2 = static_cast<int>((PosX + scaleX / 2 - offsetX) / tileWidth);
    righty2 = static_cast<int>((offsetY - (PosY - scaleY / 4)) / tileHeight);
	GridType r1 = GetTile(rightx1, righty1);
	GridType r2 = GetTile(rightx2, righty2);

	if (r1 == GridType::WALL || r2 == GridType::WALL)
	{
		isTouch |= COLLISION_RIGHT;
	}

	if (r1 == GridType::DOOR || r2 == GridType::DOOR)
	{
		isTouch |= COLLISION_DOOR_RIGHT;
	}

	int leftx1, lefty1, leftx2, lefty2;
	//hotspot 1 
	leftx1 = static_cast<int>((PosX - scaleX / 2 - offsetX) / tileWidth);
	lefty1 = static_cast<int>((offsetY - (PosY + scaleY / 4)) / tileHeight);
	//hotspot 2
	leftx2 = static_cast<int>((PosX - scaleX / 2 - offsetX) / tileWidth);
	lefty2 = static_cast<int>((offsetY - (PosY - scaleY / 4)) / tileHeight);
	
	GridType l1 = GetTile(leftx1, lefty1);
	GridType l2 = GetTile(leftx2, lefty2);
	if(l1 == GridType::WALL || l2 == GridType::WALL)
	{
		isTouch |= COLLISION_LEFT;
	}

	if (l1 == GridType::DOOR || l2 == GridType::DOOR)
	{
		isTouch |= COLLISION_DOOR_LEFT;
	}

	int topx1, topy1, topx2, topy2; //hotspot 1
	topx1 = static_cast<int>((PosX + scaleX / 4 - offsetX) / tileWidth);
	topy1 = static_cast<int>((offsetY - (PosY + scaleY / 2)) / tileHeight);
	//hotspot 2
	topx2 = static_cast<int>((PosX - scaleX / 4 - offsetX) / tileWidth);
	topy2 = static_cast<int>((offsetY - (PosY + scaleY / 2)) / tileHeight);

	GridType t1 = GetTile(topx1, topy1);
	GridType t2 = GetTile(topx2, topy2);
	if (t1 == GridType::WALL || t2 == GridType::WALL)
	{
		isTouch |= COLLISION_TOP;
	}

	if (t1 == GridType::DOOR || t2 == GridType::DOOR)
	{
		isTouch |= COLLISION_DOOR_TOP;
	}
	
	int bottomx1, bottomy1, bottomx2, bottomy2; //hotspot 1
	bottomx1 = static_cast<int>((PosX + scaleX / 4 - offsetX) / tileWidth);
	bottomy1 = static_cast<int>((offsetY - (PosY - scaleY / 2)) / tileHeight);
	//hotspot 2
	bottomx2 = static_cast<int>((PosX - scaleX / 4 - offsetX) / tileWidth);
	bottomy2 = static_cast<int>((offsetY - (PosY - scaleY / 2)) / tileHeight);

	GridType b1 = GetTile(bottomx1, bottomy1);
	GridType b2 = GetTile(bottomx2, bottomy2);
	if (b1 == GridType::WALL || b2 == GridType::WALL)
	{
		isTouch |= COLLISION_BOTTOM;
	}

	if (b1 == GridType::DOOR || b2 == GridType::DOOR)
	{
		isTouch |= COLLISION_DOOR_BOTTOM;
	}

	return isTouch;
	//return  tiles.at(y).at(x).type ==  GridType::EMPTY; // if i use at it is slower but check bounds compared to Tiles[w][h], see performance first and change if needed 
}
bool Grid::IsDoor(int x, int y) const {
	return  tiles.at(y).at(x).type == GridType::DOOR; // if i use at it is slower but check bounds compared to Tiles[w][h], see performance first and change if needed 
}