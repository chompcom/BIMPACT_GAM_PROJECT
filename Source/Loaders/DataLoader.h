/* Start Header ************************************************************************/
/*!
\file		DataLoader.h
\author 	Hong Josiah Qin, hong.j, 2501239
\par  		hong.j@digipen.edu
\brief		Contains free functions that allow for access to the JSON files and various other
data to be loaded.

Copyright (C) 2026 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited.
*/
/* End Header **************************************************************************/

#pragma once

#include "../Sprite.h"
#include "../Enemy.h"
#include "../Almanac.h"
#include "json/json.h"




namespace DataLoader {
	
	//! \brief Reads all the game related Json Files and writes the data into their corresponding data members
	//! \brief It currently loads the meshes to be used for textures, the Enemies, the Almanac and the Audio for the game.
	void Load();

	//! \brief Unloads the loaded content and clears all the data.
	void Unload();

	
	using Json::Value;

	//! \brief LoadJsonFile directly without needing to interface with the jsoncpp library directly.
	//! \param file The filepath to the JsonFile you want to load
	//! \return Returns the jsoncpp Json::Value object that contains the data of the json file.
	Json::Value LoadJsonFile(std::string const& file);

	//! \brief Meant to be used to write to json directly. Currently unused.
	bool DumpFile(std::string filename, std::vector<std::pair<std::string, std::string>> const& data);

	//! \brief Gets the squareMesh that is used by all textures.
	//! Useful to not need to create a mesh for every object.
	AEGfxVertexList* GetMesh();

	//! \brief Returns a TexturedSprite object
	//! \brief Preferred to use because we only need to load the texture once
	//! \param filename The filepath to the image you want to use
	//! \return A textured sprite with default values
	TexturedSprite CreateTexture(std::string filename);
	//! \brief Returns an AnimatedSprite object
	//! \param filename The filepath to the spritePath you want to use
	//! \param offsetXY the number of columns and rows the spritesheet has
	//! \return A textured sprite with default values
	TexturedSprite CreateAnimatedTexture(std::string filename, f32 offsetX = 1.0f, f32 offsetY = 1.0f);

	// New procedural mesh getters for UI
	//! \brief Creates a squareMesh if it doesn't exist already
	AEGfxVertexList* GetOrCreateSquareMesh();
	//! \brief Creates a circleMesh if it doesn't exist already
	AEGfxVertexList* GetOrCreateCircleMesh();
	//! \brief Creates an AnimatedMesh if it doesn't exist already
	AEGfxVertexList* GetOrCreateAnimatedMesh(f32 offsetX, f32 offsetY);

	//! \brief Creates or retrieve cached unit round rect mesh.
	AEGfxVertexList* GetOrCreateRoundRectMesh(f32 radiusRatio, int segments);

	//! \brief Creates a new final-size rounded-rect mesh.
	// Intended for extreme aspect ratio UI so corners stay correct.
	AEGfxVertexList* CreateRoundRectMesh(f32 radiusRatio, int segments, f32 width, f32 height);

	//! \brief Gets the almanac entry vector
	std::vector<AlmanacEntry> GetAlmanacVector();

	//! \brief returns a copy of an existing data type.
	//! \brief returns the default type if not existent!
	EnemyType const& GetEnemyType(std::string name);

	//! \brief Get the sound by its name if it was loaded in
	//! Returns an empty AEAudio object if the sound was not loaded previously.
	AEAudio GetSound(std::string const& name);


};