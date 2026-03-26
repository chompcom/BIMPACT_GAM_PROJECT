#pragma once
#include <vector>

#include "Player.h"
#include "Enemy.h"
#include "Gift.h"
#include "Projectile.h"
#include "Boss.h"
#include "Grid.h"

class Enemy;
class Boss;
// this class contains data for what is inside the room!
struct RoomData {
    public: // for now

        Player* player;
        std::vector<Enemy*> enemyList;
        std::vector<Gift*> giftList;
        std::vector<Projectile*> projectileList;
        Boss* boss;
        Grid grid;

        RoomData();

};

