#include <iostream>
#include <fstream>
//#include <filesystem>

#include "../Grid.h"
#include "Ui.h"
#include "../GameStateManager.h"
#include "../Utils/Utils.h"
#include "../Sprite.h"
#include "../Loaders/DataLoader.h"
#include "../GameStateList.h"

#include "AEEngine.h"

#include <cstdlib>
#include <fstream>



enum class ContentEditorTab
{
	Biomes,
	Obstacles
};

struct ContentEditorState
{


	Grid roomGrid{};

	ContentEditorTab currentTab{ ContentEditorTab::Biomes };

	std::vector<std::string> biomeNames{};
	int selectedBiomeIndex{ 0 };

	std::vector<TileType const*> obstaclePalette{};
	int currentObstaclePalettePage{ 0 };
	int selectedTileId{ 0 };

	int hoveredCellIndex{ -1 };

	std::string exportFileName{ "Custom" };
	std::string statusMessage{ "Ready" };

	// Existing CSV edit mode
	bool isLoadPopupOpen{ false };				// For Popups
	bool isEditingExistingFile{ false };		// Are we editing existing file
	int existingCsvIndex{ 0 };					// Fileindex
	std::vector<std::string> existingCsvPaths{};// Tbh im just gonan use one path
	std::string currentEditingFilePath{};		// Current File Path to modify

	// Patch some bugs???
	bool pendingClosePopUp{ false }, pendingLoadSelectedCSV{ false };

};


void ContentEditorLoad();
void ContentEditorInit();
void ContentEditorUpdate(float dt);
void ContentEditorDraw();
void ContentEditorFree();
void ContentEditorUnload();
