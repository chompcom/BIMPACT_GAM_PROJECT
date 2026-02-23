#include "AEEngine.h"
#include "Grid.h"

Grid::Grid(int w, int h, int ts) : width(w), height(h), tileSize(ts), tiles(h, std::vector<GridTile>(w)) {} // take in Tile width and height and tile size
// if you want to call use Grid name(10,10,50) etc.

GridTile& Grid::GetTile(int x, int y) {
	return tiles[y][x];
}
int Grid::GetHeight() const {
	return height * tileSize;
}
int Grid::GetWidth() const {
	return width * tileSize;
}
bool Grid::IsValid(int x, int y) const {
	return  tiles.at(y).at(x).type ==  GridType::EMPTY; // if i use at it is slower but check bounds compared to Tiles[w][h], see performance first and change if needed 
}