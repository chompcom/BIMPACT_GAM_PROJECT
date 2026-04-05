/* Start Header ************************************************************************/
/*!
\file       Boss.h
\author     Yee Kiat Lim, yeekiat.lim, 2503993
\par        yeekiat.lim@digipen.edu
\brief		This file implements the boss class.

Copyright (C) 2026 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited.
*/
/* End Header **************************************************************************/

#pragma once
#include "AEEngine.h"
#include "Sprite.h"
#include "RoomData.h"
#include "FSM.h"
#include "Player.h"
#include "Screens/Ui.h"

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

		AnimatedSprite sprite;
		TexturedSprite shadow;
		TexturedSprite hitbox;
		float shadowOffset;

		f32 currentHealth;
		bool isActive; // For checking if its alive
		Vector2 direction{};
		Vector2 velocity{};
		f32 speedModifier{ 1.0f };
		f32 invulnerableTimer{ 0.f };

		bool collideWall{ false };

		UIManager healthbar;
		bool healthbarInitialized{ false };
		
		std::unique_ptr<BossFSM> bossStateMachine;

		//points to the room it should be inside, so that it knows whats going on inside!
		RoomData& roomData;

		Boss(std::string enemyName, f32 enemyHealth, f32 enemyDamage, AnimatedSprite enemySprite, TexturedSprite shadowSprite, TexturedSprite hitboxSprite,
			RoomData& currentRoom, std::vector<AttackData> attackData, std::string const& filePath, s8 font, Vector2 pos = {0, 350}, Vector2 size = {1000, 30});
		~Boss();

		void Update(Player& player, f32 dt);
		void CollideProjectile();
		void CollideGift();

		void DamageBoss(s32 damage);

		void ResetBoss();
};

