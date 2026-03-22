#include "AEEngine.h"
#include "Grid.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <unordered_map>

#include "Loaders/DataLoader.h"	// For Json


namespace {
	// Man wts excel csv is in utf-8 BOM encoding...
	static void RemoveUtf8Bom(std::string& s)
	{
		if (s.size() >= 3 &&
			static_cast<unsigned char>(s[0]) == 0xEF &&
			static_cast<unsigned char>(s[1]) == 0xBB &&
			static_cast<unsigned char>(s[2]) == 0xBF)
		{
			s.erase(0, 3);
		}
	}
}

/*

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


GridType Grid::GetTile(int x, int y) const {
	if (!IsValid(x, y))
		return GridType::EMPTY;
	return tiles[y][x].type;
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

*/

// MJ Implementation using the above as a base
//TileDataBase Grid::tileDB;
//static TileDataBase tileDB{".\Assets\Levels\Room_Data\TilesInfo.json"};
//TileDataBase Grid::tileDB{ ".\Assets\Levels\Room_Data\TilesInfo.json" };

TileDataBase Grid::tileDB{};

Grid::Grid() : width(0), height(0), tileSizeX(0), tileSizeY(0) {
	Grid::tileDB.Load(".\\Assets\\Levels\\Room_Data\\TilesInfo.json");
};

bool Grid::IsValid(int x, int y) const {
	return x >= 0 && x < width && y >= 0 && y < height;
}

int Grid::GetHeight() const {
	return height;
}
int Grid::GetWidth() const {
	return width;
}

//Grid::LoadRoomCSV()

bool Grid::LoadTileDataBase(std::string const& fileName) {
	return tileDB.Load(fileName);
}

// Query Type of Tile and TileType
TileType const* Grid::QueryTileType(int id) { return Grid::tileDB.GetTileType(id); };
bool Grid::QueryTileAllowedInBiome(int tileId, const std::string& biome) {return tileDB.TileAllowedInBiome(tileId, biome);}

bool Grid::LoadRoomCSV(std::string const& fileName)
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

			RemoveUtf8Bom(key);
			RemoveUtf8Bom(value);

			metaData[key] = value;

		}
	}

	// Get biome information
	//std::string biome = metaData["Biome"];
	std::string biome = metaData.at("Biome");



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
			if (!QueryTileAllowedInBiome(tileId, biome)) 
				tileId = 0; // back to nth ig

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
		this->tileSizeX = totalGridWidth / static_cast<float>(this->width);

		// Row
		float totalGridHeight = static_cast<float>(AEGfxGetWindowHeight()) - this->padding[0] - this->padding[2];
		this->tileSizeY = totalGridHeight / static_cast<float>(this->height);
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

	int const outOfBound = 0xffffffff;
	if (idx == outOfBound) return -1;

	int x = idx % this->width;
	int y = idx / this->width;

	if (!(IsValid(x, y))) return -1;

	return tiles_[y][x];
}


int  Grid::SetCell(int row, int col, int val) {
	if (row < 0 || row >= height) return -1;
	if (col < 0 || col >= width) return -1;

	tiles_[row][col] = val;
	return 0;
}

int Grid::WorldToCell(float x, float y) const
{
	//if (tileSizeX <= 0.0f || tileSizeY <= 0.0f) return -1;

	//// Check if they are within the padding
	//// I forget how coordinates work in this game lmao
	//if (y < this->pad.top || y > (AEGfxGetWindowHeight()-this->pad.bottom) || 
	//	x < this->pad.left || x > (AEGfxGetWindowWidth() - this->pad.right)) return -1;

	//// Offset then calculate
	//int col = static_cast<int>((x - this->pad.left) / (this->tileSizeX));
	//int row = static_cast<int>((y - this->pad.top) / (this->tileSizeY));

	////int col = static_cast<int>(x / tileSizeX);
	////int row = static_cast<int>(y / tileSizeY);

	//if (row < 0 || row >= height || col < 0 || col >= width)
	//	return -1;

	//return row * width + col;


	if (tileSizeX <= 0.0f || tileSizeY <= 0.0f) return -1;

	float left		= AEGfxGetWinMinX() + this->pad.left;
	float right		= AEGfxGetWinMaxX() - this->pad.right;
	float top		= AEGfxGetWinMaxY() - this->pad.top;
	float bottom	= AEGfxGetWinMinY() + this->pad.bottom;

	if (x < left || x > right || y < bottom || y > top) return -1;

	float localX = x - left;
	float localY = top - y;   // row 0 starts at the top

	int col = static_cast<int>(localX / tileSizeX);
	int row = static_cast<int>(localY / tileSizeY);

	if (row < 0 || row >= height || col < 0 || col >= width)
		return -1;

	return row * width + col;

}

bool Grid::TestCollision(float x, float y, int prevCell) const {
	int idx = this->WorldToCell(x, y);

	// Out of bounds
	if (idx < 0) {

		// Check for doors
		const int doorFlag = 0x64;
		const int outOfBounds = 0xffffffff;

		if (this->WorldToCell(x, y + tileSizeY) == outOfBounds && this->GetCell(this->WorldToCell(x, y - tileSizeY))==doorFlag) {	// TOP DOOR?
			return false;
		}
		else if (this->WorldToCell(x, y - tileSizeY) == outOfBounds && this->GetCell(this->WorldToCell(x, y + tileSizeY)) == doorFlag) {	// BOTTOM DOOR?
			return false;
		}
		else if (this->WorldToCell(x - tileSizeX, y) == outOfBounds && this->GetCell(this->WorldToCell(x + tileSizeX, y)) == doorFlag) {
			return false;
		}
		else if (this->WorldToCell(x + tileSizeX, y) == outOfBounds && this->GetCell(this->WorldToCell(x - tileSizeX, y)) == doorFlag) {
			return false;
		}
		else if (this->WorldToCell(x, y) == outOfBounds && this->GetCell(prevCell) == doorFlag) {	// ME ENOJADA. ESTA ES DIFICIL...
			return false;
		}

		return true;	// depends out of bounds may as well collide



	}; 

	// Gonna account for doors somehow

	int tileId = this->GetCell(idx);
	if (tileId < 0) return false;	// tile id shouldn't be less than 0 unless there's a problem in our json

	// Get tile type
	TileType const* tile = Grid::tileDB.GetTileType(tileId);
	if (!tile) return false;

	return tile->blocked;
}

int Grid::CheckMapGridCollision(float PosX, float PosY, float scaleX, float scaleY, int prevCell) const
{
	//At the end of this function, "Flag" will be used and returned, to determine which sides
	//of the object instance are colliding. 2 hot spots will be placed on each side.

	int Flag = 0x0000;
	float x1, y1, x2, y2, x3, y3;

	/*----------------------LEFT-------------------------------*/
	// 1.1 Hotspot 1
	x1 = PosX - scaleX / 2;		// To reach the left side
	y1 = PosY + scaleY / 4;		// To go up 1/4 of the height
	if (TestCollision(x1, y1, prevCell)) Flag |= COLLISION_LEFT;

	// 1.2 Hotspot 2
	x2 = PosX - scaleX / 2;		// To reach the left side
	y2 = PosY - scaleY / 4;		// To go down 1/4 of the height
	if (TestCollision(x2, y2, prevCell)) Flag |= COLLISION_LEFT;

	// 1.3 Hotspot 3
	x3 = PosX - scaleX / 2;		// To reach the left side
	y3 = PosY;					// To go down 1/2 of the height
	if (TestCollision(x3, y3, prevCell)) Flag |= COLLISION_LEFT;


	/*----------------------RIGHT-------------------------------*/
	// 2.1 Hotspot 1
	x1 = PosX + scaleX / 2;		// To reach the right side
	y1 = PosY + scaleY / 4;		// To go up 1/4 of the height
	if (TestCollision(x1, y1, prevCell)) Flag |= COLLISION_RIGHT;

	// 2.2 Hotspot 2
	x2 = PosX + scaleX / 2;		// To reach the right side
	y2 = PosY - scaleY / 4;		// To go down 1/4 of the height
	if (TestCollision(x2, y2, prevCell)) Flag |= COLLISION_RIGHT;

	// 2.3 Hotspot 3
	x3 = PosX + scaleX / 2;		// To reach the left side
	y3 = PosY;					// To go down 1/2 of the height
	if (TestCollision(x3, y3, prevCell)) Flag |= COLLISION_RIGHT;


	/*----------------------TOP-------------------------------*/
	y1 = y2 = PosY + scaleY / 2; // To reach top side
	// 3.1 Hotspot 1
	x1 = PosX - scaleX / 4;		// Top 25% hotspot
	if (TestCollision(x1, y1, prevCell)) Flag |= COLLISION_TOP;

	// 3.1 Hotspot 2
	x2 = PosX + scaleX / 4;		// Top 75% hotspot
	if (TestCollision(x2, y2, prevCell)) Flag |= COLLISION_TOP;

	// 3.3 Hotspot 3
	x3 = PosX;
	y3 = PosY + scaleY / 2;
	if (TestCollision(x3, y3, prevCell)) Flag |= COLLISION_TOP;


	/*----------------------BOTTOM-------------------------------*/
	y1 = y2 = PosY - scaleY / 2; // To reach bottom side
	// 4.1 Hotspot 1
	x1 = PosX - scaleX / 4;		 // Bottom 25% hotspot
	if (TestCollision(x1, y1, prevCell)) Flag |= COLLISION_BOTTOM;

	// 4.2 Hotspot 2
	x2 = PosX + scaleX / 4;		 // Bottom 75% hotspot
	if (TestCollision(x2, y2, prevCell)) Flag |= COLLISION_BOTTOM;

	// 3.3 Hotspot 3
	x3 = PosX;
	y3 = PosY - scaleY / 2;
	if (TestCollision(x3, y3, prevCell)) Flag |= COLLISION_BOTTOM;


	return Flag;
}

bool TileDataBase::Load(std::string const& fileName)
{
	//using JsonOut = Json::Value;
	Json::Value file = DataLoader::LoadJsonFile(fileName);

	tileTypes.clear();
	biomes.clear();

	Json::Value const& rootTile = file["tileTypes"];
	std::vector<std::string> tileKeys = rootTile.getMemberNames();

	for (std::size_t i = 0; i < tileKeys.size(); ++i)
	{
		std::string const& key = tileKeys[i];
		Json::Value const& tile = rootTile[key];

		TileType newTile{};
		newTile.id = std::stoi(key);
		newTile.name = tile.get("name", "").asString();
		newTile.blocked = tile.get("blocked", false).asBool();
		newTile.asset = tile.get("asset", "").asString();

		tileTypes[newTile.id] = newTile;
	}

	// Load biomes
	Json::Value const& rootBiome = file["biomes"];
	std::vector<std::string> biomeKeys = rootBiome.getMemberNames();

	for (std::size_t i = 0; i < biomeKeys.size(); ++i)
	{
		std::string const& key = biomeKeys[i];
		Json::Value const& biomeJson = rootBiome[key];

		BiomeInfo biome{};
		biome.name = key;

		Json::Value const& allowed = biomeJson["allowedTiles"];
		for (Json::ArrayIndex j = 0; j < allowed.size(); ++j)
		{
			biome.allowedTiles.push_back(allowed[j].asInt());
		}

		biome.imagePath = biomeJson.get("imagePath", "").asString();

		biomes[biome.name] = biome;
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


Vector2 Grid::CellToWorldCenter(int row, int col) const 
{
	float left = AEGfxGetWinMinX() + this->pad.left;	// MIN LEFT
	float top = AEGfxGetWinMaxY() - this->pad.top;		// MAX TOP

	float x = left + (col * tileSizeX) + (tileSizeX * 0.5f);	// Get x 
	float y = top - (row * tileSizeY) - (tileSizeY * 0.5f);		// Get y

	return Vector2{ x, y };
}

float Grid::GetTileWidth() const
{
	return tileSizeX;
}

float Grid::GetTileHeight() const
{
	return tileSizeY;
}


bool TOGGLE_STATE = false;

void Grid::RenderGrid(AEGfxVertexList* mesh, Vector2 playerPos, Vector2 playerScale, AEGfxRenderMode prevRender)
{
	if (!mesh || width <= 0 || height <= 0) return;

	if (AEInputCheckTriggered(AEVK_K)) TOGGLE_STATE = !TOGGLE_STATE;
	if (TOGGLE_STATE == false) return;

	AEGfxSetRenderMode(AE_GFX_RM_COLOR);
	AEGfxSetBlendMode(AE_GFX_BM_BLEND);
	AEGfxSetTransparency(1.0f);

	int playerIdx = WorldToCell(playerPos.x, playerPos.y);
	int playerRow = -1;
	int playerCol = -1;

	if (playerIdx >= 0)
	{
		playerCol = playerIdx % width;
		playerRow = playerIdx / width;
	}

	// Grid starts from top left 0, 0
	float left = AEGfxGetWinMinX() + static_cast<float>(pad.left);
	float top = AEGfxGetWinMaxY() - static_cast<float>(pad.top);

	//// Render player tile
	//if (playerIdx >= 0)
	//{
	//	float x = left + playerCol * tileSizeX + tileSizeX/2.0f;
	//	float y = top - playerRow *  tileSizeY - tileSizeY/2.0f;

	//	Sprite cell(mesh, Vector2{ x, y }, Vector2{ tileSizeX, tileSizeY }, Color{ 0.2f, 0.8f, 0.2f, 0.35f });
	//	cell.RenderSprite();
	//}

	// Render player tiles 2 (take into account scaling)
	float halfW = playerScale.x / 2.0f;
	float halfH = playerScale.y / 2.0f;

	int idxTL = WorldToCell(playerPos.x - halfW, playerPos.y + halfH);	// Top Left Corner
	int idxTR = WorldToCell(playerPos.x + halfW, playerPos.y + halfH);	// Top Right Corner
	int idxBL = WorldToCell(playerPos.x - halfW, playerPos.y - halfH);	// Bottom Left corner
	int idxBR = WorldToCell(playerPos.x + halfW, playerPos.y - halfH);	// Bottom Right corner

	int minRow = height, maxRow = -1;
	int minCol = width, maxCol = -1;

	if (idxTL >= 0)
	{
		int row = idxTL / width;
		int col = idxTL % width;
		if (row < minRow) minRow = row;
		if (row > maxRow) maxRow = row;
		if (col < minCol) minCol = col;
		if (col > maxCol) maxCol = col;
	}

	if (idxTR >= 0)
	{
		int row = idxTR / width;
		int col = idxTR % width;
		if (row < minRow) minRow = row;
		if (row > maxRow) maxRow = row;
		if (col < minCol) minCol = col;
		if (col > maxCol) maxCol = col;
	}

	if (idxBL >= 0)
	{
		int row = idxBL / width;
		int col = idxBL % width;
		if (row < minRow) minRow = row;
		if (row > maxRow) maxRow = row;
		if (col < minCol) minCol = col;
		if (col > maxCol) maxCol = col;
	}

	if (idxBR >= 0)
	{
		int row = idxBR / width;
		int col = idxBR % width;
		if (row < minRow) minRow = row;
		if (row > maxRow) maxRow = row;
		if (col < minCol) minCol = col;
		if (col > maxCol) maxCol = col;
	}

	if (maxRow >= 0 && maxCol >= 0)
	{
		for (int row = minRow; row <= maxRow; ++row)
		{
			for (int col = minCol; col <= maxCol; ++col)
			{
				float x = left + col * tileSizeX + tileSizeX / 2.0f;
				float y = top - row * tileSizeY - tileSizeY / 2.0f;

				Sprite cell(mesh, Vector2{ x, y }, Vector2{ tileSizeX, tileSizeY }, Color{ 0.2f, 0.8f, 0.2f, 0.35f });
				cell.RenderSprite();
			}
		}
	}

	// Draw box
	// a) Verticle lines
	for (int col = 0; col <= width; ++col) {
		float x = left + col * tileSizeX;	// offset + (current line x location)
		float y = top - (height * tileSizeY) * 0.5f;  // top offset padding - (line height / 2) to get center grid
		Sprite line(mesh, Vector2{ x, y }, Vector2{ 1.0f, height * tileSizeY }, Color{ 0.f, 0.f, 0.f, 0.45f }); // Render @ x,y but scale to entire line
		line.RenderSprite();
	}

	// b) Horizontal Lines
	for (int row = 0; row <= height; ++row) {
		float y = top - row * tileSizeY;
		float x = left + (width * tileSizeX) * 0.5f;
		Sprite line(mesh, Vector2{ x, y }, Vector2{ width * tileSizeX, 1.0f }, Color{ 0.f, 0.f, 0.f, 0.45f });
		line.RenderSprite();
	}


	AEGfxSetRenderMode(prevRender);
}
