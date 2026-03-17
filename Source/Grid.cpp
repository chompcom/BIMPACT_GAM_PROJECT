#include "AEEngine.h"
#include "Grid.h"
#include <fstream>
#include <unordered_map>

#include "Loaders/DataLoader.h"	// For Json

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
	//return  tiles.at(y).at(x).type ==  GridType::EMPTY; // if i use at it is slower but check bounds compared to Tiles[w][h], see performance first and change if needed 
}
bool Grid::IsDoor(int x, int y) const {
	return  tiles.at(y).at(x).type == GridType::DOOR; // if i use at it is slower but check bounds compared to Tiles[w][h], see performance first and change if needed 
}



// MJ Implementation using the above as a base
//TileDataBase Grid::tileDB;
//static TileDataBase tileDB{".\Assets\Levels\Room_Data\TilesInfo.json"};
//TileDataBase Grid::tileDB{ ".\Assets\Levels\Room_Data\TilesInfo.json" };

TileDataBase Grid::tileDB{};

Grid::Grid() : width(0), height(0), tileSize(0), tileSizeX(0), tileSizeY(0) {
	Grid::tileDB.Load(".\\Assets\\Levels\\Room_Data\\TilesInfo.json");
};



//Grid::LoadRoomCSV()

bool Grid::LoadTileDataBase(std::string const& fileName) {
	return tileDB.Load(fileName);
}

// Query Type of Tile and TileType
TileType const* Grid::QueryTileType(int id) { return Grid::tileDB.GetTileType(id); };
bool Grid::QueryTileAllowedInBiome(int tileId, const std::string& biome) {return tileDB.TileAllowedInBiome(tileId, biome);}

bool Grid::LoadRoomCSV(std::string const& fileName, std::string const& biomeName)
{
	tiles_.clear();

	std::ifstream file(fileName);
	if (!file.is_open()) return false;

	// Get metadata (while newline does not contain numbers at the beginning
	std::unordered_map<std::string, std::string> metaData;
	std::string tmpLine;
	while (file.peek() != EOF && !std::isdigit(file.peek())) {
		if (std::getline(file, tmpLine) && !tmpLine.empty()) {
			std::stringstream ss(tmpLine);
			std::string key, value;

			std::getline(ss, key, ',');
			std::getline(ss, value, ',');

			metaData[key] = value;

		}
	}

	// Get biome information
	std::string biome = metaData["Biome"];


	// Get remaining room data
	std::string line;
	while (std::getline(file, line)) {
		if (line.empty()) continue;

		std::stringstream ss{line};
		std::string value;

		std::vector<int> row;

		while (std::getline(ss, value, ',')) {
			int tileId = std::stoi(value);

			// Biome Validation
			if (!QueryTileAllowedInBiome(tileId, biome)) tileId = 0; // back to nth ig

			row.push_back(tileId);
		}

		if (!row.empty()) tiles_.push_back(row);
		
	}

	// Return
	this->height = (int)tiles_.size();
	if (this->height > 0) { 
		width = (int)tiles_.at(0).size(); 
		// Column
		float totalGridWidth = static_cast<float>(AEGfxGetWindowWidth()) - this->padding[1] - this->padding[3];
		this->tileSizeX = static_cast<float>(this->width) / totalGridWidth;

		// Row
		float totalGridHeight = static_cast<float>(AEGfxGetWindowWidth()) - this->padding[0] - this->padding[2];
		this->tileSizeY = static_cast<float>(this->height) / totalGridHeight;	// ROW
	}


	return (width > 0 && height > 0);
}

int Grid::GetCell(int x, int y) const
{
	if (y < 0 || y >= height) return -1;
	if (x < 0 || x >= width) return -1;
	return tiles_[y][x];
}

int Grid::GetCell(int idx) const {
	int x = idx % this->width;
	int y = idx / this->width;

	if (!(IsValid(x, y))) return -1;

	return tiles_[y][x];
}

int Grid::WorldToCell(float x, float y) const
{
	if (tileSizeX <= 0.0f || tileSizeY <= 0.0f) return -1;

	// Check if they are within the padding
	// I forget how coordinates work in this game lmao
	if (y < this->pad.top || y > (AEGfxGetWindowHeight()-this->pad.bottom) || 
		x < this->pad.left || x > (AEGfxGetWindowWidth() - this->pad.right)) return -1;

	// Offset then calculate
	int col = static_cast<int>((x - this->pad.left) / (this->tileSizeX));
	int row = static_cast<int>((y - this->pad.top) / (this->tileSizeY));

	//int col = static_cast<int>(x / tileSizeX);
	//int row = static_cast<int>(y / tileSizeY);

	if (row < 0 || row >= height || col < 0 || col >= width)
		return -1;

	return row * width + col;

}

bool Grid::TestCollision(float x, float y) const {
	int idx = this->WorldToCell(x, y);
	if (idx < 0) return true; // depends out of bounds may as well collide

	int tileId = this->GetCell(idx);
	if (tileId < 0) return false;	// tile id shouldn't be less than 0 unless there's a problem in our json

	// Get tile type
	TileType const* tile = Grid::tileDB.GetTileType(tileId);
	if (!tile) return false;

	return tile->blocked;
}

int Grid::CheckMapGridCollision(float PosX, float PosY, float scaleX, float scaleY) const
{
	//At the end of this function, "Flag" will be used and returned, to determine which sides
	//of the object instance are colliding. 2 hot spots will be placed on each side.

	int Flag = 0x0000;
	float x1, y1, x2, y2;

	/*----------------------LEFT-------------------------------*/
	// 1.1 Hotspot 1
	x1 = PosX - scaleX / 2;		// To reach the left side
	y1 = PosY + scaleY / 4;		// To go up 1/4 of the height
	if (TestCollision(x1, y1)) Flag |= COLLISION_LEFT;

	// 1.2 Hotspot 2
	x2 = PosX - scaleX / 2;		// To reach the left side
	y2 = PosY - scaleY / 4;		// To go down 1/4 of the height
	if (TestCollision(x2, y2)) Flag |= COLLISION_LEFT;


	/*----------------------RIGHT-------------------------------*/
	// 2.1 Hotspot 1
	x1 = PosX + scaleX / 2;		// To reach the right side
	y1 = PosY + scaleY / 4;		// To go up 1/4 of the height
	if (TestCollision(x1, y1)) Flag |= COLLISION_RIGHT;

	// 2.2 Hotspot 2
	x2 = PosX + scaleX / 2;		// To reach the right side
	y2 = PosY - scaleY / 4;		// To go down 1/4 of the height
	if (TestCollision(x2, y2)) Flag |= COLLISION_RIGHT;


	/*----------------------TOP-------------------------------*/
	y1 = y2 = PosY + scaleY / 2; // To reach top side
	// 3.1 Hotspot 1
	x1 = PosX - scaleX / 4;		// Top 25% hotspot
	if (TestCollision(x1, y1)) Flag |= COLLISION_TOP;

	// 3.1 Hotspot 2
	x2 = PosX + scaleX / 4;		// Top 75% hotspot
	if (TestCollision(x2, y2)) Flag |= COLLISION_TOP;


	/*----------------------BOTTOM-------------------------------*/
	y1 = y2 = PosY - scaleY / 2; // To reach bottom side
	// 4.1 Hotspot 1
	x1 = PosX - scaleX / 4;		 // Bottom 25% hotspot
	if (TestCollision(x1, y1)) Flag |= COLLISION_BOTTOM;

	// 4.2 Hotspot 2
	x2 = PosX + scaleX / 4;		 // Bottom 75% hotspot
	if (TestCollision(x2, y2)) Flag |= COLLISION_BOTTOM;


	return Flag;
}

bool TileDataBase::Load(std::string const& fileName)
{
	//using JsonOut = Json::Value;
	Json::Value file = DataLoader::LoadJsonFile(fileName);

	Json::Value rootTile = file["tileTypes"];
	for (Json::Value& tile : rootTile) {
		// Retrive stuff
		TileType newTile;
		newTile.id = tile.asInt();
		newTile.name = tile["name"].asString();
		newTile.blocked = tile.get("blocked", false).asBool();
		newTile.asset = tile["asset"].asString();

		this->tileTypes[newTile.id] = newTile;
	}
	// Load biomes
	for (Json::Value& biomeDic : file["biomes"]) {
		// Retrive stuff
		BiomeInfo biome;
		biome.name = biomeDic.asString();

		for (Json::Value const& member : biomeDic) {
			biome.allowedTiles.push_back(member.asInt());
		}
		this->biomes[biome.name] = biome;
	}
	return true;
}

// Get tile by ID from unordered_map of tileTypes
const TileType* TileDataBase::GetTileType(int id) const
{
	auto it = tileTypes.find(id);
	if (it == tileTypes.end()) return nullptr;
	return &it->second;
}

// Get tukes from biome
std::vector< TileType const*> TileDataBase::GetTilesFromBiome(std::string const& biome) const
{
	std::vector<TileType const*> result;

	auto biomeIt = biomes.find(biome);
	if (biomeIt == biomes.end()) return result;

	std::vector<int> const& allowed = biomeIt->second.allowedTiles;

	for (std::size_t i = 0; i < allowed.size(); ++i)
	{
		TileType const* tile = GetTileType(allowed[i]);
		if (tile != nullptr) result.push_back(tile);
	}

	return result;
}

// Get tile by ID from unordered_map of biomes, check if tile id matches any in allowedTiles
bool TileDataBase::TileAllowedInBiome(int tileId, const std::string& biome) const
{
	auto biomeIt = biomes.find(biome);
	if (biomeIt == biomes.end()) return false;

	const std::vector<int>& allowed = biomeIt->second.allowedTiles;

	for (std::size_t i = 0; i < allowed.size(); ++i)
	{
		if (allowed[i] == tileId) return true;
	}

	return false;
}


std::vector<std::string> TileDataBase::GetAllBiomes() const
{
	std::vector<std::string> result;

	for (auto const& pair : biomes)
	{
		result.push_back(pair.first);
	}

	return result;
}

std::vector<std::string> Grid::GetAllBiomes()
{
	return tileDB.GetAllBiomes();
}