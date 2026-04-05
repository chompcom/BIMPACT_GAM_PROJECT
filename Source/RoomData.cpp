/* Start Header ************************************************************************/
/*!
\file		RoomData.cpp
\author 	Hong Josiah Qin, hong.j, 2501239
\par  		hong.j@digipen.edu
\brief		Only contains the default constructor to room data.

Copyright (C) 2026 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited.
*/
/* End Header **************************************************************************/

#include "RoomData.h"
#include "Loaders/DataLoader.h"

// Gets the DataLoader's mesh location for the particle system
RoomData::RoomData() : player{ nullptr }, enemyList{}, giftList{}, boss{ nullptr }, grid{} 
,particleSystem{DataLoader::GetMesh()}
{
};