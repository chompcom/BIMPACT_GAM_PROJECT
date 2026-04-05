/* Start Header ***********************************************************************/
/*!
\file        Grid.cpp
\author		 Quah Ming Jun, m.quah
\par         m.quah@digipen.edu
\brief
			 This source file implements the Grid and TileDataBase systems used for
			 tile-based room layouts. The Grid class loads CSV room data, stores tile
			 IDs, performs collision checks, converts between world and grid coordinates,
			 and supports biome-based tile filtering. The TileDataBase class loads tile
			 metadata and biome definitions from JSON and provides lookup utilities.

Copyright (C) 2026 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited.
*/
/* End Header *************************************************************************/

#include "AEEngine.h"
#include "Grid.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <unordered_map>

#include "Loaders/DataLoader.h"	// For Json


namespace {

	/*!
	\brief
		Man wts excel csv is in utf-8 BOM encoding...
		Hence this function is to remove UTF-8 BOM prefix from a string if present.

	\param[in,out] s
		String to sanitize.

	\par
		Excel-exported CSV files often include a UTF-8 BOM. This helper ensures
		the first three bytes (0xEF, 0xBB, 0xBF) are removed before parsing.
	*/
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


TileDataBase Grid::tileDB{};

/*!
\brief
	Construct an empty Grid and load the global tile database.

\details
	Loads TilesInfo.json immediately so tile metadata is available for
	all subsequent grid operations.
*/
Grid::Grid() : width(0), height(0), tileSizeX(0), tileSizeY(0), tileHeight(0), tileWidth(0), offsetX(0), offsetY(0) {
	const char* informationFile = ".\\Assets\\Levels\\Room_Data\\TilesInfo.json";
	Grid::tileDB.Load(informationFile);
	
};

/*!
\brief
	Check if (x, y) is within grid bounds.
*/
bool Grid::IsValid(int x, int y) const {
	return x >= 0 && x < width && y >= 0 && y < height;
}

/*!
\brief
	Get grid height in tiles.
*/
int Grid::GetHeight() const {
	return height;
}

/*!
\brief
	Get grid width in tiles.
*/
int Grid::GetWidth() const {
	return width;
}

/*!
\brief
	Load tile database from JSON file.

\return
	True if load succeeded.
*/
bool Grid::LoadTileDataBase(std::string const& fileName) {
	return tileDB.Load(fileName);
}

// Query Type of Tile and TileType
TileType const* Grid::QueryTileType(int id) { return Grid::tileDB.GetTileType(id); };	// Query tile metadata by ID.
bool Grid::QueryTileAllowedInBiome(int tileId, const std::string& biome) {return tileDB.TileAllowedInBiome(tileId, biome);} // Check if tile is allowed in biome.

/*!
\brief
	Load a room layout from a CSV file.

\par	
	Methodology:
	* Reads metadata lines until numeric rows begin.
	* Validates tiles against biome rules.
	* Computes tile sizes based on window dimensions and padding.

\return
	True if CSV loaded successfully.
*/
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

// Retrieve tile ID at (x, y). Returns -1 if out of bounds.
int Grid::GetCell(int x, int y) const
{
	if (y < 0 || y >= height) return -1;
	if (x < 0 || x >= width) return -1;
	return tiles_[y][x];
}

// Retrieve tile ID by linear index.
int Grid::GetCell(int idx) const {

	int const outOfBound = 0xffffffff;
	if (idx == outOfBound) return -1;

	int x = idx % this->width;
	int y = idx / this->width;

	if (!(IsValid(x, y))) return -1;

	return tiles_[y][x];
}

// Set tile ID at (row, col).
int  Grid::SetCell(int row, int col, int val) {
	if (row < 0 || row >= height) return -1;
	if (col < 0 || col >= width) return -1;

	tiles_[row][col] = val;
	return 0;
}

/*!
\brief
	Convert world coordinates to tile index system.

\par
	Accounts for padding and window coordinate system.
*/
int Grid::WorldToCell(float x, float y) const
{

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

	if (row < 0 || row >= height || col < 0 || col >= width) return -1;
	return row * width + col;
}

/*!
\brief
	Test collision at world coordinate (x, y).

\par
	* Returns true if tile is blocked.
	* Handles door transitions using prevCell.
*/
bool Grid::TestCollision(float x, float y, int prevCell) const {
	int idx = this->WorldToCell(x, y);

	// Out of bounds (THIS SEQUENCE IS FOR A DOOR HACK)
	if (idx < 0) {

		// Check for doors
		const int doorFlag = 0x64;
		const int outOfBounds = 0xffffffff;

		if (this->WorldToCell(x, y + tileSizeY) == outOfBounds && this->GetCell(this->WorldToCell(x, y - tileSizeY))==doorFlag) {			// TOP DOOR?
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

	int tileId = this->GetCell(idx);
	if (tileId < 0) return false;	// tile id shouldn't be less than 0 unless there's a problem in our json

	// Get tile type
	TileType const* tile = Grid::tileDB.GetTileType(tileId);
	if (!tile) return false;

	return tile->blocked;
}

/*!
\brief
	Perform collision detection using multiple hotspots around the object.

\return
	Bitmask of collision flags.
*/
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

/*!
\brief
	Load tile and biome metadata from JSON file.

\details
	Populates:
	* tileTypes: map of tile ID -> TileType
	* biomes: map of biome name -> BiomeInfo
*/
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
		newTile.spawnCategory = tile.get("spawnCategory", "").asString();
		newTile.spawnName = tile.get("spawnName", "").asString();

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

/*!
\brief
	Retrieve tile metadata by ID.

\par
	Get tile by ID from unordered_map of tileTypes
*/
const TileType* TileDataBase::GetTileType(int id) const
{
	auto it = tileTypes.find(id);
	if (it == tileTypes.end()) return nullptr;
	return &it->second;
}

/*!
\brief
	Retrieve all TileType pointers allowed in a biome.
*/
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


/*!
\brief
	Check if a tile ID is allowed in a biome.
\par
	Get tile by ID from unordered_map of biomes, check if tile id matches any in allowedTiles
*/
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

/*!
\brief
	Retrieve all biome names.
*/
std::vector<std::string> TileDataBase::GetAllBiomes() const
{
	std::vector<std::string> result;

	for (auto const& pair : biomes)
	{
		result.push_back(pair.first);
	}

	return result;
}

/*!
\brief
	Retrieve texture path for a biome.
*/
std::string TileDataBase::GetTexturePath(std::string const& biome) {
	return this->biomes[biome].imagePath;
}

/*!
\brief
	Retrieve all biome names through Grid interface.
*/
std::vector<std::string> Grid::GetAllBiomes()
{
	return tileDB.GetAllBiomes();
}

/*!
\brief
	Convert tile coordinates to world center position.
*/
Vector2 Grid::CellToWorldCenter(int row, int col) const 
{
	float left = AEGfxGetWinMinX() + this->pad.left;	// MIN LEFT
	float top = AEGfxGetWinMaxY() - this->pad.top;		// MAX TOP

	float x = left + (col * tileSizeX) + (tileSizeX * 0.5f);	// Get x 
	float y = top - (row * tileSizeY) - (tileSizeY * 0.5f);		// Get y

	return Vector2{ x, y };
}

/*!
\brief
	Convert tile index to world center position.
*/
Vector2 Grid::CellToWorldCenter(int curCell) const
{
	int x = curCell % this->width;
	int y = curCell / this->width;
	Vector2 res = this->CellToWorldCenter(y, x);
	return res;
}

// Get tile width in world units.
float Grid::GetTileWidth() const
{
	return tileSizeX;
}

// Get tile height in world units.
float Grid::GetTileHeight() const
{
	return tileSizeY;
}

// Get world boundary of grid.
Vector2 Grid::GetBoundary() const {
	Vector2 res{ this->GetWidth() * this->GetTileWidth(), this->GetHeight() * this->GetTileHeight() };
	return res;
}


bool TOGGLE_STATE = false;

/*!
\brief
	Debug-render the grid, including player tile highlighting and grid lines.

\param[in] mesh
	Mesh used for drawing tile rectangles and grid lines.

\param[in] playerPos
	Player world position.

\param[in] playerScale
	Player sprite scale, used to compute bounding corners.

\param[in] prevRender
	Previous render mode to restore after drawing.

\par
	Methodology (when active):
		1. Check prerequisites (mesh and grid dimensions). If invalid,
		   exit early.
		2. Toggle debug state when K is pressed. If debug is disabled,
		   return immediately.
		3. Compute the player's tile index and determine the bounding
		   rectangle of tiles that overlap the player's scaled sprite.
		4. Highlight all tiles intersecting the player's bounding box.
		5. Draw vertical and horizontal grid lines to visualize tile
		   boundaries.
		6. Restore the previous render mode.
*/
void Grid::RenderGrid(AEGfxVertexList* mesh, Vector2 playerPos, Vector2 playerScale, AEGfxRenderMode prevRender)
{
	if (!mesh || width <= 0 || height <= 0) return;	    // Abort if grid or mesh is invalid

	// Toggle debug mode when K is pressed
	if (AEInputCheckTriggered(AEVK_K)) TOGGLE_STATE = !TOGGLE_STATE;
	if (TOGGLE_STATE == false) return;

	// Set up color rendering
	AEGfxSetRenderMode(AE_GFX_RM_COLOR);
	AEGfxSetBlendMode(AE_GFX_BM_BLEND);
	AEGfxSetTransparency(1.0f);

	// Determine which tile the player center is in
	int playerIdx = WorldToCell(playerPos.x, playerPos.y);
	int playerRow = -1;
	int playerCol = -1;

	if (playerIdx >= 0)
	{
		playerCol = playerIdx % width;
		playerRow = playerIdx / width;
	}

	// Compute top-left world coordinate of the grid (accounting for padding)
	float left = AEGfxGetWinMinX() + static_cast<float>(pad.left);
	float top = AEGfxGetWinMaxY() - static_cast<float>(pad.top);

	// Render player tiles 2 (take into account scaling)
	float halfW = playerScale.x *0.8f*0.5f / 2.0f;
	float halfH = playerScale.y* 0.8f * 0.5f / 2.0f;

	// Compute tile indices for the four corners of the player's bounding box
	int idxTL = WorldToCell(playerPos.x - halfW, playerPos.y + halfH);	// Top Left Corner
	int idxTR = WorldToCell(playerPos.x + halfW, playerPos.y + halfH);	// Top Right Corner
	int idxBL = WorldToCell(playerPos.x - halfW, playerPos.y - halfH);	// Bottom Left corner
	int idxBR = WorldToCell(playerPos.x + halfW, playerPos.y - halfH);	// Bottom Right corner

	// Track min/max rows and columns touched by the player
	int minRow = height, maxRow = -1;
	int minCol = width, maxCol = -1;

	// Update bounds
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

	// Highlight all tiles intersecting the player's bounding box
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

	// Restore previous render mode (Tbh this should be done for most functions but wtv)
	AEGfxSetRenderMode(prevRender);
}

// Retrieve all TileType pointers allowed in a biome.
std::vector<TileType const*> Grid::GetTilesFromBiome(std::string const& biome)
{
	return tileDB.GetTilesFromBiome(biome);
}

// Retrieve texture path for biome.
std::string Grid::GetPathNameBiome(std::string const& biome)
{
	return tileDB.GetTexturePath(biome);
}
