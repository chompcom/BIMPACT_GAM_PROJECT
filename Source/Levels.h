#pragma once
#include <iostream>
#include <vector>
#include <unordered_map>


#include "Loaders/DataLoader.h" // I need the json thingy
#include "Utils/Utils.h"

#include "json/json.h"          // For types
using JsonFormat = Json::Value;

//std::unordered_map<std::string, LevelInfo> Levels
struct LevelInfo
{
    // Bad habit from prof
    using uNum = std::size_t;

    uNum        currentLevel;
    uNum        gridSizeMax;
    uNum        maxRooms;
    const uNum  minRooms = 2;
    std::vector<unsigned int> seedPool;

    // Init Levels and load into struct?
    //LevelInfo(std::string fileName = LEVEL_PATH, uNum LevelNum);
};

class Levels
{
    public:
        bool                LoadLevel(std::string const& fileName, LevelInfo::uNum levelNum);
        LevelInfo const*    GetLevelInfo();
        std::size_t         GetTotalLevelCount() const;

    private:
        LevelInfo           loadedLevel;
        LevelInfo::uNum     currentLevelNum{0};
        bool                levelLoaded;
        std::size_t         totalLevels{0};    // For checking purposes
        //std::vector<LevelInfo> loadedLevels;
};
