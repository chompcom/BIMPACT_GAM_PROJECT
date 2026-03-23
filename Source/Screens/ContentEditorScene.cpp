#include "ContentEditorScene.h"

#include <cstdlib>
#include <fstream>

#include "AEEngine.h"
#include "../Sprite.h"
#include "../Loaders/DataLoader.h"
#include "../GameStateList.h"

namespace
{
	UIManager contentEditorUi;
	s8 contentEditorFont = 0;
	ContentEditorState contentEditorState{};

	constexpr int obstaclePaletteColumns = 5;
	constexpr int obstaclePaletteRows = 5;
	constexpr int obstaclePaletteItemsPerPage = obstaclePaletteColumns * obstaclePaletteRows;

	// Thumbnail layout inside the left panel
	constexpr float obstacleThumbnailStartX = -688.0f;
	constexpr float obstacleThumbnailStartY = 100.0f;
	constexpr float obstacleThumbnailGapX = 72.0f;
	constexpr float obstacleThumbnailGapY = 70.0f;
	constexpr float obstacleThumbnailSize = 48.0f;

	struct RectBounds
	{
		float left{};
		float right{};
		float top{};
		float bottom{};
	};

	Vector2 GetMouseWorldPosition()
	{
		s32 mouseX = 0;
		s32 mouseY = 0;
		AEInputGetCursorPosition(&mouseX, &mouseY);

		float worldX = AEGfxGetWinMinX() + static_cast<float>(mouseX);
		float worldY = AEGfxGetWinMaxY() - static_cast<float>(mouseY);

		return Vector2{ worldX, worldY };
	}

	bool FileExists(std::string const& filePath)
	{
		std::ifstream inputFile{ filePath };
		bool exists = inputFile.is_open();
		inputFile.close();
		return exists;
	}

	std::string const& GetSelectedBiomeName()
	{
		return contentEditorState.biomeNames[contentEditorState.selectedBiomeIndex];
	}

	std::string GetSelectedBiomeRoomTexturePath()
	{
		return Grid::GetPathNameBiome(GetSelectedBiomeName());
	}

	std::string GetSelectedBiomePreviewTexturePath()
	{
		// Use dedicated thumbnail if it exists. (Shouldn't have anyways lol)
		if (GetSelectedBiomeName() == "Normal" && FileExists("Assets/UI/NORMAL_thumb.png"))
		{
			return "Assets/UI/NORMAL_thumb.png";
		}

		if (GetSelectedBiomeName() == "Green" && FileExists("Assets/UI/green_thumb.png"))
		{
			return "Assets/UI/green_thumb.png";
		}

		if (GetSelectedBiomeName() == "Ice" && FileExists("Assets/UI/ICE_thumb.png"))
		{
			return "Assets/UI/ICE_thumb.png";
		}

		return GetSelectedBiomeRoomTexturePath();
	}

	void ClearEditorRoomGrid()
	{
		for (int row = 0; row < contentEditorState.roomGrid.GetHeight(); ++row)
		{
			for (int column = 0; column < contentEditorState.roomGrid.GetWidth(); ++column)
			{
				contentEditorState.roomGrid.SetCell(row, column, 0);
			}
		}
	}

	void RebuildObstaclePaletteForSelectedBiome()
	{
		contentEditorState.obstaclePalette.clear();

		std::vector<TileType const*> biomeTiles =
			Grid::GetTilesFromBiome(GetSelectedBiomeName());

		for (TileType const* currentTileType : biomeTiles)
		{
			if (!currentTileType)
			{
				continue;
			}

			// Skip empty and special marker tiles
			if (currentTileType->id == 0)
			{
				continue;
			}

			if (currentTileType->id >= 100)
			{
				continue;
			}

			if (currentTileType->asset.empty())
			{
				continue;
			}

			contentEditorState.obstaclePalette.push_back(currentTileType);
		}

		contentEditorState.currentObstaclePalettePage = 0;

		if (!contentEditorState.obstaclePalette.empty())
		{
			contentEditorState.selectedTileId = contentEditorState.obstaclePalette[0]->id;
		}
		else
		{
			contentEditorState.selectedTileId = 0;
		}
	}

	void RemoveTilesNotAllowedForSelectedBiome()
	{
		for (int row = 0; row < contentEditorState.roomGrid.GetHeight(); ++row)
		{
			for (int column = 0; column < contentEditorState.roomGrid.GetWidth(); ++column)
			{
				int currentTileId = contentEditorState.roomGrid.GetCell(column, row);

				if (currentTileId <= 0)
				{
					continue;
				}

				if (currentTileId >= 100)
				{
					continue;
				}

				if (!contentEditorState.roomGrid.QueryTileAllowedInBiome(currentTileId, GetSelectedBiomeName()))
				{
					contentEditorState.roomGrid.SetCell(row, column, 0);
				}
			}
		}
	}


	// Room Code
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
	Vector2 GetEditorRoomCellWorldCenter(int row, int column)
	{
		RectBounds floorBounds = GetRoomFloorBoundsFromPreviewPanel();

		float floorWidth = floorBounds.right - floorBounds.left;
		float floorHeight = floorBounds.top - floorBounds.bottom;

		float cellWidth = floorWidth / static_cast<float>(contentEditorState.roomGrid.GetWidth());
		float cellHeight = floorHeight / static_cast<float>(contentEditorState.roomGrid.GetHeight());

		float centerX =
			floorBounds.left +
			static_cast<float>(column) * cellWidth +
			cellWidth * 0.5f;

		float centerY =
			floorBounds.top -
			static_cast<float>(row) * cellHeight -
			cellHeight * 0.5f;

		return Vector2{ centerX, centerY };
	}

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

	void UpdateContentEditorUi()
	{
		UIElement* modeLabel = contentEditorUi.FindById("label_mode");
		if (modeLabel)
		{
			if (contentEditorState.currentTab == ContentEditorTab::Biomes)
			{
				modeLabel->text = "Select Biomes:";
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
			biomePreviewImage->texturePath = GetSelectedBiomePreviewTexturePath();
		}
	}

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


	// To amend in time to come?
	void RandomizeCurrentEditorRoomLayout()
	{
		ClearEditorRoomGrid();

		if (contentEditorState.obstaclePalette.empty())
		{
			contentEditorState.statusMessage = "No obstacle palette available";
			UpdateContentEditorUi();
			return;
		}

		int numberOfPlacements = 4 + (std::rand() % 9);

		for (int i = 0; i < numberOfPlacements; ++i)
		{
			int randomRow = 1 + (std::rand() % (contentEditorState.roomGrid.GetHeight() - 2));

			int randomColumn = 1 + (std::rand() % (contentEditorState.roomGrid.GetWidth() - 2));

			int randomPaletteIndex = std::rand() % static_cast<int>(contentEditorState.obstaclePalette.size());

			contentEditorState.roomGrid.SetCell(randomRow, randomColumn, contentEditorState.obstaclePalette[randomPaletteIndex]->id
			);
		}

		contentEditorState.statusMessage = "Randomized";
		UpdateContentEditorUi();
	}

	void DrawObstaclePaletteThumbnails()
	{
		if (contentEditorState.currentTab != ContentEditorTab::Obstacles)
		{
			return;
		}

		AEGfxVertexList* squareMesh = DataLoader::GetOrCreateSquareMesh();
		if (!squareMesh)
		{
			return;
		}

		int firstPaletteIndex =
			contentEditorState.currentObstaclePalettePage * obstaclePaletteItemsPerPage;

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

			float thumbnailCenterX =
				obstacleThumbnailStartX + static_cast<float>(column) * obstacleThumbnailGapX;

			float thumbnailCenterY =
				obstacleThumbnailStartY - static_cast<float>(row) * obstacleThumbnailGapY;

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

			if (!currentTileType->asset.empty() && FileExists(currentTileType->asset))
			{
				AEGfxSetRenderMode(AE_GFX_RM_TEXTURE);
				AEGfxSetBlendMode(AE_GFX_BM_BLEND);
				AEGfxSetTransparency(1.0f);

				TexturedSprite thumbnailSprite =
					DataLoader::CreateTexture(currentTileType->asset);

				thumbnailSprite.position = Vector2{ thumbnailCenterX, thumbnailCenterY };
				thumbnailSprite.scale = Vector2{ obstacleThumbnailSize, obstacleThumbnailSize };
				thumbnailSprite.UpdateTransform();

				// true here is important because the current sprite render path can darken textures otherwise
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

void ContentEditorLoad()
{
	contentEditorFont = AEGfxCreateFont("Assets/Kenney Pixel.ttf", 56);
	contentEditorUi.SetFont(contentEditorFont);

	contentEditorUi.UseWindowAsRootRect();
	contentEditorUi.LoadFromFile("Assets/UI/contenteditor.json");

	// Load one valid room CSV first so width/height already exist
	contentEditorState.roomGrid.LoadRoomCSV(".\\Assets\\Levels\\Room_Data\\Normal_1.csv");
	ClearEditorRoomGrid();

	contentEditorState.biomeNames = Grid::GetAllBiomes();

	if (contentEditorState.biomeNames.empty())
	{
		contentEditorState.biomeNames.push_back("Normal");
	}

	contentEditorState.selectedBiomeIndex = 0;
	contentEditorState.currentTab = ContentEditorTab::Biomes;
	contentEditorState.currentObstaclePalettePage = 0;
	contentEditorState.selectedTileId = 0;
	contentEditorState.hoveredCellIndex = -1;
	contentEditorState.statusMessage = "Ready";

	RebuildObstaclePaletteForSelectedBiome();
	UpdateContentEditorUi();

	// Resolve UI once now so the preview panel has valid world bounds before first draw/update
	contentEditorUi.Update();

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
						static_cast<int>(contentEditorState.biomeNames.size()) - 1;
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
					static_cast<int>(contentEditorState.biomeNames.size()))
				{
					contentEditorState.selectedBiomeIndex = 0;
				}

				RebuildObstaclePaletteForSelectedBiome();
				RemoveTilesNotAllowedForSelectedBiome();
			}
			else
			{
				int totalPalettePages =
					static_cast<int>(
						(contentEditorState.obstaclePalette.size() + obstaclePaletteItemsPerPage - 1) /
						obstaclePaletteItemsPerPage
						);

				if (contentEditorState.currentObstaclePalettePage + 1 < totalPalettePages)
				{
					contentEditorState.currentObstaclePalettePage++;
				}
			}

			UpdateContentEditorUi();
		});

	contentEditorUi.BindOnClick("btn_random", [](UIElement&)
		{
			RandomizeCurrentEditorRoomLayout();
		});

	contentEditorUi.BindOnClick("btn_export", [](UIElement&)
		{
			std::size_t fileNumber = 1;

			std::string outputFilePath =
				".\\Assets\\Levels\\Room_Data\\" +
				GetSelectedBiomeName() +
				"_" +
				contentEditorState.exportFileName +
				"_" +
				std::to_string(fileNumber) +
				".csv";

			while (FileExists(outputFilePath))
			{
				outputFilePath =
					".\\Assets\\Levels\\Room_Data\\" +
					GetSelectedBiomeName() +
					"_" +
					contentEditorState.exportFileName +
					"_" +
					std::to_string(++fileNumber) +
					".csv";
			}

			if (ExportCurrentEditorRoomToCsv(outputFilePath))
			{
				contentEditorState.statusMessage = "Exported: " + outputFilePath;
			}
			else
			{
				contentEditorState.statusMessage = "Export failed";
			}

			UpdateContentEditorUi();
		});
}

void ContentEditorInit()
{
}

void ContentEditorUpdate(float dt)
{
	UNREFERENCED_PARAMETER(dt);

	contentEditorUi.Update();

	Vector2 mouseWorldPosition = GetMouseWorldPosition();

	contentEditorState.hoveredCellIndex =
		GetEditorRoomCellIndexAtWorldPosition(mouseWorldPosition.x, mouseWorldPosition.y);

	if (contentEditorState.currentTab == ContentEditorTab::Obstacles &&
		AEInputCheckTriggered(AEVK_LBUTTON))
	{
		int firstPaletteIndex =
			contentEditorState.currentObstaclePalettePage * obstaclePaletteItemsPerPage;

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

			float thumbnailCenterX =
				obstacleThumbnailStartX + static_cast<float>(column) * obstacleThumbnailGapX;

			float thumbnailCenterY =
				obstacleThumbnailStartY - static_cast<float>(row) * obstacleThumbnailGapY;

			float halfThumbnailSize = 30.0f;

			if (mouseWorldPosition.x >= thumbnailCenterX - halfThumbnailSize &&
				mouseWorldPosition.x <= thumbnailCenterX + halfThumbnailSize &&
				mouseWorldPosition.y >= thumbnailCenterY - halfThumbnailSize &&
				mouseWorldPosition.y <= thumbnailCenterY + halfThumbnailSize)
			{
				contentEditorState.selectedTileId =
					contentEditorState.obstaclePalette[paletteIndex]->id;

				contentEditorState.statusMessage =
					"Selected tile id: " + std::to_string(contentEditorState.selectedTileId);

				UpdateContentEditorUi();
				return;
			}

			++visibleSlotIndex;
		}
	}

	if (contentEditorState.currentTab == ContentEditorTab::Obstacles &&
		contentEditorState.hoveredCellIndex >= 0)
	{
		int clickedRow =
			contentEditorState.hoveredCellIndex / contentEditorState.roomGrid.GetWidth();

		int clickedColumn =
			contentEditorState.hoveredCellIndex % contentEditorState.roomGrid.GetWidth();

		if (AEInputCheckCurr(AEVK_LBUTTON))
		{
			contentEditorState.roomGrid.SetCell(
				clickedRow,
				clickedColumn,
				contentEditorState.selectedTileId
			);
		}
		else if (AEInputCheckCurr(AEVK_RBUTTON))
		{
			contentEditorState.roomGrid.SetCell(clickedRow, clickedColumn, 0);
		}
	}

	if (AEInputCheckTriggered(AEVK_ESCAPE))
	{
		ChangeState(GS_MAINMENU);
	}
}

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
			DataLoader::CreateTexture(GetSelectedBiomeRoomTexturePath());

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

		float roomCellWidth =
			roomFloorWidth / static_cast<float>(contentEditorState.roomGrid.GetWidth());

		float roomCellHeight =
			roomFloorHeight / static_cast<float>(contentEditorState.roomGrid.GetHeight());

		float placedObstacleDrawSize = AEMin(roomCellWidth, roomCellHeight) * 0.72f;

		for (int row = 0; row < contentEditorState.roomGrid.GetHeight(); ++row)
		{
			for (int column = 0; column < contentEditorState.roomGrid.GetWidth(); ++column)
			{
				int currentTileId = contentEditorState.roomGrid.GetCell(column, row);

				if (currentTileId <= 0)
				{
					continue;
				}

				if (currentTileId >= 100)
				{
					continue;
				}

				TileType const* currentTileType = Grid::QueryTileType(currentTileId);
				if (!currentTileType)
				{
					continue;
				}

				if (currentTileType->asset.empty())
				{
					continue;
				}

				if (!FileExists(currentTileType->asset))
				{
					continue;
				}

				TexturedSprite obstacleSprite =
					DataLoader::CreateTexture(currentTileType->asset);

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
				int hoveredRow =
					contentEditorState.hoveredCellIndex / contentEditorState.roomGrid.GetWidth();

				int hoveredColumn =
					contentEditorState.hoveredCellIndex % contentEditorState.roomGrid.GetWidth();

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
}

void ContentEditorFree()
{
}

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

