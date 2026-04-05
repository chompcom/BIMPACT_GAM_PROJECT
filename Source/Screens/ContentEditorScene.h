/*!***************************************************************************
\file       ContentEditorScene.h
\author     Quah Ming Jun, m.quah
\par        m.quah@digipen.edu
\date       30-03-2026

\brief
    This header file declares the data structures and lifecycle functions
    for the Content Editor scene. 

    Scene Lifecycle:
        * ContentEditorLoad()   - Load UI, fonts, palettes, and editor data
        * ContentEditorInit()   - Initialize editor state and UI bindings
        * ContentEditorUpdate() - Handle input, painting, UI, and popups
        * ContentEditorDraw()   - Render grid, UI, and editor overlays
        * ContentEditorFree()   - Free runtime-only resources
        * ContentEditorUnload() - Unload textures, meshes, and editor assets
***************************************************************************/

#include <iostream>
#include <fstream>
#include <cstdlib>

#include "AEEngine.h"

#include "../Grid.h"
#include "Ui.h"
#include "../GameStateManager.h"
#include "../Utils/Utils.h"
#include "../Sprite.h"
#include "../Loaders/DataLoader.h"
#include "../GameStateList.h"




/*!
\enum ContentEditorTab
\brief
	Identifies which editor panel is currently active.
*/
enum class ContentEditorTab
{
	Biomes,
	Obstacles
};

/*!
\struct ContentEditorState
\brief
	Stores all runtime data required by the Content Editor scene.

\par
	This struct acts as the working memory for the editor. It contains:
		* The grid being edited
		* UI state (tabs, selections, hover index)
		* Palette data (biomes, obstacles)
		* Export and status information
		* CSV loading/editing state
*/
struct ContentEditorState
{


	Grid roomGrid{};	// The grid currently being edited

	ContentEditorTab currentTab{ ContentEditorTab::Biomes };  // Which editor tab is active (Biomes or Obstacles).

	std::vector<std::string> biomeNames{};  // List of biome names loaded from tile database. 
	int selectedBiomeIndex{ 0 };	// Index of the currently selected biome

	std::vector<TileType const*> obstaclePalette{};  // List of obstacle tiles available for painting.
	int currentObstaclePalettePage{ 0 };			 // Current page of the obstacle palette (for pagination).
	int selectedTileId{ 0 };						 // Tile ID currently selected for painting

	int hoveredCellIndex{ -1 };						// Index of the tile cell currently hovered by the mouse. 

	std::string exportFileName{ "Custom" };			// Filename used when exporting a new CSV
	std::string statusMessage{ "Ready to export" };	// Status message displayed to the user (e.g., "Saved!", "Error"). 

	// Existing CSV edit mode
	bool isLoadPopupOpen{ false };				// For Popups
	bool isEditingExistingFile{ false };		// Are we editing existing file
	int existingCsvIndex{ 0 };					// Fileindex
	std::vector<std::string> existingCsvPaths{};// Tbh im just gonan use one path
	std::string currentEditingFilePath{};		// Current File Path to modify

	// Patch some bugs??? Flags used to handle popup close/load timing issues...
	bool pendingClosePopUp{ false }, pendingLoadSelectedCSV{ false };

};


void ContentEditorLoad();			// Load all assets, UI layouts, and editor data for the Content Editor.
void ContentEditorInit();			// Initialize editor state, UI bindings, and default selections.
void ContentEditorUpdate(float dt);	// Update the Content Editor each frame.
void ContentEditorDraw();			// Render the grid, UI, and editor overlays.
void ContentEditorFree();			// Free runtime-only resources used by the editor.
void ContentEditorUnload();			// Unload textures, meshes, and editor assets.
