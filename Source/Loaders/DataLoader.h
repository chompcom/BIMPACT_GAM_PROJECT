#pragma once

//#include "json/json.h"

#include "../Sprite.h"

#include "../Enemy.h"
namespace DataLoader {
	
	void Load();
	void Unload();

	AEGfxVertexList* GetMesh();

	TexturedSprite CreateTexture(std::string filename);

	//returns a copy of an existing data type.
	//returns the default type if not existent!
	EnemyType const& GetEnemyType(std::string name);

};