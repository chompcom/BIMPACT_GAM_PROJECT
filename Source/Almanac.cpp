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
	currentArea { "main" },
	arrowSprites {},
	closeSprites {},
	maxPages{ 15 }
{

}

AlmanacEntry::AlmanacEntry(EnemyType enemyType, std::string description, std::string area, 
	TexturedSprite enemyEntrySprite) :
	enemyType { enemyType }, 
	description { description },
	area { area },
	enemyEntrySprite { enemyEntrySprite },
	unlocked { false }
{

}

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

	//updating transform matrices for the almanac's sprites
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

void LoadAlmanacEntries(Almanac& almanac)
{
	almanac.entries.clear();
	almanac.entries = DataLoader::GetAlmanacVector();
	std::cout << "Size of Almanac after loading" << almanac.entries.size() << "\n";
}

//Only called in RenderAlmanacPages, renders the appropriate items ON the current page
void RenderCurrentPage(Almanac & almanac, s8 font)
{
	float r {105.f / 255.f}, g {87.f / 255.f}, b{61.f / 255.f};
	std::cout << almanac.currentPageNumber << " " << almanac.entries.size() << std::endl;
	almanac.currentArea = (almanac.currentPageNumber) ? almanac.entries[almanac.currentPageNumber - 1].area : "main";
	//entries
	if (almanac.currentPageNumber <= almanac.maxPages && almanac.currentPageNumber > 0)
	{
		AlmanacEntry thisEntry{ almanac.entries[almanac.currentPageNumber - 1] };
		const char * text { thisEntry.enemyType.name.c_str() };
		f32 textWidth, textHeight;
		AEGfxGetPrintSize(font, text, 1.f, &textWidth, &textHeight);
		AEGfxPrint(font, text, -textWidth / 2 - 0.4, -textHeight / 2 + 0.13, 1.f, r, g, b, 1.f);

		//text currently does not wrap, will make a function for that
		text = thisEntry.description.c_str() ;
		AEGfxGetPrintSize(font, text, 1.f, &textWidth, &textHeight);
		AEGfxPrint(font, text, -textWidth / 2 - 0.4, -textHeight / 2 - 0.3, 1.f, r, g, b, 1.f);

		text = "Traits:";
		AEGfxGetPrintSize(font, text, 1.f, &textWidth, &textHeight);
		AEGfxPrint(font, "Traits:", 0.055, -textHeight / 2 + 0.7, 1.f, r, g, b, 1.f);

		int index{ 0 };
		for (std::string trait : thisEntry.enemyType.traits)
		{
			std::string toPrint = "- " + trait;
			text = toPrint.c_str();
			AEGfxPrint(font, text, 0.055 + (index / 3 * 0.4),
				0.555 - (index % 3 * 0.1), 
				0.8f, r, g, b, 1.f);
			++index;
		}
		
		text = "Likes:";
		AEGfxGetPrintSize(font, text, 1.f, &textWidth, &textHeight);
		AEGfxPrint(font, "Likes:", 0.055, -textHeight / 2 + 0.225, 1.f, r, g, b, 1.f);
		index = 0 ;
		for (std::string like : thisEntry.enemyType.likes)
		{
			std::string toPrint = "- " + like;
			text = toPrint.c_str();
			AEGfxPrint(font, text, 0.055 + (index / 3 * 0.4), 
				0.09 - (index % 3 * 0.1), 
				0.8f, r, g, b, 1.f);
			++index;
		}

		text = "Disikes:";
		AEGfxGetPrintSize(font, text, 1.f, &textWidth, &textHeight);
		AEGfxPrint(font, "Dislikes:", 0.055, -textHeight / 2 - 0.25, 1.f, r, g, b, 1.f);

		index = 0;
		for (std::string dislike : thisEntry.enemyType.dislikes)
		{
			std::string toPrint = "- " + dislike;
			text = toPrint.c_str();
			AEGfxPrint(font, text, 0.055 + (index / 3 * 0.4),
				- 0.395 - (index % 3 * 0.1),
				0.8f, r, g, b, 1.f);
			++index;
		}

		char buffer[50];
		sprintf_s(buffer, 50, "%.0f", thisEntry.enemyType.health);
		AEGfxGetPrintSize(font, buffer, 1.f, &textWidth, &textHeight);
		AEGfxPrint(font, buffer, -textWidth / 2 - 0.555, -textHeight / 2 - 0.065, 1.f, r, g, b, 1.f);

		sprintf_s(buffer, 50, "%.0f", thisEntry.enemyType.damage);
		AEGfxGetPrintSize(font, buffer, 1.f, &textWidth, &textHeight);
		AEGfxPrint(font, buffer, -textWidth / 2 - 0.27, -textHeight / 2 - 0.065, 1.f, r, g, b, 1.f);

		sprintf_s(buffer, 50, "%d", almanac.currentPageNumber);
		AEGfxGetPrintSize(font, buffer, 1.f, &textWidth, &textHeight);
		AEGfxPrint(font, buffer, -textWidth / 2 - 0.72, -textHeight / 2 - 0.55, 1.f, r, g, b, 1.f);

		thisEntry.enemyEntrySprite.position = Vector2(-320.f, 200.f);
		thisEntry.enemyEntrySprite.UpdateTransform();
		thisEntry.enemyEntrySprite.RenderSprite();
	}
	//main page
	else if (0 == almanac.currentPageNumber)
	{
		const char* text{ "Almanac" };
		f32 textWidth, textHeight;
		AEGfxGetPrintSize(font, text, 2.f, &textWidth, &textHeight);
		AEGfxPrint(font, text, -textWidth / 2 - 0.4, -textHeight / 2 + 0.6, 2.f, r, g, b, 1.f);

		int index {0};
		for (AlmanacEntry entry: almanac.entries)
		{
			if (index < 6) entry.enemyEntrySprite.position = Vector2(-520 + (index % 3 * 187), 80 - (index / 3 * 185));
			else entry.enemyEntrySprite.position = Vector2(140 + ((index - 6) % 3 * 187), 190 - ((index - 6) / 3 * 185));
			entry.enemyEntrySprite.UpdateTransform();
			entry.enemyEntrySprite.RenderSprite();
			++index;
		}
	}
}

//Renders the book and its bookmarks/buttons, and calls RenderCurrentPage
void RenderAlmanacPages(Almanac & almanac, s8 font)
{
	AEGfxSetRenderMode(AE_GFX_RM_TEXTURE);
	if (almanac.isOpen)
	{
		std::vector<std::string> allAreas {  "main", "normal", "plant", "ocean", "cold", "hot"};

		for (int i{ 0 }; i < 6; ++i)
		{
			if (allAreas[i] == almanac.currentArea)
				almanac.pageSprites[i].RenderSprite();
		}

		//display close button when hovering mouse over it
		(IsCursorInRect(Vector2(577, 332), 72, 92)) ? almanac.closeSprites[1].RenderSprite() : almanac.closeSprites[0].RenderSprite();

		switch (almanac.currentPageNumber)
		{
			//first page
			case (0):
				almanac.arrowSprites[0].RenderSprite();
				//display lit up next when hovering mouse over it
				if (IsCursorInRect(Vector2(583, -352), 74, 85))
					almanac.arrowSprites[4].RenderSprite();
				break;
			//last page
			case (15):
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
			if (IsCursorInRect(Vector2(683, (68 * i) - 47), 75, 57))
			{
				if (almanac.currentArea != allAreas[5 - i])almanac.pageSprites[5 - i + 6].RenderSprite();
			}
		}
	}
}

//Handles the inputs in the almanac
void AlmanacInputs(Almanac & almanac/*, AEGfxVertexList* removeLater*/)
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
			if (almanac.currentPageNumber < almanac.maxPages) ++(almanac.currentPageNumber);
			//std::cout << "current: " << almanac.currentPageNumber << "\n";
		}

		//test for back
		//Sprite test3{ Sprite(removeLater, Vector2(486, -352), Vector2(74,85), Color{0.0,0.0,0.0,0.5}) };
		//test3.UpdateTransform();
		//test3.RenderSprite(true);
		if (AEInputCheckTriggered(AEVK_LBUTTON) && IsCursorInRect(Vector2(486, -352), 74, 85))
		{
			if (almanac.currentPageNumber > 0) --(almanac.currentPageNumber);
			//std::cout << "current: " << almanac.currentPageNumber << "\n";
		}

		std::vector<std::string> allAreas{ "main", "normal", "plant", "ocean", "cold", "hot" };
		std::vector<int> bookmarkPages{0,1,5,8,11,13};
		//note: goes from bottom to top
		for (int i{ 0 }; i < 6; ++i)
		{
			//test for area bookmarks
			//Sprite test {Sprite(removeLater, Vector2(683, (68 * i) - 47), Vector2(75,57), Color{0.0,0.0,0.0,0.5})};
			//test.UpdateTransform();
			//test.RenderSprite(true);

			if (almanac.currentArea != allAreas[5 - i] && AEInputCheckTriggered(AEVK_LBUTTON) &&
				IsCursorInRect(Vector2(683, (68 * i) - 47), 75, 57))
			{
				almanac.currentPageNumber = bookmarkPages[5 - i];
				//std::cout << "current: " << almanac.currentPageNumber << "\n";
			}
		}

		if (0 == almanac.currentPageNumber)
		{
			for (int i{ 0 }; i < almanac.maxPages; ++i)
			{
				//test for main page enemy buttons
				//Sprite test { (i < 6) ?
				//Sprite(removeLater, Vector2(-520 + (i % 3 * 187), 80 - (i / 3 * 185)), Vector2(160,160), Color{0.0,0.0,0.0,0.5}) :
				//Sprite(removeLater, Vector2(140 + ((i - 6) % 3 * 187), 190 - ((i - 6) / 3 * 185)), Vector2(160,160), Color{0.0,0.0,0.0,0.5}) };
				//test.UpdateTransform();
				//test.RenderSprite(true);

				if (AEInputCheckTriggered(AEVK_LBUTTON) && ((i < 6) ?
					IsCursorInRect(Vector2(-520 + (i % 3 * 187), 80 - (i / 3 * 185)), 160, 160) :
					IsCursorInRect(Vector2(140 + ((i - 6) % 3 * 187), 190 - ((i - 6) / 3 * 185)), 160, 160)))
				{
					almanac.currentPageNumber = i + 1;
					std::cout << "current: " << almanac.currentPageNumber << "\n";
				}
			}
		}
	}
}


