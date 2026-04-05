/* Start Header ************************************************************************/
/*!
\file     Almanac.cpp
\author   Lee Hng Hwee Celest, hnghweecelest.l, 2502385
\par      hnghweecelest.l@digipen.edu
\brief    This file defines some functions that implement the almanac

Copyright (C) 2026 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited.
*/
/* End Header **************************************************************************/

#include <string>
#include <iostream>
#include <fstream>
#include "Almanac.h"
#include "Enemy.h"
#include "Loaders/DataLoader.h"
#include "Utils/Vector2.hpp"
#include "Utils/Utils.h"
#include "json/json.h"

//Constructor for the almanac
Almanac::Almanac() :
	pageSprites{},
	entries{},
	isOpen{ false },
	currentPageNumber{ 0 },
	currentArea { "help" },
	arrowSprites {},
	closeSprites {},
	maxPages{ 18 },
	hasBeenOpened{ false }
{

}

//Constructor for the almanac entry
AlmanacEntry::AlmanacEntry(EnemyType enemyType, std::string description, std::string area, 
	TexturedSprite enemyEntrySprite) :
	enemyType { enemyType }, 
	description { description },
	area { area },
	enemyEntrySprite { enemyEntrySprite },
	unlocked { false }
{

}

//anonymous namespace for variables that will be needed in this file
namespace {
	//all areas except boss and tutorial
	std::vector<std::string> allAreas{ "main", "normal", "plant", "ocean", "cold", "hot" };
	//the page number each bookmark brings the player to
	std::vector<int> bookmarkPages{ 3,4,8,10,13,15 };
	static bool almanacSound = false;
}


//Loading the sprites for the almanac and setting their transforms
void LoadAlmanacPages(Almanac & almanac)
{
	//create page sprites for main UI for each main area (excluding boss and tutorial)
	almanac.pageSprites.push_back(DataLoader::CreateTexture("Assets/AlmanacScreen/openAlmanacMain.png"));
	almanac.pageSprites.push_back(DataLoader::CreateTexture("Assets/AlmanacScreen/openAlmanacNormal.png"));
	almanac.pageSprites.push_back(DataLoader::CreateTexture("Assets/AlmanacScreen/openAlmanacPlant.png"));
	almanac.pageSprites.push_back(DataLoader::CreateTexture("Assets/AlmanacScreen/openAlmanacOcean.png"));
	almanac.pageSprites.push_back(DataLoader::CreateTexture("Assets/AlmanacScreen/openAlmanacCold.png"));
	almanac.pageSprites.push_back(DataLoader::CreateTexture("Assets/AlmanacScreen/openAlmanacHot.png"));

	//create page sprites for lit up bookmarks for each area (excluding boss and tutorial)
	almanac.pageSprites.push_back(DataLoader::CreateTexture("Assets/AlmanacScreen/openAlmanacMainLitUp.png"));
	almanac.pageSprites.push_back(DataLoader::CreateTexture("Assets/AlmanacScreen/openAlmanacNormalLitUp.png"));
	almanac.pageSprites.push_back(DataLoader::CreateTexture("Assets/AlmanacScreen/openAlmanacPlantLitUp.png"));
	almanac.pageSprites.push_back(DataLoader::CreateTexture("Assets/AlmanacScreen/openAlmanacOceanLitUp.png"));
	almanac.pageSprites.push_back(DataLoader::CreateTexture("Assets/AlmanacScreen/openAlmanacColdLitUp.png"));
	almanac.pageSprites.push_back(DataLoader::CreateTexture("Assets/AlmanacScreen/openAlmanacHotLitUp.png"));

	//create tutorial page sprites
	almanac.pageSprites.push_back(DataLoader::CreateTexture("Assets/AlmanacScreen/openAlmanacHelp1.png"));
	almanac.pageSprites.push_back(DataLoader::CreateTexture("Assets/AlmanacScreen/openAlmanacHelp2.png"));
	almanac.pageSprites.push_back(DataLoader::CreateTexture("Assets/AlmanacScreen/openAlmanacHelp3.png"));
	almanac.pageSprites.push_back(DataLoader::CreateTexture("Assets/AlmanacScreen/openAlmanacHelpLitUp.png"));

	//create boss page sprites
	almanac.pageSprites.push_back(DataLoader::CreateTexture("Assets/AlmanacScreen/openAlmanacBoss.png"));
	almanac.pageSprites.push_back(DataLoader::CreateTexture("Assets/AlmanacScreen/openAlmanacBossLitUp.png"));

	//create page arrow sprites
	almanac.arrowSprites.push_back(DataLoader::CreateTexture("Assets/AlmanacScreen/openAlmanacArrowsFirstPage.png"));
	almanac.arrowSprites.push_back(DataLoader::CreateTexture("Assets/AlmanacScreen/openAlmanacArrows.png"));
	almanac.arrowSprites.push_back(DataLoader::CreateTexture("Assets/AlmanacScreen/openAlmanacArrowsLastPage.png"));
	almanac.arrowSprites.push_back(DataLoader::CreateTexture("Assets/AlmanacScreen/openAlmanacBackLitUp.png"));
	almanac.arrowSprites.push_back(DataLoader::CreateTexture("Assets/AlmanacScreen/openAlmanacNextLitUp.png"));

	//create close button sprites
	almanac.closeSprites.push_back(DataLoader::CreateTexture("Assets/AlmanacScreen/openAlmanacClose.png"));
	almanac.closeSprites.push_back(DataLoader::CreateTexture("Assets/AlmanacScreen/openAlmanacCloseLitUp.png"));

	//updating transform matrices for all of the almanac's sprites
	for (int i {0}; i < 18; ++i)
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
	//make an extra entry for the chimera
	almanac.entries[14].enemyEntrySprite = DataLoader::CreateTexture("Assets/Enemies/Boss/chimeraBoss.png");
}

//Only called in RenderAlmanacPages, renders the appropriate items ON the current page (text, enemy images)
void RenderCurrentPage(Almanac & almanac, s8 font)
{
	//text rgb values
	float r {105.f / 255.f}, g {87.f / 255.f}, b{61.f / 255.f};

	//buffer for printing text to the almanac
	char buffer[50];
	//text widt and height to centralise text
	f32 textWidth, textHeight;
	//print page number
	sprintf_s(buffer, 50, "%d", almanac.currentPageNumber);
	AEGfxGetPrintSize(font, buffer, 0.8f, &textWidth, &textHeight);
	AEGfxPrint(font, buffer, -textWidth / 2 - 0.72f, -textHeight / 2 - 0.55f, 0.8f, r, g, b, 1.f);
	

	//entries of each enemy
	if (almanac.currentPageNumber < 19 && almanac.currentPageNumber > 3)
	{
		//get the current entry to show
		AlmanacEntry thisEntry{ almanac.entries[almanac.currentPageNumber - 4] };
		//display the enemy name
		const char * text { (18 == almanac.currentPageNumber) ? "Chimera" : thisEntry.enemyType.name.c_str()};
		AEGfxGetPrintSize(font, text, 1.f, &textWidth, &textHeight);
		AEGfxPrint(font, text, -textWidth / 2 - 0.4f, -textHeight / 2 + 0.13f, 1.f, r, g, b, 1.f);

		//display the enemy description
		text = thisEntry.description.c_str() ;
		AEGfxGetPrintSize(font, text, 1.f, &textWidth, &textHeight);
		AEGfxPrint(font, text, -textWidth / 2 - 0.4f, -textHeight / 2 - 0.3f, 1.f, r, g, b, 1.f);

		//header texts for traits, likes and dislikes
		text = "Traits:";
		AEGfxGetPrintSize(font, text, 1.f, &textWidth, &textHeight);
		AEGfxPrint(font, text, 0.055f, -textHeight / 2 + 0.7f, 1.f, r, g, b, 1.f);

		text = "Likes:";
		AEGfxGetPrintSize(font, text, 1.f, &textWidth, &textHeight);
		AEGfxPrint(font, text, 0.055f, -textHeight / 2 + 0.225f, 1.f, r, g, b, 1.f);

		text = "Disikes:";
		AEGfxGetPrintSize(font, text, 1.f, &textWidth, &textHeight);
		AEGfxPrint(font, text, 0.055f, -textHeight / 2 - 0.25f, 1.f, r, g, b, 1.f);

		//get and print traits, likes, and dislikes if not boss page
		if (almanac.currentPageNumber != 18)
		{
			//Print traits
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
			index = 0;
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
			index = 0;
			for (std::string dislike : thisEntry.enemyType.dislikes)
			{
				std::string toPrint = "- " + dislike;
				text = toPrint.c_str();
				AEGfxPrint(font, text, 0.055f + (index / 3 * 0.4f),
					-0.395f - (index % 3 * 0.1f),
					0.8f, r, g, b, 1.f);
				++index;
			}
		}
		//print special traits, likes, and dislikes for the boss
		else
		{
			text = "- Evil";
			AEGfxPrint(font, text, 0.055f, 0.555f, 0.8f, r, g, b, 1.f);

			text = "- Nothing";
			AEGfxPrint(font, text, 0.055f, 0.09f, 0.8f, r, g, b, 1.f);

			text = "- Everything";
			AEGfxPrint(font, text, 0.055f, -0.395f, 0.8f, r, g, b, 1.f);
		}

		//print the enemy's damage
		sprintf_s(buffer, 50, (18 == almanac.currentPageNumber) ? "?" : "%.0f", thisEntry.enemyType.damage);
		AEGfxGetPrintSize(font, buffer, 1.f, &textWidth, &textHeight);
		AEGfxPrint(font, buffer, -textWidth / 2 - 0.555f, -textHeight / 2 - 0.065f, 1.f, r, g, b, 1.f);

		//print the enemy's speed
		sprintf_s(buffer, 50, (18 == almanac.currentPageNumber) ? "?" : "%.0f", thisEntry.enemyType.speed);
		AEGfxGetPrintSize(font, buffer, 1.f, &textWidth, &textHeight);
		AEGfxPrint(font, buffer, -textWidth / 2 - 0.27f, -textHeight / 2 - 0.065f, 1.f, r, g, b, 1.f);

		//Render enemy sprite
		thisEntry.enemyEntrySprite.position = Vector2(-320.f, 200.f);
		thisEntry.enemyEntrySprite.UpdateTransform();
		thisEntry.enemyEntrySprite.RenderSprite();
	}
	//main page
	else if (3 == almanac.currentPageNumber)
	{
	//header text
		const char* text{ "Enemies" };
		AEGfxGetPrintSize(font, text, 2.f, &textWidth, &textHeight);
		AEGfxPrint(font, text, -textWidth / 2.f - 0.4f, -textHeight / 2.f + 0.6f, 2.f, r, g, b, 1.f);

		//print the image of each enemy entry
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
	else if (0 == almanac.currentPageNumber)
	{
		//hardcoded tutorial texts
		const char* text { "Tutorial" };
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
	//tutorial page 2
	else if (1 == almanac.currentPageNumber)
	{
		//hardcoded tutorial texts
		const char* text{ "Press or hold" };
		AEGfxGetPrintSize(font, text, 1.f, &textWidth, &textHeight);
		AEGfxPrint(font, text, -textWidth / 2 - 0.56f, -textHeight / 2 - 0.22f, 1.f, r, g, b, 1.f);

		text = "to throw a gift";
		AEGfxGetPrintSize(font, text, 1.f, &textWidth, &textHeight);
		AEGfxPrint(font, text, -textWidth / 2 - 0.4f, -textHeight / 2 - 0.35f, 1.f, r, g, b, 1.f);

		text = "Give your enemies gifts they";
		AEGfxGetPrintSize(font, text, 1.f, &textWidth, &textHeight);
		AEGfxPrint(font, text, -textWidth / 2 + 0.41f, -textHeight / 2 + 0.25f, 1.f, r, g, b, 1.f);

		text = "like to befriend them";
		AEGfxGetPrintSize(font, text, 1.f, &textWidth, &textHeight);
		AEGfxPrint(font, text, -textWidth / 2 + 0.41f, -textHeight / 2 + 0.15f, 1.f, r, g, b, 1.f);

		text = "Be careful, don't give them";
		AEGfxGetPrintSize(font, text, 1.f, &textWidth, &textHeight);
		AEGfxPrint(font, text, -textWidth / 2 + 0.41f, -textHeight / 2 - 0.4f, 1.f, r, g, b, 1.f);

		text = "something they'd dislike!";
		AEGfxGetPrintSize(font, text, 1.f, &textWidth, &textHeight);
		AEGfxPrint(font, text, -textWidth / 2 + 0.41f, -textHeight / 2 - 0.5f, 1.f, r, g, b, 1.f);
	}
	//tutoirial page 3
	else if (2 == almanac.currentPageNumber)
	{
		//hardcoded tutorial texts
		const char* text{ "Some enemies might not like" };
		textWidth, textHeight;
		AEGfxGetPrintSize(font, text, 1.f, &textWidth, &textHeight);
		AEGfxPrint(font, text, -textWidth / 2 - 0.4f, -textHeight / 2 - 0.22f, 1.f, r, g, b, 1.f);

		text = "certain friends's traits,";
		AEGfxGetPrintSize(font, text, 1.f, &textWidth, &textHeight);
		AEGfxPrint(font, text, -textWidth / 2 - 0.4f, -textHeight / 2 - 0.32f, 1.f, r, g, b, 1.f);

		text = "so watch out!";
		AEGfxGetPrintSize(font, text, 1.f, &textWidth, &textHeight);
		AEGfxPrint(font, text, -textWidth / 2 - 0.4f, -textHeight / 2 - 0.42f, 1.f, r, g, b, 1.f);

		text = "Defeat the evil Chimera";
		AEGfxGetPrintSize(font, text, 1.f, &textWidth, &textHeight);
		AEGfxPrint(font, text, -textWidth / 2 + 0.41f, -textHeight / 2 - 0.22f, 1.f, r, g, b, 1.f);

		text = "with the help of your friends";
		AEGfxGetPrintSize(font, text, 1.f, &textWidth, &textHeight);
		AEGfxPrint(font, text, -textWidth / 2 + 0.41f, -textHeight / 2 - 0.32f, 1.f, r, g, b, 1.f);

		text = "to escape the dungeon!";
		AEGfxGetPrintSize(font, text, 1.f, &textWidth, &textHeight);
		AEGfxPrint(font, text, -textWidth / 2 + 0.41f, -textHeight / 2 - 0.42f, 1.f, r, g, b, 1.f);
		}
}

//Renders the book and its bookmarks/buttons, and calls RenderCurrentPage
void RenderAlmanacPages(Almanac & almanac, s8 font)
{
	//set the proper render mode to properly render textures
	AEGfxSetRenderMode(AE_GFX_RM_TEXTURE);
	//if the almanac is open
	if (almanac.isOpen)
	{
		//render the correct page sprite depending on what area the page is from
		for (int i{ 0 }; i < 6; ++i)
		{
			if (allAreas[i] == almanac.currentArea)
				almanac.pageSprites[i].RenderSprite();
		}

		//render tutorial page 1 if on page 0
		if (0 == almanac.currentPageNumber)
		{
			almanac.pageSprites[12].RenderSprite();
		}
		//render tutorial page 2 if on page 1
		else if (1 == almanac.currentPageNumber)
		{
			almanac.pageSprites[13].RenderSprite();
		}
		//render tutorial page 3 if on page 2
		else if (2 == almanac.currentPageNumber)
		{
			almanac.pageSprites[14].RenderSprite();
		}
		//render boss page 3 if on page 18
		else if (18 == almanac.currentPageNumber)
		{
			almanac.pageSprites[16].RenderSprite();
		}

		//display close button and light it up when hovering mouse over it
		(IsCursorInRect(Vector2(577, 332), 72, 92)) ? almanac.closeSprites[1].RenderSprite() : almanac.closeSprites[0].RenderSprite();
		
		//render the arrow sprite(s) depending on what page we are in
		switch (almanac.currentPageNumber)
		{
			//first page
			case (0):
				almanac.arrowSprites[0].RenderSprite();
				//display lit up next button when hovering mouse over it
				if (IsCursorInRect(Vector2(583, -352), 74, 85))
					almanac.arrowSprites[4].RenderSprite();
				break;
			//last page
			case (18):
				almanac.arrowSprites[2].RenderSprite();
				//display lit up back button when hovering mouse over it
				if (IsCursorInRect(Vector2(486, -352), 74, 85)) almanac.arrowSprites[3].RenderSprite();
				break;
			default:
				almanac.arrowSprites[1].RenderSprite();
				//display lit up next/back buttons when hovering mouse over it
				if (IsCursorInRect(Vector2(583, -352), 74, 85)) almanac.arrowSprites[4].RenderSprite();
				if (IsCursorInRect(Vector2(486, -352), 74, 85)) almanac.arrowSprites[3].RenderSprite();

		}

		//call this to figure out what contents on the page to render
		RenderCurrentPage(almanac, font);

		//display lit up bookmarks when hovering mouse over them
		for (int i{ 0 }; i < 6; ++i)
		{
			if (IsCursorInRect(Vector2(683, -(68 * i) + 201), 75, 57))
			{
				if (almanac.currentArea != allAreas[i])almanac.pageSprites[i + 6].RenderSprite();
			}
		}
		//display lit up help bookmark when hovering mouse over it
		if (almanac.currentArea != "help" && IsCursorInRect(Vector2(683, 293), 75, 57))
		{
			almanac.pageSprites[15].RenderSprite();
		}
		//display lit up help bookmark when hovering mouse over it
		if (almanac.currentArea != "boss" && IsCursorInRect(Vector2(683, -208), 75, 57))
		{
			almanac.pageSprites[17].RenderSprite();
		}
	}
}

//Handles the inputs in the almanac
void AlmanacInputs(Almanac & almanac)
{
	if (almanac.isOpen)
	{ 
		//Plays the almanac's audio if it is not active (coded by Brandon)
		if (!almanacSound) {
			PauseBGM();
			AlmanacAudio();
			almanacSound = true;
		}

		//input for the close button
		if (AEInputCheckTriggered(AEVK_LBUTTON) && IsCursorInRect(Vector2(577, 332), 72, 92))
		{
			//close almanac
			almanac.isOpen = false;
			//stop playing the almanac sounds and resume to the ususal BGM (coded by Brandon)
			almanacSound = false;
			ResumeBGM();
		}

		//input for next button
		if (AEInputCheckTriggered(AEVK_LBUTTON) && IsCursorInRect(Vector2(583, -352), 74, 85))
		{
			if (almanac.currentPageNumber < almanac.maxPages) {
				//go to the next page if not at the last
				++(almanac.currentPageNumber);
				//play the almanac audio (coded by Brandon)
				AlmanacAudio();
			}
		}

		//input for back
		if (AEInputCheckTriggered(AEVK_LBUTTON) && IsCursorInRect(Vector2(486, -352), 74, 85))
		{
			if (almanac.currentPageNumber > 0) {
				//go to the previous page if not at the last
				--(almanac.currentPageNumber);
				//play the almanac audio (coded by Brandon)
				AlmanacAudio();
			}
		}
		//input for tutorial bookmark
		if (almanac.currentArea != "help" && AEInputCheckTriggered(AEVK_LBUTTON) &&
			IsCursorInRect(Vector2(683, 293), 75, 57))
		{
			//play the almanac audio (coded by Brandon)
			BigPageFlip();
			//set curent page to first page
			almanac.currentPageNumber = 0;
		}

		//inputs for area bookmarks
		for (int i{ 0 }; i < 6; ++i)
		{
			if (almanac.currentArea != allAreas[i] && AEInputCheckTriggered(AEVK_LBUTTON) &&
				IsCursorInRect(Vector2(683, -(68 * i) + 201), 75, 57))
			{
				//play the almanac audio (coded by Brandon)
				BigPageFlip();
				//set curent page depending on which bookmark is clicked
				almanac.currentPageNumber = bookmarkPages[i];
			}
		}

		//input for the boss bookmark
		if (almanac.currentArea != "boss" && AEInputCheckTriggered(AEVK_LBUTTON) &&
			IsCursorInRect(Vector2(683, -208), 75, 57))
		{
			//play the almanac audio (coded by Brandon)
			BigPageFlip();
			//set curent page to 18, the boss page
			almanac.currentPageNumber = 18;
		}



		//inputs for the enemy entry buttons on the main page
		if (3 == almanac.currentPageNumber)
		{
			for (int i{ 0 }; i < 15; ++i)
			{
				//when an enemy on this page is clicked, go to their page
				if (AEInputCheckTriggered(AEVK_LBUTTON) && ((i < 6) ?
					IsCursorInRect(Vector2(-520 + (i % 3 * 187), 80 - (i / 3 * 185)), 160, 160) :
					IsCursorInRect(Vector2(140 + ((i - 6) % 3 * 187), 190 - ((i - 6) / 3 * 185)), 160, 160)))
				{
					almanac.currentPageNumber = i + 4;
				}
			}
		}
		
		//set current are depending on page number / current entry
		almanac.currentArea = (almanac.currentPageNumber > 3 && almanac.currentPageNumber < 20)
			? almanac.entries[almanac.currentPageNumber - 4].area :
			(almanac.currentPageNumber <= 2) ? "help" : "main";
		
	}
}

//initialise some of the almanac's values
void AlmanacInit(Almanac& almanac)
{
	almanac.isOpen = false;
	almanac.hasBeenOpened = false;
	almanac.currentPageNumber = 0;
}

//free the almanac by clearing its vectors
void AlmanacFree(Almanac& almanac)
{
	almanac.pageSprites.clear();
	almanac.entries.clear();
	almanac.arrowSprites.clear();
	almanac.closeSprites.clear();
}

