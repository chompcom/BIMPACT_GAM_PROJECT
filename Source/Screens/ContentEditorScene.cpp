/*!***************************************************************************
\file       ContentEditorScene.cpp
\author     Quah Ming Jun, m.quah
\par        m.quah@digipen.edu
\brief
	This source file implements the Content Editor scene, an in-game tool
	for creating, modifying, and exporting room layout CSV files used by
	the procedural generation system.

Copyright (C) 2026 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited.
***************************************************************************/

#include "ContentEditorScene.h"



namespace
{
	UIManager contentEditorUi;
	UIManager loadPopupUi;
	UIManager confirmUi;
	static bool isConfirmPopupOpen = false;

	// Confirmatimation Action
	enum class ConfirmAction
	{
		None,
		RestartLevel,
		BackToMainMenu
	};

	static ConfirmAction pendingConfirmAction = ConfirmAction::None;

	s8 contentEditorFont = 0;
	ContentEditorState contentEditorState{};

	constexpr int obstaclePaletteColumns = 5;
	constexpr int obstaclePaletteRows = 5;
	constexpr int obstaclePaletteItemsPerPage = obstaclePaletteColumns * obstaclePaletteRows;

	// Layout constants (left panel)
	constexpr float obstacleThumbnailStartX = -688.0f;
	constexpr float obstacleThumbnailStartY = 100.0f;
	constexpr float obstacleThumbnailGapX = 72.0f;
	constexpr float obstacleThumbnailGapY = 70.0f;
	constexpr float obstacleThumbnailSize = 48.0f;

	// Save Directory
	std::string saveDirectory{ ".\\Assets\\Levels\\Room_Data\\" };

	/*!
	\struct RectBounds
	\brief
		Represents an axis-aligned rectangle in world coordinates.

	\par
		Used for:
			* Preview panel bounds
			* Room floor bounds
			* Grid cell mapping
	*/
	struct RectBounds {float left{}, right{}, top{}, bottom{};};


	/*!
	\brief
		Extract the filename (without extension) from a CSV path.

	\param path
		Full file path.

	\return
		Filename without ".csv".
	*/
	std::string GetFileNameCSV(std::string const& path) {
		return ExtractFileName(path, ".csv");
	}

	/*!
	\brief
		Scan a folder for CSV files matching a pattern and populate a list.

	\par
		Methodology:
			1. Build a Windows wildcard search pattern.
			2. Use FindFirstFileA / FindNextFileA to enumerate files.
			3. Skip directories.
			4. Append full paths to outList.
			5. Sort results alphabetically.

	\param folder
		Directory to scan.

	\param patternAppend
		Wildcard pattern (e.g., "*.csv").

	\param outList
		Output vector of file paths.
	*/
	void ScanCSVBiomes(std::string const& folder, std::string patternAppend, std::vector<std::string>& outList)
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

	/*!
	\brief
		Refresh the list of existing CSV files available for editing.

	\details
		Scans the Room_Data folder and updates:
			* existingCsvPaths
			* existingCsvIndex
	*/
	void RefreshExistingCsvList()
	{
		contentEditorState.existingCsvPaths.clear();
		//contentEditorState.existingCsvIndex = 0;
		std::string csvFolder{ ".\\Assets\\Levels\\Room_Data" };
		ScanCSVBiomes(csvFolder, "*.csv", contentEditorState.existingCsvPaths);
		std::sort(contentEditorState.existingCsvPaths.begin(), contentEditorState.existingCsvPaths.end());
	}


	/*!
	\brief
		Update the popup label to show the currently selected CSV file.

	\details
		If no CSV files exist, displays "No CSV files found".
	*/
	void RefreshLoadPopupLabel()
	{
		UIElement* label = loadPopupUi.FindById("label_existing_file");
		if (!label) return;

		if (contentEditorState.existingCsvPaths.empty())
		{
			label->text = "No CSV files found";
			return;
		}

		// Filename only, relative path may be better?
		//label->text = ExtractFileName(contentEditorState.existingCsvPaths[contentEditorState.existingCsvIndex]); // Filename only, more professional...
		label->text = contentEditorState.existingCsvPaths[contentEditorState.existingCsvIndex];	// Relative path, got vetoed by groupmates so HAHAH...
	}


	/*!
	\brief
		Retrieve the mouse cursor position in world coordinates.

	\return
		Vector2 containing world-space X/Y.
	*/
	Vector2 GetMouseWorldPosition()
	{
		s32 mouseX = 0, mouseY = 0;
		AEInputGetCursorPosition(&mouseX, &mouseY);
		float worldX = AEGfxGetWinMinX() + static_cast<float>(mouseX), worldY = AEGfxGetWinMaxY() - static_cast<float>(mouseY);
		return Vector2{ worldX, worldY };
	}

	// Check if file exists
	bool FileExists(std::string const& filePath)
	{
		std::ifstream inputFile{ filePath };
		bool exists = inputFile.is_open();
		inputFile.close();
		return exists;
	}

	// Retrieve biome names
	std::string const& GetSelectedBiomeName()
	{
		return contentEditorState.biomeNames[contentEditorState.selectedBiomeIndex];
	}

	// Clear editor
	void ClearEditorRoomGrid()
	{
		for (int row = 0; row < contentEditorState.roomGrid.GetHeight(); ++row)
		{
			for (int column = 0; column < contentEditorState.roomGrid.GetWidth(); ++column) contentEditorState.roomGrid.SetCell(row, column, 0);
		}
	}

	
	/*!
	\brief
		Rebuild the obstacle palette based on the selected biome.

	\details
		Methodology:
			1. Retrieve all tiles belonging to the biome.
			2. Filter out:
				* Empty tiles
				* Special marker tiles (ID 0, 100)
				* Tiles with no asset path
			3. Reset palette page and selected tile.
	*/
	void RebuildObstaclePaletteForSelectedBiome()
	{
		contentEditorState.obstaclePalette.clear();

		std::vector<TileType const*> biomeTiles = Grid::GetTilesFromBiome(GetSelectedBiomeName());

		for (TileType const* currentTileType : biomeTiles)
		{
			// Skip empty and special marker tiles
			if (!currentTileType || currentTileType->id == 0 || currentTileType->id == 100) continue;
			
			// If this line proofs buggy idk.
			if (currentTileType->asset.empty()) continue;

			contentEditorState.obstaclePalette.push_back(currentTileType);
		}

		contentEditorState.currentObstaclePalettePage = 0;
		contentEditorState.selectedTileId = (!contentEditorState.obstaclePalette.empty()) ? (contentEditorState.selectedTileId = contentEditorState.obstaclePalette[0]->id) : (0);
	}

	// Remove tiles from the grid that are not allowed in the selected biome.
	void RemoveTilesNotAllowedForSelectedBiome()
	{
		for (int row = 0; row < contentEditorState.roomGrid.GetHeight(); ++row)
		{
			for (int column = 0; column < contentEditorState.roomGrid.GetWidth(); ++column)
			{
				int currentTileId = contentEditorState.roomGrid.GetCell(column, row);

				if (currentTileId <= 0 || currentTileId == 100) continue;

				if (!contentEditorState.roomGrid.QueryTileAllowedInBiome(currentTileId, GetSelectedBiomeName())) contentEditorState.roomGrid.SetCell(row, column, 0);
			}
		}
	}


	// Retrieve world-space bounds of the room preview panel from UI.
	RectBounds GetPreviewPanelBoundsFromUi()
	{
		RectBounds previewPanelBounds{};

		UIElement* roomPreviewPanel = contentEditorUi.FindById("room_preview_panel");
		if (!roomPreviewPanel)
		{
			return previewPanelBounds;
		}

		float halfWidth = roomPreviewPanel->resolvedSize.x * 0.5f;
		float halfHeight = roomPreviewPanel->resolvedSize.y * 0.5f;

		previewPanelBounds.left = roomPreviewPanel->resolvedPos.x - halfWidth;
		previewPanelBounds.right = roomPreviewPanel->resolvedPos.x + halfWidth;
		previewPanelBounds.top = roomPreviewPanel->resolvedPos.y + halfHeight;
		previewPanelBounds.bottom = roomPreviewPanel->resolvedPos.y - halfHeight;

		return previewPanelBounds;
	}

	// Compute the world-space bounds of the playable floor inside the preview panel.
	RectBounds GetRoomFloorBoundsFromPreviewPanel()
	{
		RectBounds previewPanelBounds = GetPreviewPanelBoundsFromUi();

		// The room image includes walls and borders.
		// The editor grid / hover / painting must all use the playable floor inside that image.
		const float floorInsetLeft = 0.082f;
		const float floorInsetRight = 0.087f;
		const float floorInsetTop = 0.129f;
		const float floorInsetBottom = 0.128f;

		float previewPanelWidth = previewPanelBounds.right - previewPanelBounds.left;
		float previewPanelHeight = previewPanelBounds.top - previewPanelBounds.bottom;

		RectBounds floorBounds{};
		floorBounds.left = previewPanelBounds.left + previewPanelWidth * floorInsetLeft;
		floorBounds.right = previewPanelBounds.right - previewPanelWidth * floorInsetRight;
		floorBounds.top = previewPanelBounds.top - previewPanelHeight * floorInsetTop;
		floorBounds.bottom = previewPanelBounds.bottom + previewPanelHeight * floorInsetBottom;

		return floorBounds;
	}

	// Compute the world-space center of a grid cell in the preview panel.
	Vector2 GetEditorRoomCellWorldCenter(int row, int column)
	{
		RectBounds floorBounds = GetRoomFloorBoundsFromPreviewPanel();

		float floorWidth = floorBounds.right - floorBounds.left;
		float floorHeight = floorBounds.top - floorBounds.bottom;

		float cellWidth = floorWidth / static_cast<float>(contentEditorState.roomGrid.GetWidth());
		float cellHeight = floorHeight / static_cast<float>(contentEditorState.roomGrid.GetHeight());

		float centerX = floorBounds.left + static_cast<float>(column) * cellWidth + cellWidth * 0.5f;

		float centerY = floorBounds.top - static_cast<float>(row) * cellHeight - cellHeight * 0.5f;

		return Vector2{ centerX, centerY };
	}

	// Convert a world-space position to a grid cell index in the editor.
	int GetEditorRoomCellIndexAtWorldPosition(float worldX, float worldY)
	{
		// Do not use Grid::WorldToCell() here.
		// The editor preview is driven by the JSON preview panel, not the gameplay room padding.
		RectBounds floorBounds = GetRoomFloorBoundsFromPreviewPanel();

		if (worldX < floorBounds.left || worldX > floorBounds.right ||
			worldY < floorBounds.bottom || worldY > floorBounds.top)
		{
			return -1;
		}

		float floorWidth = floorBounds.right - floorBounds.left;
		float floorHeight = floorBounds.top - floorBounds.bottom;

		float cellWidth =
			floorWidth / static_cast<float>(contentEditorState.roomGrid.GetWidth());

		float cellHeight =
			floorHeight / static_cast<float>(contentEditorState.roomGrid.GetHeight());

		float localX = worldX - floorBounds.left;
		float localY = floorBounds.top - worldY;

		int column = static_cast<int>(localX / cellWidth);
		int row = static_cast<int>(localY / cellHeight);

		if (row < 0 || row >= contentEditorState.roomGrid.GetHeight())
		{
			return -1;
		}

		if (column < 0 || column >= contentEditorState.roomGrid.GetWidth())
		{
			return -1;
		}

		return row * contentEditorState.roomGrid.GetWidth() + column;
	}

	// Update UI labels, visibility, and dynamic text based on editor state.
	void UpdateContentEditorUi()
	{
		UIElement* modeLabel = contentEditorUi.FindById("label_mode");
		if (modeLabel)
		{
			if (contentEditorState.currentTab == ContentEditorTab::Biomes)
			{
				modeLabel->text = "Biome Selected:";
			}
			else
			{
				modeLabel->text = "Select Obstacles:";
			}
		}

		UIElement* selectedBiomeLabel = contentEditorUi.FindById("label_selected_biome");
		if (selectedBiomeLabel)
		{
			selectedBiomeLabel->text = GetSelectedBiomeName();
		}

		UIElement* pageLabel = contentEditorUi.FindById("label_page");
		if (pageLabel)
		{
			int totalPalettePages =
				static_cast<int>(
					(contentEditorState.obstaclePalette.size() + obstaclePaletteItemsPerPage - 1) /
					obstaclePaletteItemsPerPage
					);

			if (totalPalettePages <= 0)
			{
				totalPalettePages = 1;
			}

			pageLabel->text =
				std::to_string(contentEditorState.currentObstaclePalettePage + 1) +
				" / " +
				std::to_string(totalPalettePages);
		}

		UIElement* statusLabel = contentEditorUi.FindById("label_status");
		if (statusLabel)
		{
			statusLabel->text = contentEditorState.statusMessage;
		}

		UIElement* biomeGroup = contentEditorUi.FindById("biome_group");
		if (biomeGroup)
		{
			biomeGroup->visible = (contentEditorState.currentTab == ContentEditorTab::Biomes);
		}

		UIElement* obstacleGroup = contentEditorUi.FindById("obstacle_group");
		if (obstacleGroup)
		{
			obstacleGroup->visible = (contentEditorState.currentTab == ContentEditorTab::Obstacles);
		}

		UIElement* biomePreviewImage = contentEditorUi.FindById("biome_preview_image");
		if (biomePreviewImage)
		{
			biomePreviewImage->texturePath = Grid::GetPathNameBiome(GetSelectedBiomeName());
		}

		// EXPORT BUTTON / OVERWRITE BUTTON
		UIElement* exportButton = contentEditorUi.FindById("btn_export");
		if (exportButton)
		{
			exportButton->text =
				contentEditorState.isEditingExistingFile ? "OVERWRITE" : "EXPORT";
		}
	}

	// Load a room layout from a CSV file and update editor state accordingly.
	bool ReadFromBiomeCsv(std::string const& filePath) {
		if (!contentEditorState.roomGrid.LoadRoomCSV(filePath))
		{
			return false;
		}

		std::string loadedBiome = ExtractFileName(filePath, ".csv");
		loadedBiome = loadedBiome.substr(0, loadedBiome.find_first_of('_'));

		if (loadedBiome.empty()) return false;

		for (int i = 0; i < static_cast<int>(contentEditorState.biomeNames.size()); ++i)
		{
			if (contentEditorState.biomeNames[i] == loadedBiome)
			{
				contentEditorState.selectedBiomeIndex = i;
				break;
			}
		}

		contentEditorState.currentEditingFilePath = filePath;
		contentEditorState.isEditingExistingFile = true;
		contentEditorState.currentTab = ContentEditorTab::Obstacles;
		contentEditorState.statusMessage = "Loaded: " + GetFileNameCSV(filePath);

		RebuildObstaclePaletteForSelectedBiome();
		UpdateContentEditorUi();
		return true;
	}

	// Export the current editor grid to a CSV file.
	bool ExportCurrentEditorRoomToCsv(std::string const& outputFilePath)
	{
		std::ofstream outputFile(outputFilePath);

		if (!outputFile.is_open())
		{
			return false;
		}

		outputFile << "Biome," << GetSelectedBiomeName() << "\n";

		for (int row = 0; row < contentEditorState.roomGrid.GetHeight(); ++row)
		{
			for (int column = 0; column < contentEditorState.roomGrid.GetWidth(); ++column)
			{
				outputFile << contentEditorState.roomGrid.GetCell(column, row);

				if (column < contentEditorState.roomGrid.GetWidth() - 1)
				{
					outputFile << ",";
				}
			}

			outputFile << "\n";
		}

		return true;
	}


	/*!
	\brief
		Randomly populate the grid with obstacles for quick testing.

	\par
		Places:
			* Random obstacles
			* One enemy marker
			* One gift marker
	*/
	void RandomizeCurrentEditorRoomLayout()
	{
		ClearEditorRoomGrid();

		if (contentEditorState.obstaclePalette.empty())
		{
			contentEditorState.statusMessage = "No obstacle palette available";
			UpdateContentEditorUi();
			return;
		}

		int numberMaxItems = 10; // (9 + 4 = 13)
		int numberOfPlacements = 4 + (std::rand() % numberMaxItems);

		for (int i = 0; i < numberOfPlacements; ++i)
		{
			int randomRow = 1 + (std::rand() % (contentEditorState.roomGrid.GetHeight() - 2));
			int randomColumn = 1 + (std::rand() % (contentEditorState.roomGrid.GetWidth() - 2));
			int randomPaletteIndex = std::rand() % static_cast<int>(contentEditorState.obstaclePalette.size());

			contentEditorState.roomGrid.SetCell(randomRow,randomColumn, contentEditorState.obstaclePalette[randomPaletteIndex]->id);
		}

		// Place one generic enemy marker
		{
			int randomRow = 1 + (std::rand() % (contentEditorState.roomGrid.GetHeight() - 2));
			int randomColumn = 1 + (std::rand() % (contentEditorState.roomGrid.GetWidth() - 2));

			contentEditorState.roomGrid.SetCell(randomRow, randomColumn, 200);
		}

		// Place one generic gift marker
		{
			int randomRow = 1 + (std::rand() % (contentEditorState.roomGrid.GetHeight() - 2));
			int randomColumn = 1 + (std::rand() % (contentEditorState.roomGrid.GetWidth() - 2));
			contentEditorState.roomGrid.SetCell(randomRow, randomColumn, 500);
		}

		contentEditorState.statusMessage = "Randomized";
		UpdateContentEditorUi();
	}

	/*!
	\brief
		Draw obstacle palette thumbnails in the left panel.

	\details
		Methodology:
			1. Determine visible palette range based on current page.
			2. Draw background slot for each thumbnail.
			3. Draw tile texture if available.
			4. Draw selection outline if tile is selected.
	*/
	void DrawObstaclePaletteThumbnails()
	{
		if (contentEditorState.currentTab != ContentEditorTab::Obstacles) return;

		AEGfxVertexList* squareMesh = DataLoader::GetOrCreateSquareMesh();
		if (!squareMesh) return;

		int firstPaletteIndex = contentEditorState.currentObstaclePalettePage * obstaclePaletteItemsPerPage;

		int lastPaletteIndex = firstPaletteIndex + obstaclePaletteItemsPerPage;
		if (lastPaletteIndex > static_cast<int>(contentEditorState.obstaclePalette.size()))
		{
			lastPaletteIndex = static_cast<int>(contentEditorState.obstaclePalette.size());
		}

		int visibleSlotIndex = 0;

		for (int paletteIndex = firstPaletteIndex; paletteIndex < lastPaletteIndex; ++paletteIndex)
		{
			int row = visibleSlotIndex / obstaclePaletteColumns;
			int column = visibleSlotIndex % obstaclePaletteColumns;

			float thumbnailCenterX = obstacleThumbnailStartX + static_cast<float>(column) * obstacleThumbnailGapX;

			float thumbnailCenterY = obstacleThumbnailStartY - static_cast<float>(row) * obstacleThumbnailGapY;

			TileType const* currentTileType = contentEditorState.obstaclePalette[paletteIndex];
			if (!currentTileType)
			{
				++visibleSlotIndex;
				continue;
			}

			AEGfxSetRenderMode(AE_GFX_RM_COLOR);
			AEGfxSetBlendMode(AE_GFX_BM_BLEND);
			AEGfxSetTransparency(1.0f);

			Sprite thumbnailSlotBackground(
				squareMesh,
				Vector2{ thumbnailCenterX, thumbnailCenterY },
				Vector2{ 60.0f, 60.0f },
				Color{ 0.95f, 0.95f, 0.95f, 1.0f }
			);
			thumbnailSlotBackground.RenderSprite(true);

			if (!currentTileType->asset.empty())	// && FileExists(currentTileType->asset) Doubt i need this
			{
				AEGfxSetRenderMode(AE_GFX_RM_TEXTURE);
				AEGfxSetBlendMode(AE_GFX_BM_BLEND);
				AEGfxSetTransparency(1.0f);

				TexturedSprite thumbnailSprite = (!FileExists(currentTileType->asset)) ? (DataLoader::CreateTexture("Assets/default.png")) : (DataLoader::CreateTexture(currentTileType->asset));

				thumbnailSprite.position = Vector2{ thumbnailCenterX, thumbnailCenterY };
				thumbnailSprite.scale = Vector2{ obstacleThumbnailSize, obstacleThumbnailSize };
				thumbnailSprite.UpdateTransform();

				// true here is important because the current sprite render path can darken textures otherwise. Act idk anymore, the more you know / don't know.
				thumbnailSprite.RenderSprite(true);
			}

			if (currentTileType->id == contentEditorState.selectedTileId)
			{
				AEGfxSetRenderMode(AE_GFX_RM_COLOR);
				AEGfxSetBlendMode(AE_GFX_BM_BLEND);
				AEGfxSetTransparency(1.0f);

				Sprite selectedOutline(
					squareMesh,
					Vector2{ thumbnailCenterX, thumbnailCenterY },
					Vector2{ 68.0f, 68.0f },
					Color{ 0.3f, 0.7f, 1.0f, 0.25f }
				);
				selectedOutline.RenderSprite(true);
			}

			++visibleSlotIndex;
		}
	}
}

// Load all assets, UI layouts, and initial editor data.
void ContentEditorLoad()
{
	contentEditorFont = AEGfxCreateFont("Assets/Kenney Pixel.ttf", 56);
	contentEditorUi.SetFont(contentEditorFont);

	contentEditorUi.UseWindowAsRootRect();
	contentEditorUi.LoadFromFile("Assets/UI/contenteditor.json");

	// Load one valid room CSV first so width/height already exist
	contentEditorState.roomGrid.LoadRoomCSV(saveDirectory + "Normal_1.csv");
	ClearEditorRoomGrid();

	contentEditorState.biomeNames = Grid::GetAllBiomes();

	if (contentEditorState.biomeNames.empty()) contentEditorState.biomeNames.push_back("Normal");

	contentEditorState.selectedBiomeIndex = 0;
	contentEditorState.currentTab = ContentEditorTab::Biomes;
	contentEditorState.currentObstaclePalettePage = 0;
	contentEditorState.selectedTileId = 0;
	contentEditorState.hoveredCellIndex = -1;
	contentEditorState.statusMessage = "Ready to export";

	RebuildObstaclePaletteForSelectedBiome();
	UpdateContentEditorUi();

	// Resolve UI once now so the preview panel has valid world bounds before first draw/update
	contentEditorUi.Update();

	contentEditorUi.BindOnClick("btn_mainmenu", [](UIElement&)
		{
			contentEditorState.currentTab = ContentEditorTab::Biomes;
			UpdateContentEditorUi();
		});


	contentEditorUi.BindOnClick("btn_biomes", [](UIElement&)
		{
			contentEditorState.currentTab = ContentEditorTab::Biomes;
			UpdateContentEditorUi();
		});

	contentEditorUi.BindOnClick("btn_obstacles", [](UIElement&)
		{
			contentEditorState.currentTab = ContentEditorTab::Obstacles;
			UpdateContentEditorUi();
		});

	contentEditorUi.BindOnClick("btn_confirm", [](UIElement&)
		{
			contentEditorState.currentTab = ContentEditorTab::Obstacles;
			UpdateContentEditorUi();
		});

	contentEditorUi.BindOnClick("btn_prev", [](UIElement&)
		{
			if (contentEditorState.currentTab == ContentEditorTab::Biomes)
			{
				contentEditorState.selectedBiomeIndex--;

				if (contentEditorState.selectedBiomeIndex < 0)
				{
					contentEditorState.selectedBiomeIndex =
						static_cast<int>(contentEditorState.biomeNames.size()) - 2;
				}

				RebuildObstaclePaletteForSelectedBiome();
				RemoveTilesNotAllowedForSelectedBiome();
			}
			else
			{
				if (contentEditorState.currentObstaclePalettePage > 0)
				{
					contentEditorState.currentObstaclePalettePage--;
				}
			}

			UpdateContentEditorUi();
		});

	contentEditorUi.BindOnClick("btn_next", [](UIElement&)
		{
			if (contentEditorState.currentTab == ContentEditorTab::Biomes)
			{
				contentEditorState.selectedBiomeIndex++;

				if (contentEditorState.selectedBiomeIndex >=
					static_cast<int>(contentEditorState.biomeNames.size()) - 1)
				{
					contentEditorState.selectedBiomeIndex = 0;
				}

				RebuildObstaclePaletteForSelectedBiome();
				RemoveTilesNotAllowedForSelectedBiome();
			}
			else
			{
				int totalPalettePages = static_cast<int>((contentEditorState.obstaclePalette.size() + obstaclePaletteItemsPerPage - 1) /obstaclePaletteItemsPerPage);
				if (contentEditorState.currentObstaclePalettePage + 1 < totalPalettePages) contentEditorState.currentObstaclePalettePage++;
			}

			UpdateContentEditorUi();
		});

	contentEditorUi.BindOnClick("btn_choose_existing", [](UIElement&)
		{
			RefreshExistingCsvList();

			loadPopupUi.Clear();
			loadPopupUi.SetFont(contentEditorFont);
			loadPopupUi.LoadFromFilePopUp("Assets/UI/contenteditor_load_popup.json", Vector2(0.0f, 0.0f), Vector2(720.0f, 260.0f));

			RefreshLoadPopupLabel();
			contentEditorState.isLoadPopupOpen = true;

			// Popup buttons only exist after popup JSON is loaded,
			// so bind them here.
			loadPopupUi.BindOnClick("btn_prev_existing", [](UIElement&)
				{
					if (contentEditorState.existingCsvPaths.empty()) return;

					contentEditorState.existingCsvIndex--;
					if (contentEditorState.existingCsvIndex < 0)
					{
						contentEditorState.existingCsvIndex = static_cast<int>(contentEditorState.existingCsvPaths.size()) - 1;
					}

					RefreshLoadPopupLabel();
				});

			loadPopupUi.BindOnClick("btn_next_existing", [](UIElement&)
				{
					if (contentEditorState.existingCsvPaths.empty()) return;

					contentEditorState.existingCsvIndex++;
					if (contentEditorState.existingCsvIndex >= static_cast<int>(contentEditorState.existingCsvPaths.size()))
					{
						contentEditorState.existingCsvIndex = 0;
					}

					RefreshLoadPopupLabel();
				});

			loadPopupUi.BindOnClick("btn_cancel_existing", [](UIElement&)
				{
					contentEditorState.pendingClosePopUp = true; // prevent crashes...
				});

			loadPopupUi.BindOnClick("btn_load_existing", [](UIElement&)
				{
					if (contentEditorState.existingCsvPaths.empty())
					{
						contentEditorState.statusMessage = "No CSV files found";
						UpdateContentEditorUi();
						contentEditorState.pendingClosePopUp = true;
						return;
					}

					contentEditorState.pendingLoadSelectedCSV = true;
				});
		});

	contentEditorUi.BindOnClick("btn_random", [](UIElement&)
		{
			RandomizeCurrentEditorRoomLayout();
		});

	contentEditorUi.BindOnClick("btn_export", [](UIElement&)
		{
			std::string outputFilePath{};

			if (contentEditorState.isEditingExistingFile && !contentEditorState.currentEditingFilePath.empty())
			{
				outputFilePath = contentEditorState.currentEditingFilePath;
			}
			else
			{
				std::size_t fileNumber = 1;

				outputFilePath = saveDirectory + GetSelectedBiomeName() + "_" + contentEditorState.exportFileName + "_" + std::to_string(fileNumber) + ".csv";

				while (FileExists(outputFilePath))
				{
					outputFilePath = saveDirectory + GetSelectedBiomeName() + "_" + contentEditorState.exportFileName + "_" + std::to_string(++fileNumber) + ".csv";
				}
			}

			if (ExportCurrentEditorRoomToCsv(outputFilePath))
			{
				contentEditorState.statusMessage =
					(contentEditorState.isEditingExistingFile ? "Overwrote: " : "Exported: ") +
					outputFilePath;
			}
			else
			{
				contentEditorState.statusMessage = "Export failed";
			}

			UpdateContentEditorUi();
		});

	contentEditorUi.BindOnClick("btn_mainmenu", [](UIElement& self)
		{
			UNREFERENCED_PARAMETER(self);
			pendingConfirmAction = ConfirmAction::BackToMainMenu;
			isConfirmPopupOpen = true;

			UIElement* messageLabel = confirmUi.FindById("message");
			if (messageLabel) messageLabel->text = "Return to menu?";
		});

	// Confirmation screen dialog
	confirmUi.LoadFromFilePopUp("Assets/UI/confirmation_popup.json", Vector2(0.0f, 0.0f), Vector2(560.0f, 240.0f));
	confirmUi.SetFont(contentEditorFont);


	confirmUi.BindOnClick("btn_yes", [](UIElement&)
		{
			// FAST CONFIRMATION ENUMS SETUP hah.
			switch (pendingConfirmAction)
			{
			case ConfirmAction::RestartLevel:
				ChangeState(GS_RESTART);
				break;

			case ConfirmAction::BackToMainMenu:
				ChangeState(GS_MAINMENU);
				break;

			default:
				break;
			}

			pendingConfirmAction = ConfirmAction::None;
			isConfirmPopupOpen = false;
		});

	confirmUi.BindOnClick("btn_no", [](UIElement&)
		{
			pendingConfirmAction = ConfirmAction::None;
			isConfirmPopupOpen = false;
		});

}

// Nth to see here
void ContentEditorInit()
{
}

// Update the Content Editor each frame
void ContentEditorUpdate(float dt)
{
	UNREFERENCED_PARAMETER(dt);


	if (contentEditorState.isLoadPopupOpen)
	{
		loadPopupUi.Update();

		if (AEInputCheckTriggered(AEVK_ESCAPE))
		{
			contentEditorState.pendingClosePopUp = true;
		}

		// Do destructive popup actions only after Update() is done
		if (contentEditorState.pendingLoadSelectedCSV)
		{
			contentEditorState.pendingLoadSelectedCSV = false;

			if (!contentEditorState.existingCsvPaths.empty())
			{
				std::string chosenPath = contentEditorState.existingCsvPaths[contentEditorState.existingCsvIndex];



				if (!ReadFromBiomeCsv(chosenPath))
				{
					contentEditorState.statusMessage = "Failed to load CSV";
					UpdateContentEditorUi();
				}
			}

			contentEditorState.pendingClosePopUp = true;
		}

		if (contentEditorState.pendingClosePopUp)
		{
			contentEditorState.pendingClosePopUp = false;
			contentEditorState.isLoadPopupOpen = false;
			loadPopupUi.Clear();
		}

		return;
	}

	if (isConfirmPopupOpen)
	{
		if (AEInputCheckTriggered(AEVK_ESCAPE))
		{
			pendingConfirmAction = ConfirmAction::None;
			isConfirmPopupOpen = false;
		}

		confirmUi.Update();
	}

	if (AEInputCheckTriggered(AEVK_ESCAPE))
	{
		pendingConfirmAction = ConfirmAction::BackToMainMenu;
		isConfirmPopupOpen = true;

		UIElement* messageLabel = confirmUi.FindById("message");
		if (messageLabel) messageLabel->text = "Return to menu?";
	}

	contentEditorUi.Update();

	Vector2 mouseWorldPosition = GetMouseWorldPosition();

	contentEditorState.hoveredCellIndex = GetEditorRoomCellIndexAtWorldPosition(mouseWorldPosition.x, mouseWorldPosition.y);

	if (contentEditorState.currentTab == ContentEditorTab::Obstacles && AEInputCheckTriggered(AEVK_LBUTTON))
	{
		int firstPaletteIndex = contentEditorState.currentObstaclePalettePage * obstaclePaletteItemsPerPage;

		int lastPaletteIndex = firstPaletteIndex + obstaclePaletteItemsPerPage;
		if (lastPaletteIndex > static_cast<int>(contentEditorState.obstaclePalette.size()))
		{
			lastPaletteIndex = static_cast<int>(contentEditorState.obstaclePalette.size());
		}

		int visibleSlotIndex = 0;

		for (int paletteIndex = firstPaletteIndex; paletteIndex < lastPaletteIndex; ++paletteIndex)
		{
			int row = visibleSlotIndex / obstaclePaletteColumns;
			int column = visibleSlotIndex % obstaclePaletteColumns;

			float thumbnailCenterX = obstacleThumbnailStartX + static_cast<float>(column) * obstacleThumbnailGapX;
			float thumbnailCenterY = obstacleThumbnailStartY - static_cast<float>(row) * obstacleThumbnailGapY;
			float halfThumbnailSize = 30.0f;

			if (mouseWorldPosition.x >= thumbnailCenterX - halfThumbnailSize &&
				mouseWorldPosition.x <= thumbnailCenterX + halfThumbnailSize &&
				mouseWorldPosition.y >= thumbnailCenterY - halfThumbnailSize &&
				mouseWorldPosition.y <= thumbnailCenterY + halfThumbnailSize)
			{
				contentEditorState.selectedTileId = contentEditorState.obstaclePalette[paletteIndex]->id;
				contentEditorState.statusMessage = "Selected tile name: " + contentEditorState.roomGrid.QueryTileType(contentEditorState.selectedTileId)->name; // std::to_string(contentEditorState.roomGrid.QueryTileType(contentEditorState.selectedTileId)->name);

				UpdateContentEditorUi();
				return;
			}

			++visibleSlotIndex;
		}
	}

	if (contentEditorState.currentTab == ContentEditorTab::Obstacles &&
		contentEditorState.hoveredCellIndex >= 0)
	{
		int clickedRow = contentEditorState.hoveredCellIndex / contentEditorState.roomGrid.GetWidth();
		int clickedColumn = contentEditorState.hoveredCellIndex % contentEditorState.roomGrid.GetWidth();

		// Add stuff
		if (AEInputCheckCurr(AEVK_LBUTTON)) contentEditorState.roomGrid.SetCell(clickedRow, clickedColumn, contentEditorState.selectedTileId);
		
		// Remove stuff
		else if (AEInputCheckCurr(AEVK_RBUTTON)) contentEditorState.roomGrid.SetCell(clickedRow, clickedColumn, 0);
		
	}

	// Idek who did this but thanks, escape back instead of explicit 'back' button
	//if (AEInputCheckTriggered(AEVK_ESCAPE)) ChangeState(GS_MAINMENU);
	
}

// Render the Content Editor scene.
void ContentEditorDraw()
{
	AEGfxSetBackgroundColor(0.08f, 0.08f, 0.08f);

	RectBounds previewPanelBounds = GetPreviewPanelBoundsFromUi();
	if (previewPanelBounds.right > previewPanelBounds.left &&
		previewPanelBounds.top > previewPanelBounds.bottom)
	{
		AEGfxSetRenderMode(AE_GFX_RM_TEXTURE);
		AEGfxSetBlendMode(AE_GFX_BM_BLEND);
		AEGfxSetTransparency(1.0f);

		TexturedSprite roomBackgroundSprite =
			DataLoader::CreateTexture(Grid::GetPathNameBiome(GetSelectedBiomeName()));

		roomBackgroundSprite.position = Vector2{
			(previewPanelBounds.left + previewPanelBounds.right) * 0.5f,
			(previewPanelBounds.top + previewPanelBounds.bottom) * 0.5f
		};

		roomBackgroundSprite.scale = Vector2{
			previewPanelBounds.right - previewPanelBounds.left,
			previewPanelBounds.top - previewPanelBounds.bottom
		};

		roomBackgroundSprite.UpdateTransform();
		roomBackgroundSprite.RenderSprite(true);

		RectBounds roomFloorBounds = GetRoomFloorBoundsFromPreviewPanel();
		float roomFloorWidth = roomFloorBounds.right - roomFloorBounds.left;
		float roomFloorHeight = roomFloorBounds.top - roomFloorBounds.bottom;

		float roomCellWidth = roomFloorWidth / static_cast<float>(contentEditorState.roomGrid.GetWidth());
		float roomCellHeight = roomFloorHeight / static_cast<float>(contentEditorState.roomGrid.GetHeight());
		float placedObstacleDrawSize = AEMin(roomCellWidth, roomCellHeight) * 0.72f;

		for (int row = 0; row < contentEditorState.roomGrid.GetHeight(); ++row)
		{
			for (int column = 0; column < contentEditorState.roomGrid.GetWidth(); ++column)
			{
				int currentTileId = contentEditorState.roomGrid.GetCell(column, row);

				if (currentTileId <= 0 || currentTileId == 100) continue;

				TileType const* currentTileType = Grid::QueryTileType(currentTileId);
				if (!currentTileType) continue;
				if (currentTileType->asset.empty()) continue;
				

				TexturedSprite obstacleSprite = (!FileExists(currentTileType->asset)) ? (DataLoader::CreateTexture("Assets/default.png")) : (DataLoader::CreateTexture(currentTileType->asset));


				obstacleSprite.position = GetEditorRoomCellWorldCenter(row, column);
				obstacleSprite.scale = Vector2{ placedObstacleDrawSize, placedObstacleDrawSize };
				obstacleSprite.UpdateTransform();
				obstacleSprite.RenderSprite(true);
			}
		}

		AEGfxVertexList* squareMesh = DataLoader::GetOrCreateSquareMesh();
		if (squareMesh)
		{
			AEGfxSetRenderMode(AE_GFX_RM_COLOR);
			AEGfxSetBlendMode(AE_GFX_BM_BLEND);
			AEGfxSetTransparency(1.0f);

			float roomCenterX = (roomFloorBounds.left + roomFloorBounds.right) * 0.5f;
			float roomCenterY = (roomFloorBounds.top + roomFloorBounds.bottom) * 0.5f;

			if (contentEditorState.hoveredCellIndex >= 0)
			{
				int hoveredRow = contentEditorState.hoveredCellIndex / contentEditorState.roomGrid.GetWidth();

				int hoveredColumn = contentEditorState.hoveredCellIndex % contentEditorState.roomGrid.GetWidth();

				Sprite hoveredCellHighlight(
					squareMesh,
					GetEditorRoomCellWorldCenter(hoveredRow, hoveredColumn),
					Vector2{ roomCellWidth, roomCellHeight },
					Color{ 0.3f, 0.8f, 0.2f, 0.35f }
				);
				hoveredCellHighlight.RenderSprite(true);
			}

			for (int column = 0; column <= contentEditorState.roomGrid.GetWidth(); ++column)
			{
				float lineX = roomFloorBounds.left + static_cast<float>(column) * roomCellWidth;

				Sprite verticalLine(
					squareMesh,
					Vector2{ lineX, roomCenterY },
					Vector2{ 2.0f, roomFloorHeight },
					Color{ 0.0f, 0.0f, 0.0f, 0.38f }
				);
				verticalLine.RenderSprite(true);
			}

			for (int row = 0; row <= contentEditorState.roomGrid.GetHeight(); ++row)
			{
				float lineY = roomFloorBounds.top - static_cast<float>(row) * roomCellHeight;

				Sprite horizontalLine(
					squareMesh,
					Vector2{ roomCenterX, lineY },
					Vector2{ roomFloorWidth, 2.0f },
					Color{ 0.0f, 0.0f, 0.0f, 0.38f }
				);
				horizontalLine.RenderSprite(true);
			}
		}
	}

	contentEditorUi.Draw();
	DrawObstaclePaletteThumbnails();


	// Popup existing file draw
	if (contentEditorState.isLoadPopupOpen)
	{
		AEGfxSetRenderMode(AE_GFX_RM_COLOR);
		AEGfxSetBlendMode(AE_GFX_BM_BLEND);
		AEGfxSetTransparency(1.0f);

		Sprite overlay(
			DataLoader::GetOrCreateSquareMesh(),
			Vector2(0.0f, 0.0f),
			Vector2(
				AEGfxGetWinMaxX() - AEGfxGetWinMinX(),
				AEGfxGetWinMaxY() - AEGfxGetWinMinY()
			),
			Color{ 0.0f, 0.0f, 0.0f, 0.35f }
		);
		overlay.RenderSprite(true);

		loadPopupUi.Draw();
	}

	if (isConfirmPopupOpen) {
		AEGfxSetRenderMode(AE_GFX_RM_COLOR);
		AEGfxSetBlendMode(AE_GFX_BM_BLEND);
		AEGfxSetTransparency(1.0f);

		Sprite overlay(
			DataLoader::GetOrCreateSquareMesh(),
			Vector2(0.0f, 0.0f),
			Vector2(
				AEGfxGetWinMaxX() - AEGfxGetWinMinX(),
				AEGfxGetWinMaxY() - AEGfxGetWinMinY()
			),
			Color{ 0.0f, 0.0f, 0.0f, 0.55f }
		);
		overlay.RenderSprite(true);

		confirmUi.Draw();
	}

}


// Nth to see here
void ContentEditorFree()
{
}

// Unload all assets used by the Content Editor.
void ContentEditorUnload()
{
	contentEditorUi.Clear();

	if (contentEditorFont)
	{
		AEGfxDestroyFont(contentEditorFont);
		contentEditorFont = 0;
	}

	DataLoader::Unload();
}

