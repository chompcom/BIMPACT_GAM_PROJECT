#pragma once
#include "AEEngine.h"
#include "Sprite.h"
//#include "RoomData.h"
#include "Player.h"
#include <iostream>

struct RoomData;

class Boss;

enum BossStates {
	BOSS_IDLE,
	BOSS_WALK,
	BOSS_CHARGE,
	BOSS_JUMP,
	BOSS_SHOOT,
	BOSS_THROW,

};

enum AttackState {
	ATTACK_NIL,
	ATTACK_CHARGE,
	ATTACK_ATTACK,
	ATTACK_COOLDOWN,

};

class Boss_FSM {
	public:
		Boss* boss;
		f32 interval{};
		BossStates initialState;
		BossStates currentState;
		AttackState attackPhase;
		Vector2 target;
		s16 counter;

		Boss_FSM(Boss* Boss);

		virtual void Update(Player&, f32) {}
};

class Boss1_FSM : public Boss_FSM {
	public:
		f32 walkSpeed{ 120 };
		bool canWalk;

		Vector2 chargeDirection{ 0, 1 };
		f32 chargeSpeed{ 300 };
		f32 chargeDamage;
		f32 chargeStartup;
		f32 chargeInterval;
		f32 chargeEndlag;
		s32 additionalCharges{};

		f32 jumpSpeed{ 540 };
		f32 jumpDamage;
		f32 jumpStartup;
		f32 jumpInterval;
		f32 jumpEndlag;

		Boss1_FSM(Boss* boss, f32 ChargeDamage, f32 ChargeStartup, f32 ChargeInterval, f32 ChargeEndlag,
			f32 JumpDamage, f32 JumpStartup, f32 JumpInterval, f32 JumpEndlag);

		void Update(Player& player, f32 dt) override;
		void ChargeAttack(Player& player, f32 dt);
		void JumpAttack(Player& player, f32 dt);
};
