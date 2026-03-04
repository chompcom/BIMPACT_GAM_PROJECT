#pragma once
#include <string>
#include <vector>
#include "Enemy.h"
#include "Almanac.h"

class AlmanacEntry {
public:
	EnemyType enemyType;
	std::string description;
	std::string area;
	TexturedSprite enemyEntrySprite;
	bool unlocked;

	AlmanacEntry(EnemyType enemyType, std::string description, std::string area, TexturedSprite enemyEntrySprite);

};

class Almanac {
public:
	std::vector<TexturedSprite> pageSprites;
	std::vector<AlmanacEntry> entries;
	bool isOpen;
	int currentPageNumber;
	std::vector<TexturedSprite> arrowSprites;
	std::vector<TexturedSprite> closeSprites;
	int const maxPages;

	Almanac();
};

void LoadAlmanacPages(Almanac & almanac);

void LoadAlmanacEntries(Almanac & almanac, std::vector<EnemyType> allEnemyTypes);

void RenderAlmanacPages(Almanac & almanac);

void AlmanacInputs(Almanac& almanac, AEGfxVertexList* removeLater);
