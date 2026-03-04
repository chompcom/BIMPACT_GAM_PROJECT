#include <string>
#include <iostream>
#include "Almanac.h"
#include "Enemy.h"
#include "Loaders/DataLoader.h"
#include "Utils/Vector2.hpp"
#include "Utils/Utils.h"

Almanac::Almanac() :
	pageSprites{},
	entries{},
	isOpen{ false },
	currentPageNumber{ 0 },
	arrowSprites {},
	closeSprites {},
	maxPages{ 6 }
{

}

AlmanacEntry::AlmanacEntry(EnemyType enemyType, std::string description, std::string area, TexturedSprite enemyEntrySprite) :
	enemyType { enemyType }, 
	description { description },
	area { area },
	enemyEntrySprite { enemyEntrySprite },
	unlocked { false }
{

}

//void LoadAlmanacPages(std::vector<TexturedSprite> & almanacPageSprites)
//{
//	almanacPageSprites.push_back(DataLoader::CreateTexture("Assets/AlmanacScreen/openAlmanacNormal.png"));
//	almanacPageSprites[0].scale = Vector2(1600.f, 900.f);
//	almanacPageSprites[0].UpdateTransform();
//	almanacPageSprites.push_back(DataLoader::CreateTexture("Assets/AlmanacScreen/openAlmanacPlant.png"));
//	almanacPageSprites[1].scale = Vector2(1600.f,900.f);
//	almanacPageSprites[1].UpdateTransform();
//
//	if (!almanacPageSprites.empty()) std::cout << almanacPageSprites[0].scale.x;
//}


void LoadAlmanacPages(Almanac & almanac)
{
	almanac.pageSprites.push_back(DataLoader::CreateTexture("Assets/AlmanacScreen/openAlmanacMain.png"));
	almanac.pageSprites.push_back(DataLoader::CreateTexture("Assets/AlmanacScreen/openAlmanacNormal.png"));
	//almanac.pageSprites[0].scale = Vector2(1600.f, 900.f);
	//almanac.pageSprites[0].UpdateTransform();
	almanac.pageSprites.push_back(DataLoader::CreateTexture("Assets/AlmanacScreen/openAlmanacPlant.png"));
	//almanac.pageSprites[1].scale = Vector2(1600.f, 900.f);
	//almanac.pageSprites[1].UpdateTransform();
	almanac.pageSprites.push_back(DataLoader::CreateTexture("Assets/AlmanacScreen/openAlmanacOcean.png"));
	//almanac.pageSprites[2].scale = Vector2(1600.f, 900.f);
	//almanac.pageSprites[2].UpdateTransform();
	almanac.pageSprites.push_back(DataLoader::CreateTexture("Assets/AlmanacScreen/openAlmanacCold.png"));
	//almanac.pageSprites[3].scale = Vector2(1600.f, 900.f);
	//almanac.pageSprites[3].UpdateTransform();
	almanac.pageSprites.push_back(DataLoader::CreateTexture("Assets/AlmanacScreen/openAlmanacHot.png"));
	//almanac.pageSprites[4].scale = Vector2(1600.f, 900.f);
	//almanac.pageSprites[4].UpdateTransform();

	almanac.arrowSprites.push_back(DataLoader::CreateTexture("Assets/AlmanacScreen/openAlmanacArrowsFirstPage.png"));
	almanac.arrowSprites.push_back(DataLoader::CreateTexture("Assets/AlmanacScreen/openAlmanacArrows.png"));
	almanac.arrowSprites.push_back(DataLoader::CreateTexture("Assets/AlmanacScreen/openAlmanacArrowsLastPage.png"));

	almanac.closeSprites.push_back(DataLoader::CreateTexture("Assets/AlmanacScreen/openAlmanacClose.png"));

	almanac.closeSprites[0].scale = Vector2(1600.f, 900.f);
	almanac.closeSprites[0].UpdateTransform();

	for (int i {0}; i < almanac.maxPages; ++i)
	{
		almanac.pageSprites[i].scale = Vector2(1600.f, 900.f);
		almanac.pageSprites[i].UpdateTransform();
		if (i < 3)
		{
			almanac.arrowSprites[i].scale = Vector2(1600.f, 900.f);
			almanac.arrowSprites[i].UpdateTransform();
		}
	}

}

void LoadAlmanacEntries(Almanac & almanac, std::vector<EnemyType> allEnemyTypes)
{
	almanac.entries.clear();

	//Entries are now added like this for now
	almanac.entries.push_back(AlmanacEntry(allEnemyTypes[0], "AAAAAAAAAA", "Normal", 
		DataLoader::CreateTexture("Assets/poprocks.png")));
	almanac.entries.push_back(AlmanacEntry(allEnemyTypes[0], "BBBBBBBBBB", "Plant",
		DataLoader::CreateTexture("Assets/pattyfish.png")));

	for (int i{ 0 }; i < 2; ++i)
	{
		//almanac.entries[i].enemyEntrySprite.scale = Vector2(1600.f, 900.f);
		almanac.entries[i].enemyEntrySprite.position = Vector2(-320.f, 200.f);
		almanac.entries[i].enemyEntrySprite.UpdateTransform();
	}

	std::cout << almanac.entries[0].enemyType.name << ", " << 
		almanac.entries[0].description << ", " << almanac.entries[0].area << "\n";
}

void RenderCurrentPage(Almanac & almanac)
{
	if (almanac.currentPageNumber < 3 && almanac.currentPageNumber > 0)
	{
		
		almanac.entries[almanac.currentPageNumber - 1].enemyEntrySprite.RenderSprite();
	}
}

void RenderAlmanacPages(Almanac & almanac)
{
	AEGfxSetRenderMode(AE_GFX_RM_TEXTURE);
	if (almanac.isOpen)
	{
		if (almanac.currentPageNumber < 6) almanac.pageSprites[almanac.currentPageNumber].RenderSprite();
		almanac.closeSprites[0].RenderSprite();
		switch (almanac.currentPageNumber)
		{
			case (0):
				almanac.arrowSprites[0].RenderSprite();
				break;
			case (5):
				almanac.arrowSprites[2].RenderSprite();
				break;
			default:
				almanac.arrowSprites[1].RenderSprite();
		}
		RenderCurrentPage(almanac);
	}
}

void AlmanacInputs(Almanac & almanac , AEGfxVertexList* removeLater)
{
	AEGfxSetRenderMode(AE_GFX_RM_COLOR);

	//test for close
	Sprite test1 {Sprite(removeLater, Vector2(577, 332), Vector2(72,92), Color{0.0,0.0,0.0,0.5})};
	test1.UpdateTransform();
	test1.RenderSprite(true);

	if (AEInputCheckTriggered(AEVK_LBUTTON) && IsCursorInRect(Vector2(577, 332), 72, 92))
	{
		almanac.isOpen = false;
		//std::cout << "current: " << almanac.currentPageNumber;
	}

	//test for next
	Sprite test2{ Sprite(removeLater, Vector2(583, -352), Vector2(74,85), Color{0.0,0.0,0.0,0.5}) };
	test2.UpdateTransform();
	test2.RenderSprite(true);

	if (AEInputCheckTriggered(AEVK_LBUTTON) && IsCursorInRect(Vector2(583, -352), 74, 85))
	{
		++(almanac.currentPageNumber);
		//almanac.isOpen = false;
		std::cout << "current: " << almanac.currentPageNumber << "\n";
	}

	//test for back
	Sprite test3{ Sprite(removeLater, Vector2(486, -352), Vector2(74,85), Color{0.0,0.0,0.0,0.5}) };
	test3.UpdateTransform();
	test3.RenderSprite(true);

	if (AEInputCheckTriggered(AEVK_LBUTTON) && IsCursorInRect(Vector2(486, -352), 74, 85))
	{
		if (almanac.currentPageNumber > 0) --(almanac.currentPageNumber);
		//almanac.isOpen = false;
		std::cout << "current: " << almanac.currentPageNumber << "\n";
	}

	//note: goes from bottom to top
	for (int i{ 0 }; i < 6; ++i)
	{
		//test for area bookmarks
		Sprite test {Sprite(removeLater, Vector2(683, (68 * i) - 47), Vector2(75,57), Color{0.0,0.0,0.0,0.5})};
		test.UpdateTransform();
		test.RenderSprite(true);

		if (AEInputCheckTriggered(AEVK_LBUTTON) && IsCursorInRect(Vector2(683, (68 * i) - 47), 75, 57))
		{
			almanac.currentPageNumber = 5 - i;
			std::cout << "current: " << almanac.currentPageNumber << "\n";
		}
	}
}


