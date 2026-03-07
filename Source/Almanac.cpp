#include <string>
#include <iostream>
#include <fstream>
#include "Almanac.h"
#include "Enemy.h"
#include "Loaders/DataLoader.h"
#include "Utils/Vector2.hpp"
#include "Utils/Utils.h"
#include "json/json.h"

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

AlmanacEntry::AlmanacEntry(EnemyType enemyType, std::string description, std::string area, 
	TexturedSprite enemyEntrySprite, Vector2 pictureScale) :
	enemyType { enemyType }, 
	description { description },
	area { area },
	enemyEntrySprite { enemyEntrySprite },
	pictureScale { pictureScale },
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
	almanac.pageSprites.push_back(DataLoader::CreateTexture("Assets/AlmanacScreen/openAlmanacPlant.png"));
	almanac.pageSprites.push_back(DataLoader::CreateTexture("Assets/AlmanacScreen/openAlmanacOcean.png"));
	almanac.pageSprites.push_back(DataLoader::CreateTexture("Assets/AlmanacScreen/openAlmanacCold.png"));
	almanac.pageSprites.push_back(DataLoader::CreateTexture("Assets/AlmanacScreen/openAlmanacHot.png"));

	almanac.pageSprites.push_back(DataLoader::CreateTexture("Assets/AlmanacScreen/openAlmanacMainLitUp.png"));
	almanac.pageSprites.push_back(DataLoader::CreateTexture("Assets/AlmanacScreen/openAlmanacNormalLitUp.png"));
	almanac.pageSprites.push_back(DataLoader::CreateTexture("Assets/AlmanacScreen/openAlmanacPlantLitUp.png"));
	almanac.pageSprites.push_back(DataLoader::CreateTexture("Assets/AlmanacScreen/openAlmanacOceanLitUp.png"));
	almanac.pageSprites.push_back(DataLoader::CreateTexture("Assets/AlmanacScreen/openAlmanacColdLitUp.png"));
	almanac.pageSprites.push_back(DataLoader::CreateTexture("Assets/AlmanacScreen/openAlmanacHotLitUp.png"));


	almanac.arrowSprites.push_back(DataLoader::CreateTexture("Assets/AlmanacScreen/openAlmanacArrowsFirstPage.png"));
	almanac.arrowSprites.push_back(DataLoader::CreateTexture("Assets/AlmanacScreen/openAlmanacArrows.png"));
	almanac.arrowSprites.push_back(DataLoader::CreateTexture("Assets/AlmanacScreen/openAlmanacArrowsLastPage.png"));
	almanac.arrowSprites.push_back(DataLoader::CreateTexture("Assets/AlmanacScreen/openAlmanacBackLitUp.png"));
	almanac.arrowSprites.push_back(DataLoader::CreateTexture("Assets/AlmanacScreen/openAlmanacNextLitUp.png"));

	almanac.closeSprites.push_back(DataLoader::CreateTexture("Assets/AlmanacScreen/openAlmanacClose.png"));
	almanac.closeSprites.push_back(DataLoader::CreateTexture("Assets/AlmanacScreen/openAlmanacCloseLitUp.png"));

	for (int i {0}; i < 12; ++i)
	{
		almanac.pageSprites[i].scale = Vector2(1600.f, 900.f);
		almanac.pageSprites[i].UpdateTransform();
		if (i < 5)
		{
			almanac.arrowSprites[i].scale = Vector2(1600.f, 900.f);
			almanac.arrowSprites[i].UpdateTransform();
		}
		if (i < 2)
		{
			almanac.closeSprites[i].scale = Vector2(1600.f, 900.f);
			almanac.closeSprites[i].UpdateTransform();
		}
	}

}

//void LoadAlmanacEntries(Almanac & almanac, std::vector<EnemyType> allEnemyTypes)
//{
//	almanac.entries.clear();
//
//	//static Json::Value almanacJsonContents;
//	//std::ifstream {"Assets/almanac.json"};
//
//
//
//	//Entries are now added like this for now
//	almanac.entries.push_back(AlmanacEntry(allEnemyTypes[0], "AAAAAAAAAA", "Normal", 
//		DataLoader::CreateTexture("Assets/poprocks.png")));
//	almanac.entries.push_back(AlmanacEntry(allEnemyTypes[0], "BBBBBBBBBB", "Plant",
//		DataLoader::CreateTexture("Assets/pattyfish.png")));
//
//	for (int i{ 0 }; i < 2; ++i)
//	{
//		//almanac.entries[i].enemyEntrySprite.scale = Vector2(1600.f, 900.f);
//		almanac.entries[i].enemyEntrySprite.position = Vector2(-320.f, 200.f);
//		almanac.entries[i].enemyEntrySprite.UpdateTransform();
//	}
//
//	std::cout << almanac.entries[0].enemyType.name << ", " << 
//		almanac.entries[0].description << ", " << almanac.entries[0].area << "\n";
//}

void LoadAlmanacEntries(Almanac& almanac, std::vector<std::string> allEnemyTypes)
{
	almanac.entries.clear();

	//static Json::Value almanacJsonContents;
	//std::ifstream {"Assets/almanac.json"};

	almanac.entries = DataLoader::GetAlmanacVector();

	//for (std::string name : allEnemyTypes)
	//{
	//	EnemyType tmp{ DataLoader::GetEnemyType(name) };
	//	almanac.entries.push_back(AlmanacEntry(tmp, "AAAAAAAAAAAAAA", "Normal",
	//		DataLoader::CreateTexture("Assets/poprocks.png")));
	//}

	//Entries are now added like this for now
	//EnemyType tmp { DataLoader::GetEnemyType("Pop Rocks")};
	//almanac.entries.push_back(AlmanacEntry(tmp, "AAAAAAAAAAAAAA", "Normal",
	//	DataLoader::CreateTexture("Assets/poprocks.png")));
	//tmp = DataLoader::GetEnemyType("Bouncy Spikes");
	//almanac.entries.push_back(AlmanacEntry(tmp, "BBBBBBBBBBBBBB", "Plant",
	//	DataLoader::CreateTexture("Assets/pattyfish.png")));

	for (int i{ 0 }; i < 3; ++i)
	{
		//almanac.entries[i].enemyEntrySprite.scale = Vector2(1600.f, 900.f);
		//almanac.entries[i].enemyEntrySprite.position = Vector2(-320.f, 200.f);
		almanac.entries[i].enemyEntrySprite.UpdateTransform();
	}

	std::cout << almanac.entries[0].enemyType.name << ", " <<
		almanac.entries[0].description << ", " << almanac.entries[0].area << "\n";
}

//Only called in RenderAlmanacPages
void RenderCurrentPage(Almanac & almanac, s8 font)
{

	if (almanac.currentPageNumber < 4 && almanac.currentPageNumber > 0)
	{
		AlmanacEntry thisEntry { almanac.entries[almanac.currentPageNumber - 1] };
		const char * text { thisEntry.enemyType.name.c_str() };
		f32 textWidth, textHeight;
		AEGfxGetPrintSize(font, text, 1.f, &textWidth, &textHeight);
		AEGfxPrint(font, text, -textWidth / 2 - 0.4, -textHeight / 2 + 0.13, 1.f, 0.f, 0.f, 0.f, 1.f);

		//text currently does not wrap, will make a function for that
		text = thisEntry.description.c_str() ;
		AEGfxGetPrintSize(font, text, 1.f, &textWidth, &textHeight);
		AEGfxPrint(font, text, -textWidth / 2 - 0.4, -textHeight / 2 - 0.3, 1.f, 0.f, 0.f, 0.f, 1.f);

		text = "Traits:";
		AEGfxGetPrintSize(font, text, 1.f, &textWidth, &textHeight);
		AEGfxPrint(font, "Traits:", 0.055, -textHeight / 2 + 0.7, 1.f, 0.f, 0.f, 0.f, 1.f);

		int index{ 0 };
		for (std::string trait : thisEntry.enemyType.traits)
		{
			std::string toPrint = "- " + trait;
			text = toPrint.c_str();
			//AEGfxGetPrintSize(font, text, 1.f, &textWidth, &textHeight);
			AEGfxPrint(font, text, 0.055 + (index / 3 * 0.4),
				0.555 - (index % 3 * 0.1), 
				0.8f, 0.f, 0.f, 0.f, 1.f);
			++index;
		}
		
		text = "Likes:";
		AEGfxGetPrintSize(font, text, 1.f, &textWidth, &textHeight);
		AEGfxPrint(font, "Likes:", 0.055, -textHeight / 2 + 0.225, 1.f, 0.f, 0.f, 0.f, 1.f);
		index = 0 ;
		for (std::string like : thisEntry.enemyType.likes)
		{
			std::string toPrint = "- " + like;
			text = toPrint.c_str();
			//AEGfxGetPrintSize(font, text, 1.f, &textWidth, &textHeight);
			AEGfxPrint(font, text, 0.055 + (index / 3 * 0.4), 
				0.09 - (index % 3 * 0.1), 
				0.8f, 0.f, 0.f, 0.f, 1.f);
			++index;
		}

		text = "Disikes:";
		AEGfxGetPrintSize(font, text, 1.f, &textWidth, &textHeight);
		AEGfxPrint(font, "Dislikes:", 0.055, -textHeight / 2 - 0.25, 1.f, 0.f, 0.f, 0.f, 1.f);

		index = 0;
		for (std::string dislike : thisEntry.enemyType.dislikes)
		{
			std::string toPrint = "- " + dislike;
			text = toPrint.c_str();
			//AEGfxGetPrintSize(font, text, 1.f, &textWidth, &textHeight);
			AEGfxPrint(font, text, 0.055 + (index / 3 * 0.4),
				- 0.395 - (index % 3 * 0.1),
				0.8f, 0.f, 0.f, 0.f, 1.f);
			++index;
		}

		char buffer[50];
		sprintf_s(buffer, 50, "%.0f", thisEntry.enemyType.health);
		AEGfxGetPrintSize(font, buffer, 1.f, &textWidth, &textHeight);
		AEGfxPrint(font, buffer, -textWidth / 2 - 0.555, -textHeight / 2 - 0.065, 1.f, 0.f, 0.f, 0.f, 1.f);

		sprintf_s(buffer, 50, "%.0f", thisEntry.enemyType.damage);
		AEGfxGetPrintSize(font, buffer, 1.f, &textWidth, &textHeight);
		AEGfxPrint(font, buffer, -textWidth / 2 - 0.27, -textHeight / 2 - 0.065, 1.f, 0.f, 0.f, 0.f, 1.f);

		thisEntry.enemyEntrySprite.position = Vector2(-320.f, 200.f);
		thisEntry.enemyEntrySprite.UpdateTransform();
		thisEntry.enemyEntrySprite.RenderSprite();
	}
	else if (0 == almanac.currentPageNumber)
	{
		const char* text{ "Almanac" };
		f32 textWidth, textHeight;
		AEGfxGetPrintSize(font, text, 2.f, &textWidth, &textHeight);
		AEGfxPrint(font, text, -textWidth / 2 - 0.4, -textHeight / 2 + 0.6, 2.f, 0.f, 0.f, 0.f, 1.f);

		int index {0};
		for (AlmanacEntry entry: almanac.entries)
		{
			entry.enemyEntrySprite.position = Vector2(-520 + (index % 3 * 190), 80 + (index / 3 * 190));
			entry.enemyEntrySprite.UpdateTransform();;
			entry.enemyEntrySprite.RenderSprite();
			++index;
		}
	}
}

void RenderAlmanacPages(Almanac & almanac, s8 font)
{
	AEGfxSetRenderMode(AE_GFX_RM_TEXTURE);
	if (almanac.isOpen)
	{
		almanac.pageSprites[almanac.currentPageNumber].RenderSprite();

		//render first page
		//hardcoded but will improve if i have time
		//if (0 == almanac.currentPageNumber) 
		//{
		//	almanac.pageSprites[0].RenderSprite();
		//}
		//else if (almanac.currentPageNumber > 0 && almanac.currentPageNumber < 2)
		//{
		//	almanac.pageSprites[1].RenderSprite();
		//}
		//else if (2 == almanac.currentPageNumber)
		//{
		//	almanac.pageSprites[2].RenderSprite();
		//}
		//else
		//{
		//	almanac.pageSprites[almanac.currentPageNumber].RenderSprite();
		//}

		//display close button when hovering mouse over it
		(IsCursorInRect(Vector2(577, 332), 72, 92)) ? almanac.closeSprites[1].RenderSprite() : almanac.closeSprites[0].RenderSprite();

		switch (almanac.currentPageNumber)
		{
			//first page
			case (0):
				almanac.arrowSprites[0].RenderSprite();
				//display lit up next when hovering mouse over it
				if (IsCursorInRect(Vector2(583, -352), 74, 85)) almanac.arrowSprites[4].RenderSprite();
				break;
			//last page
			case (5):
				almanac.arrowSprites[2].RenderSprite();
				//display lit up back when hovering mouse over it
				if (IsCursorInRect(Vector2(486, -352), 74, 85)) almanac.arrowSprites[3].RenderSprite();
				break;
			default:
				almanac.arrowSprites[1].RenderSprite();
				//display lit up next/back when hovering mouse over it
				if (IsCursorInRect(Vector2(583, -352), 74, 85)) almanac.arrowSprites[4].RenderSprite();
				if (IsCursorInRect(Vector2(486, -352), 74, 85)) almanac.arrowSprites[3].RenderSprite();

		}
		RenderCurrentPage(almanac, font);

		//display lit up bookmarks when hovering mouse over them
		for (int i{ 0 }; i < 6; ++i)
		{
			if (almanac.currentPageNumber != 5 - i && 
			IsCursorInRect(Vector2(683, (68 * i) - 47), 75, 57))
				almanac.pageSprites[5 - i + 6].RenderSprite();
		}
		//display lit up next when hovering mouse over them
		//if (IsCursorInRect(Vector2(583, -352), 74, 85)) almanac.arrowSprites[1].RenderSprite();
	}
}

void AlmanacInputs(Almanac & almanac , AEGfxVertexList* removeLater)
{
	if (almanac.isOpen)
	{ 
		AEGfxSetRenderMode(AE_GFX_RM_COLOR);

		//test for close
		//Sprite test1 {Sprite(removeLater, Vector2(577, 332), Vector2(72,92), Color{0.0,0.0,0.0,0.5})};
		//test1.UpdateTransform();
		//test1.RenderSprite(true);

		if (AEInputCheckTriggered(AEVK_LBUTTON) && IsCursorInRect(Vector2(577, 332), 72, 92))
		{
			almanac.isOpen = false;
			//std::cout << "current: " << almanac.currentPageNumber;
		}

		//test for next
		//Sprite test2{ Sprite(removeLater, Vector2(583, -352), Vector2(74,85), Color{0.0,0.0,0.0,0.5}) };
		//test2.UpdateTransform();
		//test2.RenderSprite(true);

		if (AEInputCheckTriggered(AEVK_LBUTTON) && IsCursorInRect(Vector2(583, -352), 74, 85))
		{
			if (almanac.currentPageNumber < 5) ++(almanac.currentPageNumber);
			//almanac.isOpen = false;
			std::cout << "current: " << almanac.currentPageNumber << "\n";
		}

		//test for back
		//Sprite test3{ Sprite(removeLater, Vector2(486, -352), Vector2(74,85), Color{0.0,0.0,0.0,0.5}) };
		//test3.UpdateTransform();
		//test3.RenderSprite(true);

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
			//Sprite test {Sprite(removeLater, Vector2(683, (68 * i) - 47), Vector2(75,57), Color{0.0,0.0,0.0,0.5})};
			//test.UpdateTransform();
			//test.RenderSprite(true);

			if (almanac.currentPageNumber != 5 - i && AEInputCheckTriggered(AEVK_LBUTTON) && 
				IsCursorInRect(Vector2(683, (68 * i) - 47), 75, 57))
			{
				almanac.currentPageNumber = 5 - i;
				std::cout << "current: " << almanac.currentPageNumber << "\n";
			}
		}

		if (0 == almanac.currentPageNumber)
		{
			for (int i{ 0 }; i < 3; ++i)
			{
				//test for area bookmarks
				//Sprite test {Sprite(removeLater, Vector2(-520 + (i % 3 * 190), 80 + (i / 3 * 190)), Vector2(160,160), Color{0.0,0.0,0.0,0.5})};
				//test.UpdateTransform();
				//test.RenderSprite(true);

				if (almanac.currentPageNumber != 5 - i && AEInputCheckTriggered(AEVK_LBUTTON) &&
					IsCursorInRect(Vector2(-520 + (i % 3 * 190), 80 + (i / 3 * 190)), 160, 160))
				{
					almanac.currentPageNumber = i + 1;
					std::cout << "current: " << almanac.currentPageNumber << "\n";
				}
			}
		}
	}
}


