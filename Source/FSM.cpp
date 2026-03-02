#include "AEEngine.h"
#include "FSM.h"
#include "Collision.h"
#include "Boss.h"
#include "Sprite.h"
#include <iostream>

Boss_FSM::Boss_FSM(Boss* Boss) : boss{Boss}, interval { 0.0f }, initialState{ BOSS_IDLE }, currentState{ BOSS_IDLE } {}

Boss1_FSM::Boss1_FSM(Boss* Boss, f32 ChargeDamage, f32 ChargeStartup, f32 ChargeInterval, f32 ChargeEndlag,
	f32 JumpDamage, f32 JumpStartup, f32 JumpInterval, f32 JumpEndlag) 
	: Boss_FSM(Boss), attackPhase{ ATTACK_NIL }, target{}, counter{0},
	chargeDamage{ ChargeDamage }, chargeStartup{ ChargeStartup }, chargeInterval{ ChargeInterval }, chargeEndlag{ ChargeEndlag },
	jumpDamage{ JumpDamage }, jumpStartup{ JumpStartup }, jumpInterval{ JumpInterval }, jumpEndlag{ JumpEndlag }
{}

void Boss1_FSM::Update(Player& player, f32 dt) {
	//std::cout << "FSM linked\n";
	switch (currentState) {
		case BOSS_IDLE:
			boss->sprite.color = Color{ 0.0f, 1.0f, 0.0f, 1.0f };
			std::cout << "Boss Idle\n";
			interval += dt;
			if (interval >= 5.0f) {
				interval = 0.0f;
				if (counter == 2) {
					currentState = BOSS_JUMP;
					attackPhase = ATTACK_CHARGE;
					counter = 0;
				}
				else {
					currentState = BOSS_CHARGE;
					attackPhase = ATTACK_CHARGE;
					counter++;
				}
			}
			break;
		case BOSS_CHARGE:
			std::cout << "Boss Charge\n";
			ChargeAttack(player, dt);
			break;
		case BOSS_JUMP:
			//std::cout << "Boss Jump\n";
			JumpAttack(player, dt);
			break;
	}
}

void Boss1_FSM::ChargeAttack(Player& player, f32 dt) {
	Vector2 direction{};
	float collisionTime{ 0.0f };
	boss->sprite.color = Color{ 1.0f, 0.0f, 0.0f, 1.0f };
	switch (attackPhase) {
	case ATTACK_CHARGE:
		std::cout << "Boss Charge Starting\n";
		target = player.position;

		interval += dt;
		if (interval >= chargeStartup) {
			interval = 0.0f;
			attackPhase = ATTACK_ATTACK;
		}

		break;

	case ATTACK_ATTACK:
		std::cout << "Boss Charge Attack\n";
		direction = target - boss->sprite.position;

		if (CollisionIntersection_RectRect(boss->sprite.position, boss->sprite.scale, direction.Normalized() * 60 * dt,
			player.position, player.sprite.scale, player.direction * player.speed * dt, collisionTime)) {
			playerTakesDamage(player);
		}

		boss->sprite.position += direction.Normalized() * 60 * dt;

		interval += dt;
		boss->sprite.UpdateTransform();
		if (interval >= chargeInterval) {
			interval = 0.0f;
			attackPhase = ATTACK_COOLDOWN;
		}

		break;

	case ATTACK_COOLDOWN:
		std::cout << "Boss Charge Ending\n";
		interval += dt;
		if (interval >= chargeEndlag) {
			interval = 0.0f;
			currentState = BOSS_IDLE;
		}

		break;
	}
}

void Boss1_FSM::JumpAttack(Player& player, f32 dt) {
	Vector2 direction{};
	float collisionTime{ 0.0f };
	boss->sprite.color = Color{ 0.0f, 0.0f, 1.0f, 1.0f };
	switch (attackPhase) {
	case ATTACK_CHARGE:
		target = player.position;

		interval += dt;
		if (interval >= 0.4f * jumpStartup) {
			boss->sprite.position += Vector2{ 0, 1 } * 60 * dt;
		}
		if (interval >= jumpStartup) {
			interval = 0.0f;
			attackPhase = ATTACK_ATTACK;
		}

		break;

	case ATTACK_ATTACK:
		interval += dt;
		if (interval < 0.75f * jumpInterval) {
			boss->sprite.position = Vector2{ target.x, target.y + 180 } + Vector2{ 0, -1 } * 60 * interval;
		}
		if (interval >= 0.75f * jumpInterval) {
			if (CollisionIntersection_RectRect(target, boss->sprite.scale, Vector2{},
				player.position, player.sprite.scale, player.direction * player.speed * dt, collisionTime)) {
				playerTakesDamage(player);
			}
		}
		if (interval >= jumpInterval) {
			interval = 0.0f;
			attackPhase = ATTACK_COOLDOWN;
		}

		break;

	case ATTACK_COOLDOWN:
		interval += dt;
		if (interval >= jumpEndlag) {
			interval = 0.0f;
			currentState = BOSS_IDLE;
		}

		break;
	}
}