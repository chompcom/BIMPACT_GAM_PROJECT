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
		//TexturedSprite sprite;
		AnimatedSprite sprite;
		TexturedSprite shadow;
		f32 currentHealth;
		bool isActive; // For checking if its alive
		Vector2 velocity{};
		f32 speedModifier{ 1.0f };
		f32 invulnerableTimer{ 0.f };
		bool collideWall{ false };
		
		std::unique_ptr<Boss_FSM> bossStateMachine;

		//points to the room it should be inside, so that it knows whats going on inside!
		RoomData& roomData;

		Boss(std::string enemyName, f32 enemyHealth, f32 enemyDamage, TexturedSprite enemySprite, TexturedSprite shadowSprite, 
			RoomData& currentRoom, std::vector<AttackData> attackData);
		~Boss();

		void Update(Player& player, f32 dt);
		void CollideProjectile();
		void CollideGift();

		void ResetBoss();
};

