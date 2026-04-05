/* Start Header ************************************************************************/
/*!
\file		DataLoader.cpp
\author 	Hong Josiah Qin, hong.j, 2501239
\par  		hong.j@digipen.edu
\brief		Loads the various files into their respective data.
All data is stored as static global variables accessible only via the functions dataloader provides.

Copyright (C) 2026 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited.
*/
/* End Header **************************************************************************/

#include "DataLoader.h"

#include "AEEngine.h"

#include <unordered_map>
#include <iostream>

#include "json/json.h"
#include <fstream>
#include <string>

#include "Behaviour/Behaviours.h"
#include <vector>
#include "Boss.h"
#include "FSM.h"


//Helper function space!!
namespace {

	// Helper function to add behaviours to the EnemyType from the Json Source
	void AddBehaviours(EnemyType& tmp, Json::Value& source, std::string const& type) {

		for (Json::Value& thing : source[type]) {
			BundledBehaviour behavs{};
			for (Json::Value commernd : thing["actions"]) {
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

	// Helper function to set the projectile information in the enemy type from the json source
	bool MapProjectile(EnemyType::ProjectileInfo& tmp, Json::Value& source, std::string const& type) {
		if (!source[type].isNull()) {

			Json::Value og = source[type];

			tmp.damage     = og["damage"].asFloat();
			tmp.radius     = og["radius"].asFloat();
			tmp.speed      = og["speed"].asFloat();
			tmp.lifetime   = og["lifetime"].asFloat();
			tmp.spritePath = og["sprite"].asString();

			if (!og["color"].isNull())
			{
				float maxVal = 255.f;
				tmp.color.r = og["color"][0].asFloat();
				tmp.color.g = og["color"][1].asFloat();
				tmp.color.b = og["color"][2].asFloat();
				tmp.color.a = og["color"][3].asFloat();

				tmp.color.r /= maxVal;
				tmp.color.g /= maxVal;
				tmp.color.b /= maxVal;
				tmp.color.a /= maxVal;

			}
			return true;
		}
		return false;
	}
	// Add quarter circle (depending on segments and radius) vertices to a vector list (For rrect mesh)
	void AddArcPoints(std::vector<Vector2>& points,
		f32 cx, f32 cy,
		f32 radius,
		f32 startAngle, f32 endAngle,
		int segments,
		bool skipFirst)
	{
		for (int i = 0; i <= segments; ++i)
		{
			if (skipFirst && i == 0)
				continue;

			f32 t = static_cast<f32>(i) / static_cast<f32>(segments);
			f32 a = startAngle + (endAngle - startAngle) * t;

			f32 x = cx + cosf(a) * radius;
			f32 y = cy + sinf(a) * radius;

			points.push_back(Vector2(x, y));
		}
	}

	// For caching later... 
	// To be honest, this is likely counter-productive, given that we have alot of meshes created in ui... Meaning that cache misses are likely.
	std::string MakeRoundRectKey(f32 radiusRatio, int segments)
	{
		// NVRO says that returning a temp directly creates another copy... Ty prof pras hahah
		std::string res{ std::to_string(radiusRatio) + "_" + std::to_string(segments) };
		return res;
	}

}


namespace DataLoader {

	//These global variables are only created here and should not be accessed directly

	static AEGfxVertexList* squareMesh = nullptr;                        //For sprites
	static AEGfxVertexList* animatedMesh = nullptr;						 //For animated sprites
	static std::vector<AEGfxVertexList*> animatedMeshes;                 //List of animatedMeshes because we have a lot of them for every spritesheet made

	using TextureList = std::unordered_map<std::string, AEGfxTexture*>;  //List of textures loaded by AETextureLoad. Made a map to prevent duplicate textures.
	using TexturePair = std::pair<std::string, AEGfxTexture*>;			 
	static TextureList textures{};

	using EnemyTypeList = std::unordered_map<std::string, EnemyType>;    //List of enemyTypes because we have a lot of them and they carry a lot of data.
	using EnemyPair = std::pair<std::string, EnemyType>;
	static EnemyTypeList enemyTypes{};

	static std::vector<AlmanacEntry> almanacEntries{};                   //List of almanac entries
	static Json::Value theGuy;

	using SoundList = std::unordered_map<std::string, AEAudio>;          //List of sounds
	using SoundPair = std::pair<std::string, AEAudio>;
	static SoundList sounds{};

	// Sorry Josiah the mesh was a rabbit hole ~ MJ
	using RoundRectMeshList = std::unordered_map<std::string, AEGfxVertexList*>;	// For Rrect mesh cache
	using RoundRectMeshPair = std::pair<std::string, AEGfxVertexList*>;				// For rrect mesh pair

	static AEGfxVertexList *circleMesh{ nullptr };			// For square and circle mesh
	static RoundRectMeshList roundRectMeshes{};										// For rrect unit mesh

	static std::vector< std::unique_ptr<Boss_FSM>> bossStateMachines;

	// Loan Json From File into Json::Value object
	Json::Value LoadJsonFile(std::string const& file) {
		Json::Value res;
		std::ifstream fileInstance{ file };

		if (fileInstance.is_open()) {
			fileInstance >> res;
		}
		return res;
	}
	// Write to json file
	bool DumpFile(std::string filename, std::vector<std::pair<std::string, std::string>> const &data) {
		Json::Value output;
		std::ofstream o{ filename };

		if (o.is_open()) {
			for (std::pair<std::string, std::string> const& entry : data) output[entry.first] = entry.second;
			// Output
			Json::StreamWriterBuilder builder;
			builder["commentStyle"] = "None";
			builder["indentation"] = "   ";
			std::unique_ptr<Json::StreamWriter> writer(builder.newStreamWriter());
			writer->write(output, &o);
			o << std::endl;	// Write and flush
			return true;
		}
		return false;
	}

	  //  Lookup the name given in the sounds map and returns the Aeaudio handle (brandon)
	AEAudio GetSound(std::string const& name)  
	{
		SoundList::const_iterator finder = sounds.find(name);
		if (finder == sounds.end()) {
			return AEAudio{};							// if no name is found then returns this line
														// fyi even if the sound is missing no sound will just play and it wont crash
		}
		return finder->second;
	}

	// Loads all the data for the game to function!!
	void Load() {
		squareMesh = CreateSquareMesh();
		circleMesh = CreateCircleMesh();

		textures.reserve(5);
		enemyTypes.reserve(5);
		roundRectMeshes.reserve(8);

		std::ifstream enemyFile{ "Assets/test.json" };
		InitCommands();
		std::ifstream almanacFile{ "Assets/almanac.json" };

		std::ifstream audioFile{ "Assets/audio.json" };


		if (enemyFile.is_open()) {
			enemyFile >> theGuy; //Take the value!

			//Getting enemy types
			//enemyTypes.reserve(theGuy["enemies"].size());


			for (Json::Value& name : theGuy["enemies"]) {
				EnemyType tmp{ name["name"].asString(),0,0, {}, {}, {} };

				AddBehaviours(tmp, name, "happy");
				AddBehaviours(tmp, name, "angry");
				AddBehaviours(tmp, name, "neutral");

				tmp.spritePath = name["sprite"].asString();
				tmp.health = name["health"].asFloat();
				tmp.damage = name["damage"].asFloat();
				tmp.angrySound = name["angrySound"].asString();
				tmp.speed = name["speed"].asFloat();
				tmp.detectionRadius = name["detectionRadius"].asFloat();
				tmp.safeRadius = name["safeRadius"].asFloat();
				tmp.attackRate = name["attackRate"].asFloat();
				if (!name["wanderTime"].isNull()) {
					tmp.wanderTime = name["wanderTime"].asFloat();
				}
				if (!name["waitTime"].isNull()) {
					tmp.waitTime = name["waitTime"].asFloat();
				}

				if (name["traits"]) {
					for (Json::Value& traitStr : name["traits"]) {
						tmp.traits.insert(traitStr.asString());

					}
				}

				if (MapProjectile(tmp.happyProjectile, name, "happyProjectile"))
				{
					std::cout << tmp.name << "'s Happy Projectile: " << tmp.happyProjectile.damage << "\n";
				}

				if (MapProjectile(tmp.angryProjectile, name, "angryProjectile"))
				{
					std::cout << tmp.name << "'s Angry Projectile: " << tmp.angryProjectile.damage << "\n";
				}

				if (MapProjectile(tmp.neutralProjectile, name, "neutralProjectile"))
				{
					std::cout << tmp.name << "'s Neutral Projectile: " << tmp.neutralProjectile.damage << "\n";
				}


				for (Json::Value& thing : name["likes"]){
					tmp.likes.insert(thing.asString());
				}

				for (Json::Value& thing : name["dislikes"]) {
					tmp.dislikes.insert(thing.asString());
				}



				enemyTypes.insert({
					name["name"].asString(),
					tmp
					});

			}

		}



		if (almanacFile.is_open()) {
			//Getting almanac entries
			almanacEntries.clear();
			almanacFile >> theGuy;
			almanacEntries.reserve(theGuy["almanacEntries"].size());

			for (Json::Value& name : theGuy["almanacEntries"])
			{
				EnemyType const& entryEnemyType = DataLoader::GetEnemyType(name["name"].asString());
				AlmanacEntry tmp{entryEnemyType, name["description"].asString(), 
					name["area"].asString(), 
					DataLoader::CreateTexture(entryEnemyType.spritePath)};	
					//DataLoader::CreateTexture(name["spritePath"].asString())};
				tmp.enemyEntrySprite.scale = Vector2(name["xPictureScale"].asInt(), name["yPictureScale"].asInt());
				tmp.enemyEntrySprite.UpdateTransform();


				almanacEntries.push_back(tmp);
				
			}

		}


// Parses the audio json and loads all audio assets into the sounds map. (Brandon)
		if (audioFile.is_open()) {  
			Json::Value audioJson;
			audioFile >> audioJson;

			for (Json::Value& val : audioJson["audio"]["player"]) {
				sounds.insert({ val["name"].asString(), AEAudioLoadSound(val["path"].asString().c_str()) });
			}
			for (Json::Value& val : audioJson["audio"]["charging"]) {
				sounds.insert({ val["name"].asString(), AEAudioLoadSound(val["path"].asString().c_str()) });
			}
			for (Json::Value& val : audioJson["audio"]["ui"]) {
				sounds.insert({ val["name"].asString(), AEAudioLoadSound(val["path"].asString().c_str()) });
			}
			for (Json::Value& val : audioJson["audio"]["mob"]) {
				sounds.insert({ val["name"].asString(), AEAudioLoadSound(val["path"].asString().c_str()) });
			}
			for (Json::Value& val : audioJson["audio"]["room"]) {
				sounds.insert({ val["name"].asString(), AEAudioLoadSound(val["path"].asString().c_str()) });
			}
			for (Json::Value& val : audioJson["audio"]["menu"]) {
				sounds.insert({ val["name"].asString(), AEAudioLoadSound(val["path"].asString().c_str()) });
			}
		}
	}


	// -------------------
	// TEXTURES
	// -------------------

	// Texture creator function
	// Finds an existing texture and if it doesn't exist, loads it using AEGfxTextureLoad
	TexturedSprite CreateTexture(std::string filename)
	{
		TextureList::const_iterator finder = textures.find(filename);
		if (finder == textures.end()) {
			//couldn't find it
			textures.insert(TexturePair{ filename, AEGfxTextureLoad(filename.c_str())});
		}
		
		return TexturedSprite(GetOrCreateSquareMesh(), textures.find(filename)->second, Vector2(0, 0), Vector2(100, 100), Color{ 1.0f,1.0f,1.0f,1.0f });
	}

	//Same as Create Texture but specifically for AnimatedSprites
	TexturedSprite CreateAnimatedTexture(std::string filename, f32 offsetX, f32 offsetY)
	{
		TextureList::const_iterator finder = textures.find(filename);
		if (finder == textures.end()) {
			//couldn't find it
			textures.insert(TexturePair{ filename, AEGfxTextureLoad(filename.c_str()) });
		}

		return TexturedSprite(GetOrCreateAnimatedMesh(offsetX, offsetY), textures.find(filename)->second, Vector2(0, 0), Vector2(100, 100), Color{ 1.0f,1.0f,1.0f,1.0f });
	}

	// Gets the mesh object to point to
	// Useful for constructors where we need to store the mesh somewhere but we haven't loaded the mesh itself yet
	AEGfxVertexList* GetMesh()
	{
		return squareMesh;
	}

	// Creates the mesh if it doesn't exist yet
	AEGfxVertexList* GetOrCreateSquareMesh()
	{
		if (!squareMesh) squareMesh = CreateSquareMesh();
		return squareMesh;
	}

	// Creates the mesh if it doesn't exist yet
	AEGfxVertexList* GetOrCreateCircleMesh()
	{
		if (!circleMesh) circleMesh = CreateCircleMesh();
		return circleMesh;
	}

	// Creates the mesh if it doesn't exist yet
	AEGfxVertexList* GetOrCreateAnimatedMesh(f32 offsetX, f32 offsetY)
	{
		animatedMesh = CreateSquareMesh(1.f / offsetX, 1.f / offsetY);
		animatedMeshes.push_back(animatedMesh);
		return animatedMeshes.back();
	}



	/*!
		Cool but bruh. For creating rounded rectangle meshes. See the excel for demostration is you dk what is border-ratio in HTML...
	*/
	AEGfxVertexList* CreateRoundRectMeshInternal(f32 radiusRatio, int segmentsPerCorner, f32 width, f32 height)
	{
		AEGfxVertexList* mesh = nullptr;
		AEGfxMeshStart();

		const u32 white = 0xFFFFFFFF;
		const f32 pi = AEDegToRad(180);	// PI RAD = 180 DEG hahah

		UNREFERENCED_PARAMETER(pi);

		if (segmentsPerCorner < 1) segmentsPerCorner = 0;	// Segments cannot be less than 1 obviously?

		f32 halfW = width * 0.5f;
		f32 halfH = height * 0.5f;

		// Radius is based on smaller dimension
		f32 radius = radiusRatio * ((width < height) ? width : height);
		radius = AEClamp(radius, 0.0f, (halfW < halfH) ? halfW : halfH);

		// radius == 0 => normal rectangle
		if (radius <= 0.0f)
		{
			// Could have returned rect mesh but lazy scale so nvm

			AEGfxTriAdd(
				-halfW, -halfH, white, 0.0f, 1.0f,
				halfW, -halfH, white, 1.0f, 1.0f,
				halfW, halfH, white, 1.0f, 0.0f
			);

			AEGfxTriAdd(
				-halfW, -halfH, white, 0.0f, 1.0f,
				halfW, halfH, white, 1.0f, 0.0f,
				-halfW, halfH, white, 0.0f, 0.0f
			);

			return AEGfxMeshEnd();
		}

		// Original Vertices
		f32 left = -halfW + radius;
		f32 right = halfW - radius;
		f32 top = halfH - radius;
		f32 bottom = -halfH + radius;

		// Store vertices
		std::vector<Vector2> boundary;
		boundary.reserve(static_cast<size_t>(segmentsPerCorner * 4 + 4));	// Total number of vertices

		// Clockwise boundary (Essentially we are creating vertices at 4 corners in a clockwise manner)
		// Intention is in order to create rrect, we have to create a quadrant on 4 corners of a rectangle...
		AddArcPoints(boundary, right, top, radius, PI * 0.5f, 0.0f, segmentsPerCorner, false);
		AddArcPoints(boundary, right, bottom, radius, 0.0f, -PI * 0.5f, segmentsPerCorner, true);
		AddArcPoints(boundary, left, bottom, radius, -PI * 0.5f, -PI, segmentsPerCorner, true);
		AddArcPoints(boundary, left, top, radius, PI, PI * 0.5f, segmentsPerCorner, true);

		/*
		Fill with triangle fan from center
		Essentially adds our calculated arc boundaries into vertex lists
		r = 0.1, segments = 2

		Total vertices = (4*segments+4=12)
			  p3 ----- p2
		   p4             p1
		 p5                 p0
		 |                   |
		 p6                 pN-1
		   p7             pN-2
			  p8 ----- pN-3

		Each loop:
						 p1
						/|
					   / |
					  /  |
					 /   |
					/    |
				   C-----p0
		*/
		for (size_t i = 0; i < boundary.size(); ++i)
		{
			Vector2 const& p0 = boundary[i];
			Vector2 const& p1 = boundary[(i + 1) % boundary.size()];

			// For uv-mapping texture coordinates (likely-won't be used but yeah)
			f32 u0 = (p0.x + halfW) / width;
			f32 v0 = (halfH - p0.y) / height;
			f32 u1 = (p1.x + halfW) / width;
			f32 v1 = (halfH - p1.y) / height;

			// Creates a triangle segment of current to next point...
			AEGfxTriAdd(
				0.0f, 0.0f, white, 0.5f, 0.5f,	// Center should be 0.5 for UV
				p0.x, p0.y, white, u0, v0,		// Current Point for UV is based on where isit currently
				p1.x, p1.y, white, u1, v1		// Computated UV for next Point
			);
		}

		mesh = AEGfxMeshEnd();
		return mesh;
	}

	// For rrect meshes (UNIT ONLY)
	AEGfxVertexList* GetOrCreateRoundRectMesh(f32 radiusRatio, int segments)
	{
		std::string key = MakeRoundRectKey(radiusRatio, segments);

		RoundRectMeshList::const_iterator finder = roundRectMeshes.find(key);
		if (finder != roundRectMeshes.end()) return finder->second;

		AEGfxVertexList* mesh = CreateRoundRectMeshInternal(radiusRatio, segments, 1.0f, 1.0f);
		roundRectMeshes.insert(RoundRectMeshPair{ key, mesh });

		return mesh;
	}

	// NO CACHING FOR NORMAL RRECT MESHES BECAUSE THAT'S STUPID. Many differing variables like radiusRatio, segments, width and height bruh.
	AEGfxVertexList* CreateRoundRectMesh(f32 radiusRatio, int segments, f32 width, f32 height)
	{
		return CreateRoundRectMeshInternal(radiusRatio, segments, width, height);
	}


	//This is so that other files can get the almanac entry vector
	std::vector<AlmanacEntry> GetAlmanacVector()
	{
		return almanacEntries;
	}

	// Finds the enemy type if it exists. Otherwise, return a default type so that the program doesn't completely fail
	// Always take note that default enemies are errors
	EnemyType const& GetEnemyType(std::string name)
	{
		EnemyTypeList::const_iterator finder = enemyTypes.find(name); 
		if (finder == enemyTypes.end()) {
			return enemyTypes.find("Default")->second;
		}
		return finder->second;
	}

	// Clears all the global data and Unloads them.
	void Unload() {
		for (TexturePair them : textures) {
			if (them.second) AEGfxTextureUnload(them.second);
		}
		textures.clear(); //just in case

		// For rrect mesh cache
		for (RoundRectMeshPair const& pair : roundRectMeshes)
		{
			if (pair.second) AEGfxMeshFree(pair.second);
		}
		roundRectMeshes.clear();

		FreeAudio();   // this unloads the audio grp only
		for (SoundPair AllSounds : sounds) { // unload individual sounds
			AEAudioUnloadAudio(AllSounds.second);
		}
		sounds.clear();

		if (squareMesh) { AEGfxMeshFree(squareMesh); squareMesh = nullptr; };
		if (circleMesh) { AEGfxMeshFree(circleMesh); circleMesh = nullptr; };
		for (AEGfxVertexList* mesh : animatedMeshes) AEGfxMeshFree(mesh);
		animatedMeshes.clear();
		
		
	}


} //end DataLoader
