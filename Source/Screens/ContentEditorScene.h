#include <iostream>
#include <fstream>

#include "../Grid.h"
#include "Ui.h"
#include "../GameStateManager.h"

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
};


void ContentEditorLoad();
void ContentEditorInit();
void ContentEditorUpdate(float dt);
void ContentEditorDraw();
void ContentEditorFree();
void ContentEditorUnload();
