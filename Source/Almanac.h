#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include "Enemy.h"
#include "Almanac.h"

class AlmanacEntry {
public:
	EnemyType enemyType;
	std::string description;
	std::string area;
	TexturedSprite enemyEntrySprite;
	Vector2 pictureScale;
	bool unlocked;

	AlmanacEntry(EnemyType enemyType, std::string description, std::string area, 
		TexturedSprite enemyEntrySprite, Vector2 pictureScale = Vector2(100,100));

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

//void LoadAlmanacEntries(Almanac & almanac, std::vector<EnemyType> allEnemyTypes);
void LoadAlmanacEntries(Almanac& almanac, std::vector<std::string> allEnemyTypes);

void RenderAlmanacPages(Almanac & almanac, s8 font);

void AlmanacInputs(Almanac& almanac, AEGfxVertexList* removeLater);
