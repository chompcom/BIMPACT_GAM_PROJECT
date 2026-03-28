#include "RoomData.h"
#include "Loaders/DataLoader.h"

RoomData::RoomData() : player{ nullptr }, enemyList{}, giftList{}, boss{ nullptr }, grid{} 
,particleSystem{DataLoader::GetMesh()}
{
};