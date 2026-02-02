#pragma once
#include <vector>

#include "Player.h"
#include "Enemy.h"
#include "Gift.h"

class Enemy;
// this class contains data for what is going to go to the next room!
struct RoomData {
    public: // for now

        Player* player;
        std::vector<Enemy*> enemyList;
        std::vector<Gift*> giftList;

        RoomData();

};

