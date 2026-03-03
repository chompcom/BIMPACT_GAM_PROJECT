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

		Boss_FSM(Boss* Boss);

		virtual void Update(Player&, f32) {}
};

class Boss1_FSM : public Boss_FSM {
	public:
		AttackState attackPhase;
		Vector2 target;
		s16 counter;

		f32 chargeDamage;
		f32 chargeStartup;
		f32 chargeInterval;
		f32 chargeEndlag;

		f32 jumpDamage;
		f32 jumpStartup;
		f32 jumpInterval;
		f32 jumpEndlag;

		Boss1_FSM(Boss* boss, f32 ChargeDamage, f32 ChargeStartup, f32 ChargeInterval, f32 ChargeEndlag,
			f32 jumpDamage, f32 jumpStartup, f32 jumpInterval, f32 jumpEndlag);

		void Update(Player& player, f32 dt) override;
		void ChargeAttack(Player& player, f32 dt);
		void JumpAttack(Player& player, f32 dt);
};