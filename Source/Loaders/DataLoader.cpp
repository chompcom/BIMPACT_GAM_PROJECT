#include "DataLoader.h"

#include "AEEngine.h"

#include <unordered_map>
#include <iostream>

#include "json/json.h"
#include <fstream>
#include <string>

#include <vector>

namespace DataLoader {

	static AEGfxVertexList* squareMesh = nullptr;

	using TextureList = std::unordered_map<std::string, AEGfxTexture*>;
	using TexturePair = std::pair<std::string, AEGfxTexture*>;
	static TextureList textures{};

	using EnemyTypeList = std::unordered_map<std::string, EnemyType>;
	using EnemyPair = std::pair<std::string, EnemyType>;
	static EnemyTypeList enemyTypes{};

	static std::vector<AlmanacEntry> almanacEntries{};

	static Json::Value theGuy;

	void Load() {
		squareMesh = CreateSquareMesh();
		textures.reserve(5);
		enemyTypes.reserve(5);
		//std::ifstream ifs{"Assets/test.json"};
		std::ifstream ifs{ "Assets/almanac.json" };

		if (ifs.is_open()) {
			std::cout << "ok there's something!" << std::endl;
			ifs >> theGuy; //Take the value!

			//std::cout << theGuy["enemies"][0]["name"];

			//Getting enemy types
			enemyTypes.reserve(theGuy["enemyTypes"].size());

			for (Json::Value& name : theGuy["enemyTypes"]) {

				//get all the traits
				Labels tmpTraits;
				for (Json::Value& trait : name["traits"])
				{
					tmpTraits.insert(trait.asString());
				}

				//get all the likes
				Labels tmpLikes;
				for (Json::Value& like : name["likes"])
				{
					tmpLikes.insert(like.asString());
				}

				//get all the dislikes
				Labels tmpDislikes;
				for (Json::Value& dislike : name["dislikes"])
				{
					tmpDislikes.insert(dislike.asString());
				}

				EnemyType tmp{ name["name"].asString(), name["health"].asFloat(), name["damage"].asFloat(), 
					tmpTraits, tmpLikes, tmpDislikes };


				enemyTypes.insert({
					name["name"].asString(),
					tmp
					});

				// std::cout << "name: " << name["name"] << std::endl;
			}
			
			for (EnemyPair const& type : enemyTypes) {
				std::cout << type.first << std::endl;
			}
			//std::cout << "ok there's something!" << std::endl;
			//ifs >> theGuy; //Take the value!

			////std::cout << theGuy["enemies"][0]["name"];

			//enemyTypes.reserve(theGuy["enemies"].size());

			//for (Json::Value& name : theGuy["enemies"]) {

			//	EnemyType tmp{ name["name"].asString(),0,0, {}, {}, {}};
	
			//	enemyTypes.insert({
			//		name["name"].asString(),
			//		tmp
			//		});

			//	//std::cout << "name: " << name["name"] << std::endl;
			//}

			//for (EnemyPair const& type : enemyTypes) {
			//	std::cout << type.first << std::endl;
			//}

			//Getting almanac entries
			almanacEntries.reserve(theGuy["almanacEntries"].size());

			for (Json::Value& name : theGuy["almanacEntries"]) 
			{
				AlmanacEntry tmp{DataLoader::GetEnemyType(name["name"].asString()), name["description"].asString(), 
					name["area"].asString(), DataLoader::CreateTexture(name["spritePath"].asString())};
				tmp.enemyEntrySprite.scale = Vector2(name["xPictureScale"].asInt(), name["yPictureScale"].asInt());
				tmp.enemyEntrySprite.UpdateTransform();

				almanacEntries.push_back(tmp);
				// std::cout << "name: " << name["name"] << std::endl;
			}

		}
	}

	TexturedSprite CreateTexture(std::string filename)
	{
		TextureList::const_iterator finder = textures.find(filename);
		if (finder == textures.end()) {
			//couldn't find it
			textures.insert(TexturePair{ filename, AEGfxTextureLoad(filename.c_str())});
		}
		//(textures.find(filename))->second
		return TexturedSprite(squareMesh, textures.find(filename)->second, Vector2(0, 0), Vector2(100, 100), Color{1.0f,1.0f,1.0f,1.0f});
	}

	//This is so that other files can get the almanac entry vector
	std::vector<AlmanacEntry> GetAlmanacVector()
	{
		return almanacEntries;
	}

	EnemyType GetEnemyType(std::string name)
	{
		EnemyTypeList::const_iterator finder = enemyTypes.find(name); 
		if (finder == enemyTypes.end()) {
			
			return EnemyType{"default",100,0,{},{},{}};
		}
		return finder->second;
	}

	void Unload() {
		for (TexturePair them : textures) {
			if (them.second)
				AEGfxTextureUnload(them.second);
		}
		textures.clear(); //just in case
		if (squareMesh) AEGfxMeshFree(squareMesh);
	}

	AEGfxVertexList* GetMesh()
	{
		return squareMesh;
	}

} //end DataLoader
