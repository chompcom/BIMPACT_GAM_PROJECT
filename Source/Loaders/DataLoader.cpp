#include "DataLoader.h"

#include "AEEngine.h"

#include <unordered_map>


namespace DataLoader {

	static AEGfxVertexList* squareMesh = nullptr;

	using TextureList = std::unordered_map<std::string, AEGfxTexture*>;
	using TexturePair = std::pair<std::string, AEGfxTexture*>;
	static TextureList textures{};

	using EnemyTypeList = std::unordered_map<std::string, EnemyType>;
	using EnemyPair = std::pair<std::string, EnemyType>;
	static EnemyTypeList enemyTypes{};

	void Load() {
		squareMesh = CreateSquareMesh();
		textures.reserve(5);
		enemyTypes.reserve(5);
		
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

}
