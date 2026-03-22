#include "DataLoader.h"

#include "AEEngine.h"

#include <unordered_map>
#include <iostream>

#include "json/json.h"
#include <fstream>
#include <string>

#include "Behaviour/Behaviours.h"
#include <vector>


//Helper function space!!
namespace {

	void AddBehaviours(EnemyType& tmp, Json::Value& source, std::string const& type) {

		for (Json::Value& thing : source[type]) {
			BundledBehaviour behavs{};
			for (Json::Value commernd : thing["actions"]) {
				std::cout << commernd.asString() << std::endl;
				behavs.push_back(GetCommand(commernd.asString()));
			}

			if (type == "happy")
				tmp.AddHappy(GetFlag(thing["context"].asString()), behavs);

			if (type == "neutral")
				tmp.AddNeutral(GetFlag(thing["context"].asString()), behavs);

			if (type == "angry")
				tmp.AddAngry(GetFlag(thing["context"].asString()), behavs);
		}
		
	}

	bool MapProjectile(EnemyType::ProjectileInfo& tmp, Json::Value& source, std::string const& type) {
		if (!source[type].isNull()) {

			Json::Value og = source[type];

			tmp.damage     = og["damage"].asFloat();
			tmp.radius     = og["radius"].asFloat();
			tmp.speed      = og["speed"].asFloat();
			tmp.lifetime   = og["lifetime"].asFloat();
			tmp.spritePath = og["sprite"].asString();
			return true;
		}
		return false;
	}

}


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
		std::ifstream enemyFile{"Assets/test.json"};
		InitCommands();
		//std::ifstream ifs{"Assets/test.json"};
		std::ifstream almanacFile{ "Assets/almanac.json" };

		if (enemyFile.is_open()) {
			std::cout << "ok there's something!" << std::endl;
			enemyFile >> theGuy; //Take the value!

			//std::cout << theGuy["enemies"][0]["name"];

			//Getting enemy types
			enemyTypes.reserve(theGuy["enemies"].size());

			for (Json::Value& name : theGuy["enemies"]) {
				EnemyType tmp{ name["name"].asString(),0,0, {}, {}, {}};

				AddBehaviours(tmp, name, "happy");
				AddBehaviours(tmp, name, "angry");
				AddBehaviours(tmp, name, "neutral");

				tmp.health = name["health"].asFloat();
				tmp.damage = name["damage"].asFloat();
				tmp.speed = name["speed"].asFloat();
				tmp.detectionRadius = name["detectionRadius"].asFloat();
				tmp.safeRadius = name["safeRadius"].asFloat();


				if (MapProjectile(tmp.happyProjectile, name, "happyProjectile"))
				{
					std::cout << tmp.name << "'s Happy Projectile: " << tmp.happyProjectile.damage << "\n";
				}

				if (MapProjectile(tmp.angryProjectile, name, "angryProjectile"))
				{
					std::cout << tmp.name << "'s Angry Projectile: " << tmp.angryProjectile.damage << "\n";
				}

				for (Json::Value& thing : name["likes"]){
					tmp.likes.insert(thing.asString());
				}
	

				enemyTypes.insert({
					name["name"].asString(),
					tmp
					});

				// std::cout << "name: " << name["name"] << std::endl;
			}
			
			for (EnemyPair const& type : enemyTypes) {
				std::cout << type.first << std::endl;
			}

		}



		if (almanacFile.is_open()) {
			//Getting almanac entries

			almanacFile >> theGuy;
			almanacEntries.reserve(theGuy["almanacEntries"].size());

			for (Json::Value& name : theGuy["almanacEntries"]) 
			{
				AlmanacEntry tmp{DataLoader::GetEnemyType(name["name"].asString()), name["description"].asString(), 
					name["area"].asString(), DataLoader::CreateTexture(name["spritePath"].asString())};
				tmp.enemyEntrySprite.scale = Vector2(name["xPictureScale"].asInt(), name["yPictureScale"].asInt());
				tmp.enemyEntrySprite.UpdateTransform();
				

				almanacEntries.push_back(tmp);
				 std::cout << "name: " << name["name"] << std::endl;
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

	EnemyType const& GetEnemyType(std::string name)
	{
		EnemyTypeList::const_iterator finder = enemyTypes.find(name); 
		if (finder == enemyTypes.end()) {
			return enemyTypes.find("Default")->second;
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
