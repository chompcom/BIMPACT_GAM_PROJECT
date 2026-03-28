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

