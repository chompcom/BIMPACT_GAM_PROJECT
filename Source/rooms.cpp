#include "rooms.h"
#include <random>
#include <cstdlib>
#include "AEEngine.h"

#include <algorithm>	// sort
#include <cctype>		// tolower
#include <Windows.h>	// Windows

#include "Sprite.h"		// TexturedSpirte for rendering room bg
#include "Enemy.h"
#include "Loaders/DataLoader.h"
#include "Collision.h"
#include "Behaviour/Behaviours.h"

#include "RoomData.h"
#include "Gift.h"		// ????
#include "Boss.h"
//#include "Grid.h"

namespace Config {
	// We are making an n x n grid with 1 and 0s
	static const int minGrid = 3, maxGrid = 4;

	// Legacy File System Scanning (Perhaps rework in a utils class?)
	void ScanPngFolderWin32(std::string const& folder, std::vector<std::string>& outList)
	{
		outList.clear();	// Clear Lists

		// Pattern Example: Assets\Rooms\Normal\*.png
		std::string pattern = folder;
		if (!pattern.empty() && pattern.back() != '\\' && pattern.back() != '/')
			pattern += "\\";
		pattern += "*.png";

		// Idk how this works truly winapi stuff
		WIN32_FIND_DATAA data{};
		HANDLE hFind = FindFirstFileA(pattern.c_str(), &data);
		if (hFind == INVALID_HANDLE_VALUE)
			return;

		do
		{
			if (data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)	// Skip directories, we are looking for files
				continue;

			// Full path to file
			std::string fullPath = folder;
			if (!fullPath.empty() && fullPath.back() != '\\' && fullPath.back() != '/')
				fullPath += "\\";
			fullPath += data.cFileName;

			outList.push_back(fullPath);	// Accumulate Files
		} while (FindNextFileA(hFind, &data));

		FindClose(hFind);

		std::sort(outList.begin(), outList.end());
	}


	void ScanCsvFolderWin32(std::string const& folder, std::string patternAppend, std::vector<std::string>& outList)
	{
		outList.clear();	// Clear Lists

		// Pattern Example: Assets\Rooms\Normal\*.png
		std::string pattern = folder;
		if (!pattern.empty() && pattern.back() != '\\' && pattern.back() != '/')
			pattern += "\\";
		pattern += patternAppend;

		// Idk how this works truly winapi stuff
		WIN32_FIND_DATAA data{};
		HANDLE hFind = FindFirstFileA(pattern.c_str(), &data);
		if (hFind == INVALID_HANDLE_VALUE)
			return;

		do
		{
			if (data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)	// Skip directories, we are looking for files
				continue;

			// Full path to file
			std::string fullPath = folder;
			if (!fullPath.empty() && fullPath.back() != '\\' && fullPath.back() != '/')
				fullPath += "\\";
			fullPath += data.cFileName;

			outList.push_back(fullPath);	// Accumulate Files
		} while (FindNextFileA(hFind, &data));

		FindClose(hFind);

		std::sort(outList.begin(), outList.end());
	}


	std::string ChooseRandomRoomCsv(std::string const& biome) {
		// Load csv files
		std::vector<std::string> files;

		const char* filePath = "./Assets/Levels/Room_Data/";
		Config::ScanCsvFolderWin32(filePath, (biome + "*.csv"), files);

		int idx = rand() % static_cast<int>(files.size());
		return files[idx];
	}

	// Absolute
	static float AbsF(float v) { return (v < 0.0f) ? -v : v; }

	// Overlap Algo rectangle
	static bool OverlapAabb(Vector2 aCenter, Vector2 aHalf, Vector2 bCenter, Vector2 bHalf)
	{
		float dx = AbsF(aCenter.x - bCenter.x);
		float dy = AbsF(aCenter.y - bCenter.y);
		return (dx <= (aHalf.x + bHalf.x)) && (dy <= (aHalf.y + bHalf.y));
	}

	// Extracting filename from fullpath
	std::string ExtractThemeTag(std::string const& fullPath, mapRooms::RoomType type)
	{
		if (type == mapRooms::RoomType::Boss) return "Boss";
		if (fullPath.empty()) return "Default";

		std::string fileName = ExtractFileName(fullPath);	// Extract File name only
		
		
		size_t underScore = fileName.find_last_of('_');		// Get position of underscore
		if (underScore != std::string::npos) fileName = fileName.substr(0, underScore);


		if (fileName.empty()) return "Default";
		return fileName;
	}
}

// Is this still needed lmao
EnemyType somethingelse{"rock",100,10,{"sad"},{"happy"},{"sad"}};

namespace mapRooms
{
    Room::Room(RoomType roomType) :
        left{ nullptr }, right{ nullptr }, up{ nullptr }, down{ nullptr },
        rmType{ roomType },
		visited{ false },
		themeTag{ "Default" },
		toBeTransferred{nullptr},
		currentRoomData{}
    {}
	Room::~Room() {

		for (Enemy* i : currentRoomData.enemyList){
			delete i;
			i = nullptr;
		}
		currentRoomData.enemyList.clear();

		for (Gift* g : currentRoomData.giftList) {
			delete g;
			g = nullptr;
		}
		currentRoomData.giftList.clear();
		for (Projectile* p : currentRoomData.projectileList) {
			delete p;
			p = nullptr;
		}
		currentRoomData.projectileList.clear();

		if (currentRoomData.boss) {
			delete currentRoomData.boss;
			currentRoomData.boss = nullptr;
		}
	}
	

	void Room::Init(RoomType roomType) {

		//rmType = RoomType::Normal;

		std::cout << static_cast<int>(this->rmType) << '\n';

		if (this->rmType == RoomType::Empty) {
			this->rmType = roomType;
		}

		// For starting Rooms Init
		if (this->rmType == RoomType::Start) {
			biome = "Start";
			this->layoutFile = Config::ChooseRandomRoomCsv(biome);
			//this->layoutFile = "Assets\"
			this->roomGrid.LoadRoomCSV(this->layoutFile);
			PatchDoorCells();

			//Gift* gift = new Gift("trash", { "Gross" }, DataLoader::CreateTexture("Assets/Gifts/trash.png"), DataLoader::CreateTexture("Assets/shadow.png"));
			//Gift* sauce = new Gift("sauce", { "Hot", "Spicy"}, DataLoader::CreateTexture("Assets/Gifts/hotsauce.png"), DataLoader::CreateTexture("Assets/shadow.png"));
			//Gift* spray = new Gift("spray", { "Wet", "Clean"}, DataLoader::CreateTexture("Assets/Gifts/spray.png"), DataLoader::CreateTexture("Assets/shadow.png"));

			Gift* gift = new Gift(allGiftTypes["Trash"], DataLoader::CreateTexture("Assets/shadow.png"));
			Gift* sauce = new Gift(allGiftTypes["HotSauce"], DataLoader::CreateTexture("Assets/shadow.png"));
			Gift* spray = new Gift(allGiftTypes["Spray"], DataLoader::CreateTexture("Assets/shadow.png"));

			gift->shadow.position = Vector2{ 0.f, -40.f };
			sauce->shadow.position = Vector2{ 0.f, -40.f };
			spray->shadow.position = Vector2{ 0.f, -40.f };

			gift->position = Vector2{ -300, 0 };
			sauce->position = Vector2{ 300,0 };
			spray->position = Vector2{ 0,-200 };

			currentRoomData.giftList.push_back(gift);
			currentRoomData.giftList.push_back(sauce);
			currentRoomData.giftList.push_back(spray);


		}


		// Normal Room Init
		if (rmType == RoomType::Normal) {

			// 1. Choose biome / layout
			std::vector<std::string> biomeNames = Grid::GetAllBiomes();

			if (!biomeNames.empty()) {
				int randomBiomeIndex = std::rand() % static_cast<int>(biomeNames.size());
				//biome = biomeNames[randomBiomeIndex];
				biome = biomeNames[randomBiomeIndex] == "Start" ? "Normal" : biomeNames[randomBiomeIndex];
			}
			else {
				biome = "Normal";
			}

			layoutFile = Config::ChooseRandomRoomCsv(biome);
			roomGrid.LoadRoomCSV(layoutFile);
			themeTag = biome;
			PatchDoorCells();

			// 2. Marker helper lambdas used only in Init
			auto isGenericEnemyMarker = [](int tileId) { return tileId == 200; };
			auto isSpecificEnemyMarker = [](int tileId) { return tileId >= 201 && tileId <= 499; };
			auto isGenericGiftMarker = [](int tileId) { return tileId == 500; };
			auto isSpecificGiftMarker = [](int tileId) { return tileId >= 501 && tileId <= 799; };

			auto findRandomSpecificMarkerForBiome = [this, &isSpecificEnemyMarker, &isSpecificGiftMarker](std::string const& spawnCategory) -> TileType const*
				{
					std::vector<TileType const*> biomeTiles = Grid::GetTilesFromBiome(biome);
					std::vector<TileType const*> validMarkers{};

					for (TileType const* currentTileType : biomeTiles)
					{
						if (!currentTileType) continue;
						if (currentTileType->spawnCategory != spawnCategory) continue;

						if (spawnCategory == "enemy" && !isSpecificEnemyMarker(currentTileType->id)) continue;
						if (spawnCategory == "gift" && !isSpecificGiftMarker(currentTileType->id)) continue;
						if (currentTileType->spawnName.empty()) continue;

						validMarkers.push_back(currentTileType);
					}

					if (validMarkers.empty()) return nullptr;

					int randomMarkerIndex = std::rand() % static_cast<int>(validMarkers.size());
					return validMarkers[randomMarkerIndex];
				};

			auto createEnemyByName = [this](std::string const& enemyName, Vector2 spawnPosition)
				{
					EnemyType const& enemyType = DataLoader::GetEnemyType(enemyName);

					Enemy* enemy = new Enemy(
						enemyType,
						DataLoader::CreateTexture(enemyType.spritePath),
						DataLoader::CreateTexture("Assets/shadow.png")
					);

					enemy->sprite.position = spawnPosition;
					enemy->shadow.position = spawnPosition - Vector2{ 0.0f, 35.0f };
					enemy->sprite.UpdateTransform();
					enemy->shadow.UpdateTransform();

					enemy->ChangeState(EnemyStates::ES_NEUTRAL);
					enemy->roomData = &currentRoomData;

					currentRoomData.enemyList.push_back(enemy);
				};

			auto createGiftFromMarker = [this](TileType const& giftMarkerTile, Vector2 spawnPosition)
				{

					std::string giftDisplayName = giftMarkerTile.spawnName.empty() ? giftMarkerTile.name : giftMarkerTile.spawnName;

					std::string giftTexturePath = giftMarkerTile.asset.empty() ? "Assets/default.png" : giftMarkerTile.asset;

					Labels giftTraits{};

					// Idk how should i set traits
					//if (giftDisplayName == "Spray")
					//{
					//	giftTraits.insert("Clean");
					//}
					//else
					//{
					//	// HotSauce / Trash default to Gross for now
					//	giftTraits.insert("Gross");
					//}

					/*Gift* gift = new Gift(
						giftDisplayName,
						giftTraits,
						DataLoader::CreateTexture(giftTexturePath),
						DataLoader::CreateTexture("Assets/shadow.png"),
						spawnPosition
					);*/


					Gift* gift = new Gift(
						allGiftTypes[giftDisplayName],
						DataLoader::CreateTexture("Assets/shadow.png"),
						spawnPosition
					);

					gift->position = spawnPosition;
					gift->giftType.sprite.position = spawnPosition;
					gift->shadow.position = spawnPosition - Vector2{ 0.0f, 40.0f };
					gift->giftType.sprite.UpdateTransform();
					gift->shadow.UpdateTransform();

					currentRoomData.giftList.push_back(gift);
				};


			// [STEP 3]: Scan room grid and spawn by marker
			for (int row = 0; row < roomGrid.GetHeight(); ++row)
			{
				for (int column = 0; column < roomGrid.GetWidth(); ++column)
				{
					// IMPORTANT:
					// Current GetCell behaves like GetCell(x, y),
					// so use (column, row) here.
					int currentTileId = roomGrid.GetCell(column, row);
					Vector2 spawnPosition = roomGrid.CellToWorldCenter(row, column);	// Set to world coordinates spawn?

					if (isGenericEnemyMarker(currentTileId))
					{
						TileType const* randomEnemyMarker = findRandomSpecificMarkerForBiome("enemy");

						if (randomEnemyMarker && !randomEnemyMarker->spawnName.empty())
						{
							createEnemyByName(randomEnemyMarker->spawnName, spawnPosition);
						}

						roomGrid.SetCell(row, column, 0);
					}
					else if (isSpecificEnemyMarker(currentTileId))
					{
						TileType const* currentTileType = Grid::QueryTileType(currentTileId);

						if (currentTileType && !currentTileType->spawnName.empty())
						{
							createEnemyByName(currentTileType->spawnName, spawnPosition);
						}

						roomGrid.SetCell(row, column, 0);
					}
					else if (isGenericGiftMarker(currentTileId))
					{
						TileType const* randomGiftMarker =
							findRandomSpecificMarkerForBiome("gift");

						if (randomGiftMarker && !randomGiftMarker->spawnName.empty())
						{
							createGiftFromMarker(*randomGiftMarker, spawnPosition);
						}

						roomGrid.SetCell(row, column, 0);
					}
					else if (isSpecificGiftMarker(currentTileId))
					{
						TileType const* currentTileType = Grid::QueryTileType(currentTileId);

						if (currentTileType && !currentTileType->spawnName.empty())
						{
							createGiftFromMarker(*currentTileType, spawnPosition);
						}

						roomGrid.SetCell(row, column, 0);
					}
				}
			}
		}


			std::cout << "Display Room Init of " << biome << "\n";
			//roomGrid.
			//for (int j = 0; j < roomGrid.GetHeight(); j++) {
			//	for (int i = 0; i < roomGrid.GetWidth(); i++) {
			//		const TileType* tile = Grid::QueryTileType(roomGrid.GetCell(j, i));
			//		std::cout << (tile ? tile->id : 0) << " ";
			//		if (!tile) continue;
			//		if (tile->id > 101) {
			//			//std::cout << "Spawned " << tile->name << "! \n";
			//			const EnemyType& enemyType = DataLoader::GetEnemyType(tile->name);
			//			currentRoomData.enemyList.push_back(new Enemy(enemyType, DataLoader::CreateTexture(enemyType.spritePath), DataLoader::CreateTexture("Assets/shadow.png")));
			//			currentRoomData.enemyList.back()->sprite.position = Vector2{ i * roomGrid.GetTileWidth()*0.5f , j * roomGrid.GetTileHeight()*0.5f};

			//		}
			//	}
			//	std::cout << "\n";
			//}

			// Spawn enemies
			if (0 && currentRoomData.enemyList.empty()) {
				currentRoomData.enemyList.push_back(new Enemy(DataLoader::GetEnemyType("Booger"), DataLoader::CreateTexture("Assets/Enemies/booger.png"), DataLoader::CreateTexture("Assets/shadow.png")));
				for (Enemy* i : currentRoomData.enemyList) {
					i->shadow.position = Vector2{ 0.f, -35.f };
					i->shadow.UpdateTransform();
					i->ChangeState(EnemyStates::ES_NEUTRAL);
					i->roomData = &this->currentRoomData;
				}
			}

			// Spawn gifts
			//if (0 && currentRoomData.giftList.empty()) {
			//	Gift* gift = new Gift("boat", { "Gross" }, DataLoader::CreateTexture("Assets/pattyfish.png"), DataLoader::CreateTexture("Assets/shadow.png"));
			//	gift->shadow.position = Vector2{ 0.f, -40.f };
			//	currentRoomData.giftList.push_back(gift);
			//}
		//}
			// Gifts here (1 Gift per room for now)
			//currentRoomData.giftList.push_back(new Gift{ "boat", {"happy"}, Sprite() });	// Does this make sense?
			//Vector2 giftPos{ 200.0f, 450.0f };
			//TexturedSprite giftSprite(somemesh, giftPos, Vector2{ 80.0f, 80.0f }, Color{ 1.f, 0.f, 0.f, 1.f });
			//currentRoomData.giftList.push_back(new Gift(somethingelse, DataLoader::CreateTexture("Assets/poprocks.png")));
			
		

		// Boss Init
		if (rmType == RoomType::Boss) {
			std::vector<AttackData>attackData = { {5.0f, 2.0f, 3.0f, 1.5f}, {10.0f, 2.0f, 1.5f, 2.0f}, {5.0f, 1.5f, 4.0f, 1.5f} };
			currentRoomData.boss = new Boss("Boss 1", 100.0f, 5.0f, DataLoader::CreateTexture("Assets/veggiefish.png"), DataLoader::CreateTexture("Assets/shadow.png"), currentRoomData, attackData);
			currentRoomData.boss->sprite.scale = Vector2{ 100.0f, 100.0f };
			currentRoomData.boss->shadow.position = Vector2{ 0.f, -35.f };

			// Initialize Default Grid (Should be all zero ig)
			biome = "Normal"; // or keep previously chosen biome if you want
			layoutFile = ".\\Assets\\Levels\\Room_Data\\Boss_1.csv";
			//layoutFile = ChooseRandomBossRoomCsv();	// Maybe if required
			this->roomGrid.LoadRoomCSV(layoutFile);
			PatchDoorCells();
		}
		
		//Setting grid to roomGrid
		currentRoomData.grid = roomGrid;

	} 
	void Room::Update(float dt) {
		//for (Enemy* i : currentRoomData.enemyList) {
			//i->Update(dt);


		//}

		// Check collision with room ig
		



	}


	void Room::PatchDoorCells()
	{
		const int DOOR_TILE = 100;
		const int DOOR_W = 2;
		const int DOOR_H = 2;

		int width = this->roomGrid.GetWidth();
		int height = this->roomGrid.GetHeight();

		// Center the door patch on each wall
		int doorStartCol = (width - DOOR_W) / 2;
		int doorStartRow = (height - DOOR_H) / 2;

		if (up)
		{
			for (int r = 0; r < DOOR_H; ++r)
			{
				for (int c = doorStartCol; c < doorStartCol + DOOR_W; ++c)
				{
					roomGrid.SetCell(r, c, DOOR_TILE);
				}
			}
		}

		if (down)
		{
			for (int r = height - DOOR_H; r < height; ++r)
			{
				for (int c = doorStartCol; c < doorStartCol + DOOR_W; ++c)
				{
					roomGrid.SetCell(r, c, DOOR_TILE);
				}
			}
		}

		if (left)
		{
			for (int r = doorStartRow; r < doorStartRow + DOOR_H; ++r)
			{
				for (int c = 0; c < DOOR_W; ++c)
				{
					roomGrid.SetCell(r, c, DOOR_TILE);
				}
			}
		}

		if (right)
		{
			for (int r = doorStartRow; r < doorStartRow + DOOR_H; ++r)
			{
				for (int c = width - DOOR_W; c < width; ++c)
				{
					roomGrid.SetCell(r, c, DOOR_TILE);
				}
			}
		}
	}



	Map::Map() :
		gridSize{ 0 },
		rooms{},
		currentRoom{ nullptr },
		rngState{ 0xA341316Cu },
		transferData{nullptr}	// Some random seed
	{
		//Room x[2]{};
		//for (Room y : x) {
			//rooms.push_back(y);
		//}
	}

	Map::~Map(){
		DeleteMap();
	}

	// Maybe make it dynamic
	void Map::LoadRoomArtLists()
	{
		// Normal Room Files
		biomeRoomFiles.clear();
		normalRoomFiles.clear();
		bossRoomFiles.clear();

		//Config::ScanPngFolderWin32(normalDir, normalRoomFiles);
		std::string const normalDir = "Assets/Rooms/Normal/NORMAL";
		std::string const greenDir	= "Assets/Rooms/Normal/GREEN";
		std::string const iceDir	= "Assets/Rooms/Normal/ICE";
		std::string const bossDir	= "Assets/Rooms/Boss";
		std::string const oceanDir	= "Assets/Rooms/Normal/OCEAN";

		std::string filePath = "Assets/Levels/Room_Data/TilesInfo.json";
		Json::Value tilesInfo = DataLoader::LoadJsonFile(filePath);

		std::string cliInput = "powershell.exe -c \"Test-Path ([System.IO.Path]::Combine($PWD.Path, '" + filePath + "'))\"";
		system(cliInput.c_str());	// Look and see if yo shit is FALSE, that means it doesn't exist on that path



		Json::Value::Members biomeNames = tilesInfo["biomes"].getMemberNames();
		for (Json::String name : biomeNames) {
			std::string imagePath = tilesInfo["biomes"][name]["imageDir"].asString();
			Config::ScanPngFolderWin32( imagePath, biomeRoomFiles[name]);
		}
		/*
		Config::ScanPngFolderWin32(normalDir,	biomeRoomFiles["Start"]);
		Config::ScanPngFolderWin32(normalDir,	biomeRoomFiles["Normal"]);
		Config::ScanPngFolderWin32(greenDir,	biomeRoomFiles["Green"]);
		Config::ScanPngFolderWin32(iceDir,		biomeRoomFiles["Ice"]);
		Config::ScanPngFolderWin32(oceanDir,    biomeRoomFiles["Ocean"]);
		*/

		Config::ScanPngFolderWin32(bossDir,		bossRoomFiles);

		// Not up yet... There should be a neater way via json lol
		//Config::ScanPngFolderWin32("Assets/Rooms/Boss", bossRoomFiles);
	}

	AEGfxTexture* Map::GetOrLoadTexture(std::string const& path)
	{
		// If already loaded, reuse it
		auto it = textureCache.find(path);
		if (it != textureCache.end()) return it->second;

		// Load new texture if not found ig
		AEGfxTexture* tex = AEGfxTextureLoad(path.c_str());
		textureCache[path] = tex; // cache even if nullptr (prevents repeated attempts)
		return tex;
	}

	// Asset Code was here
	void Map::AssignRoomArt() {
		//bool haveNormal = !normalRoomFiles.empty();
		bool haveNormal = !biomeRoomFiles.empty();
		bool haveBoss = !bossRoomFiles.empty();

		for (int y = 0; y < gridSize; ++y)
		{
			for (int x = 0; x < gridSize; ++x)
			{
				Room* rm = GetRoom(x, y);
				// Skip empty rooms lol
				if (!rm || rm->rmType == RoomType::Empty)
					continue;

				std::string chosenPath;

				if (rm->rmType == RoomType::Boss)
				{
					if (haveBoss) {
						int pick = static_cast<int>(RandInt(0, static_cast<int>(bossRoomFiles.size()) - 1));
						chosenPath = bossRoomFiles[pick];
					}
					else continue;
				}
				else if (rm->rmType == RoomType::Normal || rm->rmType == RoomType::Start) {
					if (haveNormal) {
						/*int pick = static_cast<int>(RandInt(0, static_cast<int>(normalRoomFiles.size()) - 1));
						chosenPath = normalRoomFiles[pick];*/

						auto it = biomeRoomFiles.find(rm->biome);	// Get biome detail
						if (it != biomeRoomFiles.end() && !it->second.empty()) {
							int pick = static_cast<int>(RandInt(0, static_cast<int>(it->second.size()) - 1));
							chosenPath = it->second[pick];
						}
						else continue;  // Doubt this will happen but anyways
					}


				}

				rm->roomTexturePath		= chosenPath;
				rm->roomTexture			= (chosenPath.empty() ? nullptr : GetOrLoadTexture(chosenPath));
				rm->themeTag			= Config::ExtractThemeTag(chosenPath, rm->rmType); // hopefully this works

			}
		}
	}
		
	
	//InitMap(): Allocate / Reset Maps (Rooms)
	void Map::InitMap(RoomData &globalSceneData, unsigned int seed) {
		rngState = seed;
		srand(rngState);
		gridSize = RandInt(Config::minGrid, Config::maxGrid);


		rooms.clear();
		rooms.resize(gridSize * gridSize);	// Grid generated, calls upon default constructor (no init rooms yet)

		transferData = &globalSceneData;

		//somemesh = CreateSquareMesh();		// Remember to unfuck this
		ResetRooms();							// Ensure rooms are nothing;
		GenerateRooms();						// Generate, links and init rooms.
		// Probably generate other room types???


		//std::cout << rooms.size();
		//for (Room& curRoom : rooms) {
		//	std::cout << static_cast<int>(curRoom.rmType);
		//}

		// Load available Pngs
		LoadRoomArtLists();
		AssignRoomArt();

		doorTex = GetOrLoadTexture("Assets/Door/door.png");

		// Somehow assign startX and startY into starting room coordinates value
		currentRoom = GetRoom(startX, startY);

		if (currentRoom!=nullptr) {
			currentRoom->toBeTransferred = transferData;
			currentRoom->currentRoomData.player = transferData ? transferData->player : nullptr;
		}

		doorCooldown = 0.0f;


	}

	bool Map::InBounds(int x, int y) const
	{
		return (x >= 0 && x < gridSize && y >= 0 && y < gridSize);
	}

	void Map::ResetRooms() {
		for (int i = 0; i < gridSize * gridSize; ++i) {
			rooms[i] = Room(RoomType::Empty);
		}
	}

	int Map::GetRoomIdx(int x, int y) const
	{
		return ((y * gridSize) + x);
	}

	unsigned int Map::RandInt(int low, int high) {
		return (static_cast<unsigned int>(low + (std::rand() % (high - low + 1))));
	}

	//// UpdateMap()
	//void Map::UpdateMap(  )
	//{
	//	// Input (Get x and y of player)
	//	// Perhaps testing for collision
	//	

	//	MoveTo(Direction::Left);	// If collide with door area, use MoveTo(Direction direcci�n)
	//}

	// DeleteMap()?
	void Map::DeleteMap() 
	{
		rooms.clear();			// Clear room information
		gridSize = 0;			// Set gridsize to zero
		currentRoom = nullptr;	// Deference room item
		doorCooldown = 0.0f;	// Cooldown thingy

		for (auto& kv : textureCache) {
			if (kv.second) AEGfxTextureUnload(kv.second);
		}
		textureCache.clear();
		normalRoomFiles.clear();
		bossRoomFiles.clear();


		// Help clear globaldata
		// Actually don't we need it between different state levels
		// Perhaps before unload, we serialize this and save it in some save file before loading in a new level. It will be only erased if user fails the level.
	}

	void Map::LinkRooms(Room* a, Room* b, Direction dirFromAToB)
	{
		if (dirFromAToB == Direction::Right) { a->right = b; b->left = a; }
		if (dirFromAToB == Direction::Left) { a->left = b; b->right = a; }
		if (dirFromAToB == Direction::Up) { a->up = b; b->down = a; }
		if (dirFromAToB == Direction::Down) { a->down = b; b->up = a; }
	}

	// Simple generation using random pathing. Alternative would be randomized DFS. Computationally intensive however.
	// https://en.wikipedia.org/wiki/File:Depth-First_Search_Animation.ogv explains how DFS works
	void Map::GenerateRooms() {

		int MinRooms = min(4, gridSize * gridSize - 1);
		int MaxRooms = max(4, gridSize * gridSize - 1);

		int roomsVisit = RandInt(MinRooms, MaxRooms);	// Set How many rooms to make available
		
		startX =		RandInt(0, gridSize - 1), startY = RandInt(0, gridSize - 1);
		int startIdx =	GetRoomIdx(startX, startY);

		// DFS Tracking for paths
		std::vector<bool> visited(gridSize * gridSize, false); // Init array of n x n size to false
		std::vector<int> stack, visitedIndices;


		visited[startIdx] = true;	// Mark start as visited
		int visitedCount = 1;
		stack.push_back(startIdx);
		visitedIndices.push_back(startIdx);
		GetRoom(startX, startY)->rmType = RoomType::Start;

		// Random DFS until roomVisits reached or out of expansion
		while (!stack.empty() && visitedCount < roomsVisit) {
			int currentIdx = stack.back();	// Get current Room
			int curX = currentIdx % gridSize, curY = currentIdx / gridSize;	// Get x, y value of 2d room array
			

			int neighbourIdx[4]{}, neighbourX[4]{}, neighbourY[4]{}, neighCount{};

			// Go Left
			if (InBounds(curX - 1, curY))
			{
				int idxRoom = GetRoomIdx(curX - 1, curY);	
				if (!visited[idxRoom]) {
					neighbourIdx[neighCount] = idxRoom;
					neighbourX[neighCount] = -1;
					neighbourY[neighCount] = 0;
					++neighCount;
				}
			}

			// Go Right
			if (InBounds(curX + 1, curY))
			{
				int idxRoom = GetRoomIdx(curX + 1, curY);
				if (!visited[idxRoom]) {
					neighbourIdx[neighCount] = idxRoom;
					neighbourX[neighCount] = 1;
					neighbourY[neighCount] = 0;
					++neighCount;
				}
			}

			// Go Up
			if (InBounds(curX, curY - 1))
			{
				int idxRoom = GetRoomIdx(curX, curY - 1);
				if (!visited[idxRoom]) {
					neighbourIdx[neighCount] = idxRoom;
					neighbourX[neighCount] = 0;
					neighbourY[neighCount] = -1;
					++neighCount;
				}
			}

			// Go Down
			if (InBounds(curX, curY + 1))
			{
				int idxRoom = GetRoomIdx(curX, curY + 1);
				if (!visited[idxRoom]) {
					neighbourIdx[neighCount] = idxRoom;
					neighbourX[neighCount] = 0;
					neighbourY[neighCount] = 1;
					++neighCount;
				}
			}


			// No neighbours somehow
			if (neighCount == 0) {
				stack.pop_back();
				continue;
			}

			// Randomly pick 1 neighbour for next iteration
			int pick = RandInt(0, neighCount - 1);
			int nextIdx = neighbourIdx[pick];
			int nextX = nextIdx % gridSize, nextY = nextIdx / gridSize;
			
			Room* curRoom  = GetRoom(curX, curY);
			Room* nextRoom = GetRoom(nextX, nextY);

			// Link current room to next Room
			Direction linkDirection = Direction::Left;

			if (neighbourX[pick] == -1) {
				linkDirection = Direction::Left;
			}
			else if (neighbourX[pick] == 1) {
				linkDirection = Direction::Right;
			}
			else if (neighbourY[pick] == -1) {
				linkDirection = Direction::Up;
			}
			else if (neighbourY[pick] == 1) {
				linkDirection = Direction::Down;
			}

			// Link Rooms
			LinkRooms(curRoom, nextRoom, linkDirection);
			

			// Mark Visited
			visited[nextIdx] = true;
			stack.push_back(nextIdx);			// Track Depth in neighbours
			visitedIndices.push_back(nextIdx);	// Track visited indexes
			++visitedCount;

			// Mark next room for exist
			if (nextRoom != nullptr) {
				//nextRoom->Init(); // move this after everything? if room != empty
				nextRoom->rmType = RoomType::Normal;
			}



		}

		
		// End Boss for Level
		startIdx = GetRoomIdx(startX, startY);
		std::vector<int> deadEnds;
		deadEnds.reserve(visitedIndices.size());

		for (int idx : visitedIndices) {
			if (idx == startIdx) continue;

			Room const& r = rooms[idx];
			int links = 0;
			if (r.left!=nullptr) ++links;
			if (r.right != nullptr) ++links;
			if (r.up != nullptr) ++links;
			if (r.down!= nullptr) ++links;

			if (links == 1) deadEnds.push_back(idx);
		}

		int bossIdx{startIdx};

		if (!deadEnds.empty()) {
			bossIdx = deadEnds.at(RandInt(0, static_cast<int>(deadEnds.size() - 1)));
		}
		else if (visitedIndices.size() > 1) {	// No dead ends
			bossIdx = visitedIndices.back();
			if (bossIdx == startIdx) bossIdx = visitedIndices.at(1);
		}

		// Mark Boss
		GetRoom(bossIdx% gridSize, bossIdx / gridSize)->rmType = RoomType::Boss;

		// Init all rooms now;
		//for (Room &curRoom : this->rooms) {
		//	if (curRoom.rmType != RoomType::Empty) {
		//		curRoom.Init(curRoom.rmType);	// init room here
		//	}
		//}

		// Link all Adjacent Rooms here
		for (int idx = 0; idx < (gridSize * gridSize); ++idx) {
			int curX = idx % gridSize, curY = idx / gridSize;	// Get x, y value of 2d room array
			Room* curRoom = GetRoom(curX, curY);

			if (!visited[idx] || curRoom->rmType == RoomType::Empty) continue; // HAHAH FORGOT ABOUT THIS FOR VOID ROOMS...

			// Check Left
			if (InBounds(curX - 1, curY))
			{
				int idxRoom		= GetRoomIdx(curX - 1, curY);
				Room* neighbour = GetRoom(curX - 1, curY);
				if (visited[idxRoom] && (curRoom->left == nullptr || neighbour->right == nullptr) && neighbour->rmType != RoomType::Empty) LinkRooms(curRoom, neighbour, Direction::Left);
			}

			// Check Right
			if (InBounds(curX + 1, curY))
			{
				int idxRoom		= GetRoomIdx(curX + 1, curY);
				Room* neighbour = GetRoom(curX + 1, curY);
				if (visited[idxRoom] && (curRoom->right == nullptr || neighbour->left == nullptr) && neighbour->rmType != RoomType::Empty) LinkRooms(curRoom, neighbour, Direction::Right);
			}

			// Check Up
			if (InBounds(curX, curY - 1))
			{
				int idxRoom		= GetRoomIdx(curX, curY - 1);
				Room* neighbour = GetRoom(curX, curY - 1);
				if (visited[idxRoom] && (curRoom->up == nullptr || neighbour->down == nullptr) && neighbour->rmType != RoomType::Empty) LinkRooms(curRoom, neighbour, Direction::Up);
			}

			// Check Down
			if (InBounds(curX, curY + 1))
			{
				int idxRoom		= GetRoomIdx(curX, curY + 1);
				Room* neighbour = GetRoom(curX, curY + 1);
				if (visited[idxRoom] && (curRoom->down == nullptr || neighbour->up == nullptr) && neighbour->rmType != RoomType::Empty) LinkRooms(curRoom, neighbour, Direction::Down);
			}
		}

		// Init Rooms here
		for (int idx = 0; idx < (gridSize * gridSize); ++idx) {
			if (visited[idx]) GetRoom(idx % gridSize, idx / gridSize)->Init(GetRoom(idx % gridSize, idx / gridSize)->rmType);
		}

	}

	int Map::GetGridSize() const {
		return gridSize;
	}

	

	void Map::RenderCurrentRoom(AEGfxVertexList* squaremesh) const {
		if (!currentRoom || !squaremesh || !currentRoom->roomTexture) return;	// possibly skip empty textures?

		float minX = AEGfxGetWinMinX();
		float maxX = AEGfxGetWinMaxX();
		float minY = AEGfxGetWinMinY();
		float maxY = AEGfxGetWinMaxY();

		float winW = (maxX - minX);
		float winH = (maxY - minY);

		AEGfxSetRenderMode(AE_GFX_RM_TEXTURE);
		AEGfxSetBlendMode(AEGfxBlendMode::AE_GFX_BM_BLEND);
		AEGfxSetTransparency(1.0f);

		// Render Room Background
		TexturedSprite bg(
			squaremesh,
			currentRoom->roomTexture,
			Vector2{ 0.0f, 0.0f },                  // centered
			Vector2{ winW, winH },                  // fill screen
			Color{ 1.0f, 1.0f, 1.0f, 1.0f }			// no tint
		);
		bg.RenderSprite();

		// Render Room Doors
		//GetOrLoadTexture()
		//RenderDoorsSimple(squaremesh);

		//TexturedSprite dr = DataLoader::CreateTexture("Assets/Rooms/Door/door.png");	// Does this require freeing
		
		// Render Room Obstacles
		
		
		// Render Room Doors
		RenderRoomDoors(squaremesh, doorTex);


		AEGfxSetRenderMode(AE_GFX_RM_TEXTURE);
		// Render Room Obstacles
		if (currentRoom != nullptr) {
			Grid const& grid = currentRoom->roomGrid;
			for (int row = 0; row < grid.GetHeight(); ++row) {
				for (int col = 0; col < grid.GetWidth(); ++col) {

					int tileId = grid.GetCell(col, row);
					if (tileId < 0) continue;
					if (tileId == 0) continue;     // empty
					if (tileId == 100) continue;   // door tile, no obstacle sprite

					TileType const* tile = Grid::QueryTileType(tileId);

					//std::cout << "Tile: " << tile->asset.c_str() << '\n';

					if (!tile) continue;
					if (tile->asset.empty()) continue;

					Vector2 pos = grid.CellToWorldCenter(row, col);

					TexturedSprite tex = DataLoader::CreateTexture(tile->asset);

					tex.position = pos;
					tex.scale = Vector2{ grid.GetTileWidth(), grid.GetTileHeight()};
					tex.UpdateTransform();
					tex.RenderSprite();

				}
			}
		}

		
		// Render Enemy?
		for (Enemy* i : currentRoom->currentRoomData.enemyList) {
			Color oldColor = i->sprite.color;
			i->sprite.color = { 0.5f,0.5f,0.5f,1.f };
			i->sprite.RenderSprite();
			i->sprite.color = oldColor;
		}
		
	}



	// Render map
	void Map::RenderDebugMap(AEGfxVertexList* squareMesh) const
	{
		if (!squareMesh) return;
		if (gridSize <= 0) return; // shouldn't happen ngl

		float minX = AEGfxGetWinMinX();
		float maxX = AEGfxGetWinMaxX(); UNREFERENCED_PARAMETER(maxX);
		float minY = AEGfxGetWinMinY();	UNREFERENCED_PARAMETER(minY);
		float maxY = AEGfxGetWinMaxY();

		float cell	= 18.0F;
		float gap = 4.0f;

		float startXPixel = minX + 30.0f;
		float startYPixel = maxY - 30.0f;

		AEGfxSetRenderMode(AE_GFX_RM_COLOR); // COLOR RENDERING
		AEGfxSetBlendMode(AEGfxBlendMode::AE_GFX_BM_BLEND); // Normal color blending
		AEGfxSetTransparency(1.0f); // Opaque

		for (int idx = 0; idx < (gridSize * gridSize); ++idx) {
			int curX = idx % gridSize, curY = idx / gridSize;	// Get x, y value of 2d room array
			Room const* curRoom = Map::GetRoom(curX, curY);
			Room const& rm = *(curRoom);
			
			if (rm.rmType == RoomType::Empty) continue;			// Skip void rooms obviously, given that this is a 2D Array

			Color c;

			if (rm.rmType == RoomType::Start) c = Color{ 0.2f, 0.9f, 0.2f, 0.90f };						// Green
			else if (rm.rmType == RoomType::Boss)  c = Color{ 0.9f, 0.2f, 0.2f, 0.90f };				// Red
			else c = Color{ 0.6f, 0.6f, 0.6f, 0.85f };													// Normal

			if (this->currentRoom == &(const_cast<Room&>(rm))) c = Color{ 0.2f, 0.4f, 1.0f, 0.95f };	// Blue for current room?
			else if ((rm.visited)) c = Color{ c.r * 0.5f, c.g * 0.5f , c.b * 0.5f, 0.55f };				// Dimmed color on map if room is visited

			float px = startXPixel + curX * (cell + gap) + (cell / 2.0f);								// Essentially minX + 30 + current X (e.g., 0) * (total cell space) + half cell (for center)
			float py = startYPixel - curY * (cell + gap) - (cell / 2.0f);								// Essentially minY - 30 + current X (e.g., 0) * (total cell space) - half cell (for center)

			Sprite tile(squareMesh, Vector2{ px, py }, Vector2{ cell, cell }, c);						// Convert each tile into a sprite, then render such
			tile.RenderSprite(); // Render
		}
	}

	void Map::RenderRoomDoors(AEGfxVertexList* squareMesh, AEGfxTexture* doorTexture) const
	{
		if (!squareMesh || !doorTexture || !currentRoom) return;

		// Init settings here
		AEGfxSetRenderMode(AE_GFX_RM_TEXTURE);
		AEGfxSetBlendMode(AE_GFX_BM_BLEND);
		AEGfxSetTransparency(1.0f);
		AEGfxSetColorToMultiply(1.0f, 1.0f, 1.0f, 1.0f);
		AEGfxSetColorToAdd(0.0f, 0.0f, 0.0f, 0.0f);
		AEGfxTextureSet(doorTexture, 0.0f, 0.0f);

		// Init variables
		const float winMinX = AEGfxGetWinMinX();
		const float winMaxX = AEGfxGetWinMaxX();
		const float winMinY = AEGfxGetWinMinY();
		const float winMaxY = AEGfxGetWinMaxY();

		// Size of doors
		const float doorW = 280.0f;
		const float doorH = 220.0f;
		const float edgeOffset = 35.0f;	// Offset from screen

		// Render Door Up Asset
		if (currentRoom->up)
		{
			float offsetUp = 0.75f;
			AEMtx33 scale, rot, trans, temp, finalMtx;
			AEMtx33Scale(&scale, doorW, doorH * offsetUp);
			AEMtx33Rot(&rot, 0.0f);	// Up is 0 rad
			AEMtx33Trans(&trans, 0.0f, winMaxY - edgeOffset);

			AEMtx33Concat(&temp, &rot, &scale);
			AEMtx33Concat(&finalMtx, &trans, &temp);

			AEGfxSetTransform(finalMtx.m);
			AEGfxMeshDraw(squareMesh, AE_GFX_MDM_TRIANGLES);
		}

		// Down
		if (currentRoom->down)
		{
			// DOOR is not perfecion...
			float offsetDown = 0.75f;
			AEMtx33 scale, rot, trans, temp, finalMtx;
			AEMtx33Scale(&scale, doorW, doorH * offsetDown);
			AEMtx33Rot(&rot, PI);	// Down is 180 from top = PI
			AEMtx33Trans(&trans, 0.0f, winMinY + edgeOffset);

			AEMtx33Concat(&temp, &rot, &scale);
			AEMtx33Concat(&finalMtx, &trans, &temp);

			AEGfxSetTransform(finalMtx.m);
			AEGfxMeshDraw(squareMesh, AE_GFX_MDM_TRIANGLES);
		}

		// Left
		if (currentRoom->left)
		{
			// DOOR is not perfecion...
			float offsetLeft = 0.92f;
			AEMtx33 scale, rot, trans, temp, finalMtx;
			AEMtx33Scale(&scale, doorW, doorH * offsetLeft);
			AEMtx33Rot(&rot, PI * 0.5f); // Left is 90 deg counterclockwise -> PI / 2
			AEMtx33Trans(&trans, winMinX + edgeOffset, 0.0f);

			AEMtx33Concat(&temp, &rot, &scale);
			AEMtx33Concat(&finalMtx, &trans, &temp);

			AEGfxSetTransform(finalMtx.m);
			AEGfxMeshDraw(squareMesh, AE_GFX_MDM_TRIANGLES);
		}

		// Right
		if (currentRoom->right)
		{
			// DOOR is not perfecion...
			float offsetRight = 0.99f;
			AEMtx33 scale, rot, trans, temp, finalMtx;
			AEMtx33Scale(&scale, doorW, doorH * offsetRight);
			AEMtx33Rot(&rot, -PI * 0.5f); // Right is 90 deg clockwise or 270 counterclockwise -> - (PI / 2) OR (2PI * (3/4))
			AEMtx33Trans(&trans, winMaxX - edgeOffset, 0.0f);

			// T * (R * S)
			AEMtx33Concat(&temp, &rot, &scale);
			AEMtx33Concat(&finalMtx, &trans, &temp);

			// Set Transform for rendering
			AEGfxSetTransform(finalMtx.m);
			AEGfxMeshDraw(squareMesh, AE_GFX_MDM_TRIANGLES);	// Render Door Right
		}
	}

	Room* Map::GetRoom(int x, int y) {
		size_t idx = static_cast<size_t>(GetRoomIdx(x, y));
		return &(Map::rooms[idx]);
	}

	const Room* Map::GetRoom(int x, int y) const {
		size_t idx = static_cast<size_t>(GetRoomIdx(x, y));
		return &(Map::rooms[idx]);
	}

	Room* Map::GetCurrentRoom() {
		return currentRoom;
	}

	bool Map::MoveTo(Direction direction) {

		if (!currentRoom) return false;	// should not happen just a function safeguard...

		Room* previousRoom = currentRoom;
		Room* target = nullptr;
		switch (direction) {
			case Direction::Left:  target = currentRoom->left;  break;
			case Direction::Right: target = currentRoom->right; break;
			case Direction::Up:    target = currentRoom->up;    break;
			case Direction::Down:  target = currentRoom->down;  break;
			default: break;
		}

		if (!target) return false;	// again should not happen just a function safeguard

		if (previousRoom->rmType == RoomType::Boss) previousRoom->currentRoomData.boss->ResetBoss();

		currentRoom = target; // Room changed
		StopAllAudio();
		RoomEnterAudio();
		
		previousRoom->visited = true;
		currentRoom->toBeTransferred = transferData;
		currentRoom->currentRoomData.player = transferData ? transferData->player : nullptr;	// Is this necessary lol idk 
		//currentRoom->toBeTransferred = previousRoom->toBeTransferred;	// Hopefully?
		//currentRoom->toBeTransferred = &transferData;					// Maybe this works?

		return true;
	}

	// Update Loop
	void Map::UpdateMap(Vector2& playerPos, Vector2 playerHalfSize, ParticleSystem& particleSystem, float dt)
	{
		if (!currentRoom) return;

		// Cooldown prevents immediate bounce-back after warp
		if (doorCooldown > 0.0f) {
			doorCooldown -= dt;
			if (doorCooldown < 0.0f) doorCooldown = 0.0f;
			return;
		}

		float minX = AEGfxGetWinMinX();
		float maxX = AEGfxGetWinMaxX();
		float minY = AEGfxGetWinMinY();
		float maxY = AEGfxGetWinMaxY();

		// --- Door trigger tuning knobs ---
		// span: how "wide" the doorway is (along the wall)
		// depth: how "thick" the trigger is (into the room)
		// margin: how far inside the new room we spawn after switching
		float doorSpan = 220.0f;	// Wide doorway
		float doorDepth = 80.0f;
		float warpMargin = 120.0f;	// Distance padded when entering new room

		// Trigger half-sizes
		Vector2 halfLR{ doorDepth * 0.5f, doorSpan * 0.5f }; // left/right doors
		Vector2 halfUD{ doorSpan * 0.5f, doorDepth * 0.5f }; // up/down doors

		// Door trigger centers (middle of each wall)
		Vector2 leftDoorC{ minX + doorDepth * 0.5f, 0.0f };
		Vector2 rightDoorC{ maxX - doorDepth * 0.5f, 0.0f };
		Vector2 topDoorC{ 0.0f, maxY - doorDepth * 0.5f };
		Vector2 botDoorC{ 0.0f, minY + doorDepth * 0.5f };

		// LEFT
		if (currentRoom->left && CollisionIntersection_RectRect_Static(AABB{ playerPos - playerHalfSize, playerPos + playerHalfSize },
			AABB{ leftDoorC - halfLR, leftDoorC + halfLR }))
		{
			if (MoveTo(Direction::Left)) {
				// appear at RIGHT side of the next room, inside boundary
				playerPos.x = maxX - playerHalfSize.x - warpMargin;
				doorCooldown = 0.20f;
				particleSystem.DestroyParticles();
			}
			return;
		}

		// RIGHT
		if (currentRoom->right && CollisionIntersection_RectRect_Static(AABB{ playerPos - playerHalfSize, playerPos + playerHalfSize },
			AABB{ rightDoorC - halfLR, rightDoorC + halfLR }))
		{
			if (MoveTo(Direction::Right)) {
				playerPos.x = minX + playerHalfSize.x + warpMargin;
				doorCooldown = 0.20f;
				particleSystem.DestroyParticles();
			}
			return;
		}

		// UP
		if (currentRoom->up && CollisionIntersection_RectRect_Static(AABB{ playerPos - playerHalfSize, playerPos + playerHalfSize },
			AABB{ topDoorC - halfUD, topDoorC + halfUD }))
		{
			if (MoveTo(Direction::Up)) {
				playerPos.y = minY + playerHalfSize.y + warpMargin;
				doorCooldown = 0.20f;
				particleSystem.DestroyParticles();
			}
			return;
		}

		// DOWN
		if (currentRoom->down && CollisionIntersection_RectRect_Static(AABB{ playerPos - playerHalfSize, playerPos + playerHalfSize },
			AABB{ botDoorC - halfUD, botDoorC + halfUD }))
		{
			if (MoveTo(Direction::Down)) {
				playerPos.y = maxY - playerHalfSize.y - warpMargin;
				doorCooldown = 0.20f;
				particleSystem.DestroyParticles();
			}
			return;
		}
	}

	RoomData& Map::GetTransferData()
	{
		return *transferData;
	}
}
