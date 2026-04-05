/* Start Header ************************************************************************/
/*!
\file       FSM.h
\author     Yee Kiat Lim, yeekiat.lim, 2503993
\par        yeekiat.lim@digipen.edu
\brief		This file implements the FSM classes that handle boss behaviour.

Copyright (C) 2026 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited.
*/
/* End Header **************************************************************************/

#pragma once
#include "AEEngine.h"
#include "Sprite.h"
#include "Player.h"
#include <iostream>

struct RoomData;

class Boss;

// Possible states boss may be in
enum BossStates {
	BOSS_IDLE,
	BOSS_WALK,
	BOSS_CHARGE,
	BOSS_FOLLOW,
	BOSS_JUMP
};

// Which phase of attack boss is in
enum AttackState {
	ATTACK_NIL,
	ATTACK_CHARGE,
	ATTACK_ATTACK,
	ATTACK_COOLDOWN
};

// Base Boss FSM
class BossFSM {
	public:
		Boss* boss;
		f32 interval{};
		BossStates initialState;
		BossStates currentState;
		AttackState attackPhase;
		Vector2 target;
		s16 counter;

		BossFSM(Boss* Boss);

		virtual void Update(Player&, f32) {}
};

// Chimera Boss FSM
class ChimeraBossFSM : public BossFSM {
	public:
		f32 walkSpeed{ 120 };
		bool canWalk;

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

		f32 followSpeed{ 240 };
		f32 followDamage;
		f32 followStartup;
		f32 followInterval;
		f32 followEndlag;

		ChimeraBossFSM(Boss* boss, f32 ChargeDamage, f32 ChargeStartup, f32 ChargeInterval, f32 ChargeEndlag,
			f32 JumpDamage, f32 JumpStartup, f32 JumpInterval, f32 JumpEndlag,
			f32 FollowDamage, f32 FollowStartup, f32 FollowInterval, f32 FollowEndlag);

		void Update(Player& player, f32 dt) override;
		void ChargeAttack(Player& player, f32 dt);
		void JumpAttack(Player& player, f32 dt);
		void FollowAttack(Player& player, f32 dt);
};
