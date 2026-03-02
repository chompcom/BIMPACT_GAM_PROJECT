#pragma once
#include "AEEngine.h"
#include "Sprite.h"
#include "RoomData.h"
#include "FSM.h"
#include "Player.h"

struct RoomData;

struct AttackData {
	f32 attackDamage;
	f32 attackStartup;
	f32 attackInterval;
	f32 attackEndlag;
};

class Boss {
	public:
		std::string name;
		f32 health;
		f32 damage;
		TexturedSprite sprite;
		f32 currentHealth;
		
		std::unique_ptr<Boss_FSM> bossStateMachine;

		//points to the room it should be inside, so that it knows whats going on inside!
		const RoomData& roomData;

		Boss(std::string enemyName, f32 enemyHealth, f32 enemyDamage, TexturedSprite enemySprite, const RoomData& currentRoom, 
			std::vector<AttackData> attackData);
		~Boss();

		void Update(Player& player, f32 dt);
};

