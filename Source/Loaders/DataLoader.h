#pragma once

//#include "json/json.h"

#include "../Sprite.h"

#include "../Enemy.h"

#include "../Almanac.h"
namespace DataLoader {
	
	void Load();
	void Unload();

	AEGfxVertexList* GetMesh();

	TexturedSprite CreateTexture(std::string filename);

	//This is so that other files can get the almanac entry vector
	std::vector<AlmanacEntry> GetAlmanacVector();

	//returns a copy of an existing data type.
	//returns the default type if not existent!
	EnemyType GetEnemyType(std::string name);

};