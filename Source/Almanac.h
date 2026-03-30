#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include "Enemy.h"
#include "Almanac.h"
//
//enum BookmarkPages{
//	TUTORIAL = 0,
//	MAIN = 0,
//	NORMAL = 1,
//	PLANT = 5,
//	OCEAN = 8
//};

class AlmanacEntry {
public:
	EnemyType enemyType;
	std::string description;
	std::string area;
	TexturedSprite enemyEntrySprite;
	bool unlocked;

	AlmanacEntry(EnemyType enemyType, std::string description, std::string area, 
		TexturedSprite enemyEntrySprite);

};

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

	Almanac();
};

void LoadAlmanacPages(Almanac & almanac);

void LoadAlmanacEntries(Almanac& almanac);

void RenderAlmanacPages(Almanac & almanac, s8 font);

void AlmanacInputs(Almanac& almanac/*, AEGfxVertexList* removeLater*/);

void AlmanacInit(Almanac& almanac);

void AlmanacFree(Almanac& almanac);
