/* Start Header ************************************************************************/
/*!
\file     Almanac.h
\author   Lee Hng Hwee Celest, hnghweecelest.l, 2502385
\par      hnghweecelest.l@digipen.edu
\brief    This file declares some functions that implement the almanac

Copyright (C) 2026 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited.
*/
/* End Header **************************************************************************/

#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include "Enemy.h"
#include "Almanac.h"

//class for an almanac entry, there should be an almanac entry for each enemy + boss
class AlmanacEntry {
public:
	EnemyType enemyType;
	std::string description;
	std::string area;
	TexturedSprite enemyEntrySprite;
	bool unlocked;

	//AlmanacEntry constructor
	AlmanacEntry(EnemyType enemyType, std::string description, std::string area, 
		TexturedSprite enemyEntrySprite);

};

//class for the whole almanac
class Almanac {
public:
	std::vector<TexturedSprite> pageSprites;
	std::vector<AlmanacEntry> entries;
	bool isOpen;
	int currentPageNumber;
	std::string currentArea;
	std::vector<TexturedSprite> arrowSprites;
	std::vector<TexturedSprite> closeSprites;
	int const maxPages;
	bool hasBeenOpened;

	//Almanac constructor
	Almanac();
};

//Loading the sprites for the almanac and setting their transforms
void LoadAlmanacPages(Almanac & almanac);

//fill up the entries vector via the data loader
void LoadAlmanacEntries(Almanac& almanac);

//Renders the book and its bookmarks/buttons, and calls RenderCurrentPage
void RenderAlmanacPages(Almanac & almanac, s8 font);

//Handles the inputs in the almanac
void AlmanacInputs(Almanac& almanac);

//initialise some of the almanac's values
void AlmanacInit(Almanac& almanac);

//free the almanac by clearing its vectors
void AlmanacFree(Almanac& almanac);
