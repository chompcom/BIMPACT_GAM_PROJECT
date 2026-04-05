/* Start Header ************************************************************************/
/*!
\file		RoomData.h
\author 	Hong Josiah Qin, hong.j, 2501239
\par  		hong.j@digipen.edu
\brief		This file defines RoomData, which is a class that stores the collection of 
all the things the room contains. This way, entities are able to easily access each other
when they are in a room, without needing to interface with the rest of the room code.

Copyright (C) 2026 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited.
*/
/* End Header **************************************************************************/

#pragma once
#include <vector>

#include "Player.h"
#include "Enemy.h"
#include "Gift.h"
#include "Projectile.h"
#include "Boss.h"
#include "Grid.h"
#include "ParticleSystem.h"

class Enemy;
class Boss;
// this class contains data for what is inside the room!
struct RoomData {
    public: // for now

        //! Points to the player 
        Player* player;
        //! Each room should handle their own particles
        ParticleSystem particleSystem;
        std::vector<Enemy*> enemyList;
        std::vector<Gift*> giftList;
        std::vector<Projectile*> projectileList;
        Boss* boss;
        Grid grid;
        

        RoomData();

};

