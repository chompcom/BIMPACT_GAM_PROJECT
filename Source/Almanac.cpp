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
	currentPageNumber{ 16 },
	currentArea { "main" },
	arrowSprites {},
	closeSprites {},
	maxPages{ 17 },
	hasBeenOpened{ false }
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

//Loading the sprites for the almanac and setting their transforms
void LoadAlmanacPages(Almanac & almanac)
{
	//page sprites for main UI for each area
	almanac.pageSprites.push_back(DataLoader::CreateTexture("Assets/AlmanacScreen/openAlmanacMain.png"));
	almanac.pageSprites.push_back(DataLoader::CreateTexture("Assets/AlmanacScreen/openAlmanacNormal.png"));
	almanac.pageSprites.push_back(DataLoader::CreateTexture("Assets/AlmanacScreen/openAlmanacPlant.png"));
	almanac.pageSprites.push_back(DataLoader::CreateTexture("Assets/AlmanacScreen/openAlmanacOcean.png"));
	almanac.pageSprites.push_back(DataLoader::CreateTexture("Assets/AlmanacScreen/openAlmanacCold.png"));
	almanac.pageSprites.push_back(DataLoader::CreateTexture("Assets/AlmanacScreen/openAlmanacHot.png"));

	//page sprites for lit up bookmarks for each area
	almanac.pageSprites.push_back(DataLoader::CreateTexture("Assets/AlmanacScreen/openAlmanacMainLitUp.png"));
	almanac.pageSprites.push_back(DataLoader::CreateTexture("Assets/AlmanacScreen/openAlmanacNormalLitUp.png"));
	almanac.pageSprites.push_back(DataLoader::CreateTexture("Assets/AlmanacScreen/openAlmanacPlantLitUp.png"));
	almanac.pageSprites.push_back(DataLoader::CreateTexture("Assets/AlmanacScreen/openAlmanacOceanLitUp.png"));
	almanac.pageSprites.push_back(DataLoader::CreateTexture("Assets/AlmanacScreen/openAlmanacColdLitUp.png"));
	almanac.pageSprites.push_back(DataLoader::CreateTexture("Assets/AlmanacScreen/openAlmanacHotLitUp.png"));

	//tutorial page sprites
	almanac.pageSprites.push_back(DataLoader::CreateTexture("Assets/AlmanacScreen/openAlmanacHelp1.png"));
	almanac.pageSprites.push_back(DataLoader::CreateTexture("Assets/AlmanacScreen/openAlmanacHelp2.png"));
	almanac.pageSprites.push_back(DataLoader::CreateTexture("Assets/AlmanacScreen/openAlmanacHelpLitUp.png"));


	//page arrow sprites
	almanac.arrowSprites.push_back(DataLoader::CreateTexture("Assets/AlmanacScreen/openAlmanacArrowsFirstPage.png"));
	almanac.arrowSprites.push_back(DataLoader::CreateTexture("Assets/AlmanacScreen/openAlmanacArrows.png"));
	almanac.arrowSprites.push_back(DataLoader::CreateTexture("Assets/AlmanacScreen/openAlmanacArrowsLastPage.png"));
	almanac.arrowSprites.push_back(DataLoader::CreateTexture("Assets/AlmanacScreen/openAlmanacBackLitUp.png"));
	almanac.arrowSprites.push_back(DataLoader::CreateTexture("Assets/AlmanacScreen/openAlmanacNextLitUp.png"));

	//close button sprites
	almanac.closeSprites.push_back(DataLoader::CreateTexture("Assets/AlmanacScreen/openAlmanacClose.png"));
	almanac.closeSprites.push_back(DataLoader::CreateTexture("Assets/AlmanacScreen/openAlmanacCloseLitUp.png"));

	//updating transform matrices for the almanac's sprites
	for (int i {0}; i < 15; ++i)
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

//fill up the entries vector via the data loader
void LoadAlmanacEntries(Almanac& almanac)
{
	almanac.entries.clear();
	almanac.entries = DataLoader::GetAlmanacVector();
}

//Only called in RenderAlmanacPages, renders the appropriate items ON the current page
void RenderCurrentPage(Almanac & almanac, s8 font)
{
	//text rgb values
	float r {105.f / 255.f}, g {87.f / 255.f}, b{61.f / 255.f};
	//std::cout << almanac.currentPageNumber << " " << almanac.entries.size() << std::endl;
	//figure out which area of the almanac we are looking at
	/*almanac.currentArea = (almanac.currentPageNumber && almanac.currentPageNumber <= almanac.maxPages - 1) 
		? almanac.entries[almanac.currentPageNumber - 1].area : "main";*/
	

	//entries of each creature
	if (almanac.currentPageNumber <= almanac.maxPages - 2 && almanac.currentPageNumber > 0)
	{
		AlmanacEntry thisEntry{ almanac.entries[almanac.currentPageNumber - 1] };
		const char * text { thisEntry.enemyType.name.c_str() };
		f32 textWidth, textHeight;
		AEGfxGetPrintSize(font, text, 1.f, &textWidth, &textHeight);
		AEGfxPrint(font, text, -textWidth / 2 - 0.4f, -textHeight / 2 + 0.13f, 1.f, r, g, b, 1.f);

		//NOTE: text currently does NOT wrap if too long, will do something about it if we need longer descriptions
		text = thisEntry.description.c_str() ;
		AEGfxGetPrintSize(font, text, 1.f, &textWidth, &textHeight);
		AEGfxPrint(font, text, -textWidth / 2 - 0.4f, -textHeight / 2 - 0.3f, 1.f, r, g, b, 1.f);

		//Print traits
		text = "Traits:";
		AEGfxGetPrintSize(font, text, 1.f, &textWidth, &textHeight);
		AEGfxPrint(font, "Traits:", 0.055f, -textHeight / 2 + 0.7f, 1.f, r, g, b, 1.f);
		int index{ 0 };
		for (std::string trait : thisEntry.enemyType.traits)
		{
			std::string toPrint = "- " + trait;
			text = toPrint.c_str();
			AEGfxPrint(font, text, 0.055f + (index / 3 * 0.4f),
				0.555f - (index % 3 * 0.1f), 
				0.8f, r, g, b, 1.f);
			++index;
		}
		
		//Print likes
		text = "Likes:";
		AEGfxGetPrintSize(font, text, 1.f, &textWidth, &textHeight);
		AEGfxPrint(font, "Likes:", 0.055f, -textHeight / 2 + 0.225f, 1.f, r, g, b, 1.f);
		index = 0 ;
		for (std::string like : thisEntry.enemyType.likes)
		{
			std::string toPrint = "- " + like;
			text = toPrint.c_str();
			AEGfxPrint(font, text, 0.055f + (index / 3 * 0.4f), 
				0.09f - (index % 3 * 0.1f), 
				0.8f, r, g, b, 1.f);
			++index;
		}

		//Print dislikes
		text = "Disikes:";
		AEGfxGetPrintSize(font, text, 1.f, &textWidth, &textHeight);
		AEGfxPrint(font, "Dislikes:", 0.055f, -textHeight / 2 - 0.25f, 1.f, r, g, b, 1.f);
		index = 0;
		for (std::string dislike : thisEntry.enemyType.dislikes)
		{
			std::string toPrint = "- " + dislike;
			text = toPrint.c_str();
			AEGfxPrint(font, text, 0.055f + (index / 3 * 0.4f),
				- 0.395f - (index % 3 * 0.1f),
				0.8f, r, g, b, 1.f);
			++index;
		}

		//Print health and damage
		//changed to speed and damage?
		char buffer[50];
		/*sprintf_s(buffer, 50, "%.0f", thisEntry.enemyType.health);
		AEGfxGetPrintSize(font, buffer, 1.f, &textWidth, &textHeight);
		AEGfxPrint(font, buffer, -textWidth / 2 - 0.27f, -textHeight / 2 - 0.065f, 1.f, r, g, b, 1.f);*/
		sprintf_s(buffer, 50, "%.0f", thisEntry.enemyType.damage);
		AEGfxGetPrintSize(font, buffer, 1.f, &textWidth, &textHeight);
		AEGfxPrint(font, buffer, -textWidth / 2 - 0.555f, -textHeight / 2 - 0.065f, 1.f, r, g, b, 1.f);

		sprintf_s(buffer, 50, "%.0f", thisEntry.enemyType.speed);
		AEGfxGetPrintSize(font, buffer, 1.f, &textWidth, &textHeight);
		AEGfxPrint(font, buffer, -textWidth / 2 - 0.27f, -textHeight / 2 - 0.065f, 1.f, r, g, b, 1.f);

		//Print entry number
		sprintf_s(buffer, 50, "%d", almanac.currentPageNumber);
		AEGfxGetPrintSize(font, buffer, 1.f, &textWidth, &textHeight);
		AEGfxPrint(font, buffer, -textWidth / 2 - 0.72f, -textHeight / 2 - 0.55f, 1.f, r, g, b, 1.f);

		//Render enemy sprite
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
		AEGfxPrint(font, text, -textWidth / 2 - 0.4f, -textHeight / 2 + 0.6f, 2.f, r, g, b, 1.f);

		//const char* text{ "Tutorial" };
		//f32 textWidth, textHeight;
		//AEGfxGetPrintSize(font, text, 2.f, &textWidth, &textHeight);
		//AEGfxPrint(font, text, -textWidth / 2 - 0.4, -textHeight / 2 + 0.6, 2.f, r, g, b, 1.f);

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
	//tutorial page 1
	else if (16 == almanac.currentPageNumber)
	{
		const char* text { "Tutorial" };
		f32 textWidth, textHeight;
		AEGfxGetPrintSize(font, text, 2.f, &textWidth, &textHeight);
		AEGfxPrint(font, text, -textWidth / 2 - 0.4f, -textHeight / 2 + 0.6f, 2.f, r, g, b, 1.f);

		text = "Use W, A, S, D to move!";
		AEGfxGetPrintSize(font, text, 1.f, &textWidth, &textHeight);
		AEGfxPrint(font, text, -textWidth / 2 - 0.4f, -textHeight / 2 - 0.5f, 1.f, r, g, b, 1.f);

		text = "Pick up gifts by touching";
		AEGfxGetPrintSize(font, text, 1.f, &textWidth, &textHeight);
		AEGfxPrint(font, text, -textWidth / 2 + 0.41f, -textHeight / 2 - 0.3f, 1.f, r, g, b, 1.f);

		text = "them!";
		AEGfxGetPrintSize(font, text, 1.f, &textWidth, &textHeight);
		AEGfxPrint(font, text, -textWidth / 2 + 0.41f, -textHeight / 2 - 0.4f, 1.f, r, g, b, 1.f);
	}
	//tutorial page 1
	else if (17 == almanac.currentPageNumber)
	{
		const char* text{ "Press or hold" };
		f32 textWidth, textHeight;
		AEGfxGetPrintSize(font, text, 1.f, &textWidth, &textHeight);
		AEGfxPrint(font, text, -textWidth / 2 - 0.56f, -textHeight / 2 - 0.22f, 1.f, r, g, b, 1.f);

		text = "to throw a gift";
		AEGfxGetPrintSize(font, text, 1.f, &textWidth, &textHeight);
		AEGfxPrint(font, text, -textWidth / 2 - 0.4f, -textHeight / 2 - 0.35f, 1.f, r, g, b, 1.f);

		text = "Find out your enemies' liked";
		AEGfxGetPrintSize(font, text, 1.f, &textWidth, &textHeight);
		AEGfxPrint(font, text, -textWidth / 2 + 0.41f, -textHeight / 2 + 0.25f, 1.f, r, g, b, 1.f);

		text = "gifts to befriend them";
		AEGfxGetPrintSize(font, text, 1.f, &textWidth, &textHeight);
		AEGfxPrint(font, text, -textWidth / 2 + 0.41f, -textHeight / 2 + 0.15f, 1.f, r, g, b, 1.f);

		text = "Be careful, don't give them";
		AEGfxGetPrintSize(font, text, 1.f, &textWidth, &textHeight);
		AEGfxPrint(font, text, -textWidth / 2 + 0.41f, -textHeight / 2 - 0.4f, 1.f, r, g, b, 1.f);

		text = "something they'd dislike!";
		AEGfxGetPrintSize(font, text, 1.f, &textWidth, &textHeight);
		AEGfxPrint(font, text, -textWidth / 2 + 0.41f, -textHeight / 2 - 0.5f, 1.f, r, g, b, 1.f);
	}
}

//Renders the book and its bookmarks/buttons, and calls RenderCurrentPage
void RenderAlmanacPages(Almanac & almanac, s8 font)
{
	//std::cout << "current: " << almanac.currentPageNumber << "\n";
	AEGfxSetRenderMode(AE_GFX_RM_TEXTURE);
	if (almanac.isOpen)
	{
		std::vector<std::string> allAreas {"main", "normal", "plant", "ocean", "cold", "hot"};
		
		//check which area we are looking at in the almanac, and render the appropriate sprite
		for (int i{ 0 }; i < 6; ++i)
		{
			if (allAreas[i] == almanac.currentArea)
				almanac.pageSprites[i].RenderSprite();
		}

		if (16 == almanac.currentPageNumber)
		{
			almanac.pageSprites[12].RenderSprite();
		}
		else if (17 == almanac.currentPageNumber)
		{
			almanac.pageSprites[13].RenderSprite();
		}

		//display close button and light it up when hovering mouse over it
		(IsCursorInRect(Vector2(577, 332), 72, 92)) ? almanac.closeSprites[1].RenderSprite() : almanac.closeSprites[0].RenderSprite();
		
		//render the current arrow sprite(s) depending on what page we are in
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
			case (17):
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

		//call this to figure out what contents on the page to render
		RenderCurrentPage(almanac, font);

		//display lit up bookmarks when hovering mouse over them
		for (int i{ 0 }; i < 6; ++i)
		{
			if (IsCursorInRect(Vector2(683, (68 * i) - 47), 75, 57))
			{
				if (almanac.currentArea != allAreas[5 - i])almanac.pageSprites[5 - i + 6].RenderSprite();
			}
		}
		//display lit up help bookmark when hovering mouse over it
		if (almanac.currentArea != "help" && IsCursorInRect(Vector2(683, -235), 75, 57))
		{
			almanac.pageSprites[14].RenderSprite();

		}
	}
}

//Handles the inputs in the almanac
void AlmanacInputs(Almanac & almanac/*, AEGfxVertexList* removeLater*/)
{
	if (almanac.isOpen)
	{ 
		AEGfxSetRenderMode(AE_GFX_RM_COLOR);

		//input for close
		//test for close
		//Sprite test1 {Sprite(removeLater, Vector2(577, 332), Vector2(72,92), Color{0.0,0.0,0.0,0.5})};
		//test1.UpdateTransform();
		//test1.RenderSprite(true);
		if (AEInputCheckTriggered(AEVK_LBUTTON) && IsCursorInRect(Vector2(577, 332), 72, 92))
		{
			almanac.isOpen = false;
			//std::cout << "current: " << almanac.currentPageNumber;
		}

		//input for next
		//test for next
		//Sprite test2{ Sprite(removeLater, Vector2(583, -352), Vector2(74,85), Color{0.0,0.0,0.0,0.5}) };
		//test2.UpdateTransform();
		//test2.RenderSprite(true);
		if (AEInputCheckTriggered(AEVK_LBUTTON) && IsCursorInRect(Vector2(583, -352), 74, 85))
		{
			if (almanac.currentPageNumber < almanac.maxPages) ++(almanac.currentPageNumber);
			//std::cout << "current: " << almanac.currentPageNumber << "\n";
		}

		//input for back
		//test for back
		//Sprite test3{ Sprite(removeLater, Vector2(486, -352), Vector2(74,85), Color{0.0,0.0,0.0,0.5}) };
		//test3.UpdateTransform();
		//test3.RenderSprite(true);
		if (AEInputCheckTriggered(AEVK_LBUTTON) && IsCursorInRect(Vector2(486, -352), 74, 85))
		{
			if (almanac.currentPageNumber > 0) --(almanac.currentPageNumber);
			//std::cout << "current: " << almanac.currentPageNumber << "\n";
		}

		//inputs for area bookmarks
		std::vector<std::string> allAreas{ "main", "normal", "plant", "ocean", "cold", "hot" };
		std::vector<int> bookmarkPages{0,1,5,8,11,13,16};
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
		//input for the help bookmark
		//Sprite test {Sprite(removeLater, Vector2(683, -235), Vector2(75,57), Color{0.0,0.0,0.0,0.5})};
		//test.UpdateTransform();
		//test.RenderSprite(true);
		if (almanac.currentArea != "help" && AEInputCheckTriggered(AEVK_LBUTTON) &&
			IsCursorInRect(Vector2(683, -235), 75, 57))
		{
			almanac.currentPageNumber = 16;
			//std::cout << "current: " << almanac.currentPageNumber << "\n";
		}

		//inputs for main page enemy buttons 
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

		//set current are depending on page number / current entry
		almanac.currentArea = (almanac.currentPageNumber > 0 && almanac.currentPageNumber <= almanac.maxPages - 2)
			? almanac.entries[almanac.currentPageNumber - 1].area :
			(almanac.currentPageNumber > 0) ? "help" : "main";
	}
}

void AlmanacInit(Almanac& almanac)
{
	almanac.isOpen = false;
	almanac.hasBeenOpened = false;
	almanac.currentPageNumber = 16;
}

void AlmanacFree(Almanac& almanac)
{
	almanac.pageSprites.clear();
	almanac.entries.clear();
	almanac.arrowSprites.clear();
	almanac.closeSprites.clear();
}

