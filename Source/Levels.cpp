#include "Levels.h"
//#include "Loaders/DataLoader.h"

bool Levels::LoadLevel(std::string const& fileName, LevelInfo::uNum levelNum)
{
	// If already loaded, why load level details again?
	if (currentLevelNum == levelNum) return true;



	JsonFormat json = DataLoader::LoadJsonFile(fileName);

	// should not fail but yeah
	
	if (!json.isMember("levels")) return false;

	// Json is now loaded
	//auto& lvl = json["levels"][levelNum];
	Json::Value& lvl = json["levels"][(Json::ArrayIndex)levelNum];
	this->totalLevels = json["levels"].size();
	loadedLevel.currentLevel = static_cast<unsigned int>(lvl["levelId"].asInt());
	loadedLevel.gridSizeMax = static_cast<unsigned int>(lvl["gridSizeMax"].asInt());
	loadedLevel.maxRooms = static_cast<unsigned int>(lvl["maxRooms"].asInt());


	if (lvl.isMember("seedPool")) {
		if (!loadedLevel.seedPool.empty()) loadedLevel.seedPool.clear();
		for (std::size_t i = 0; i < lvl["seedPool"].size(); ++i) loadedLevel.seedPool.push_back(static_cast<unsigned int>(lvl["seedPool"][(Json::ArrayIndex)i].asInt()));
	}

	return true;
}

LevelInfo const* Levels::GetLevelInfo() {
	return &this->loadedLevel;
}

std::size_t Levels::GetTotalLevelCount() const {
	return this->totalLevels;
}
