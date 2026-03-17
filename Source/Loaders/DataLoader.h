#pragma once

//#include "json/json.h"

#include "../Sprite.h"

#include "../Enemy.h"

#include "../Almanac.h"

#include "json/json.h"


namespace DataLoader {
	
	void Load();
	void Unload();

	//auto LoadJson(std::string filename);	// auto cuz we never load the header here
	using Json::Value;
	Json::Value LoadJsonFile(std::string const& file);
	bool DumpFile(std::string filename, std::vector<std::pair<std::string, std::string>> const& data);


	AEGfxVertexList* GetMesh();

	TexturedSprite CreateTexture(std::string filename);

	//This is so that other files can get the almanac entry vector
	std::vector<AlmanacEntry> GetAlmanacVector();

	//returns a copy of an existing data type.
	//returns the default type if not existent!
	EnemyType const& GetEnemyType(std::string name);

};