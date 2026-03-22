#include "AEEngine.h"
#include "FSM.h"
#include "BoundaryCollision.h"
#include "Collision.h"
#include "Boss.h"
#include "Sprite.h"
#include <iostream>

Boss_FSM::Boss_FSM(Boss* Boss) : boss{Boss}, interval { 0.0f }, initialState{ BOSS_IDLE }, currentState{ BOSS_IDLE } {}

Boss1_FSM::Boss1_FSM(Boss* Boss, f32 ChargeDamage, f32 ChargeStartup, f32 ChargeInterval, f32 ChargeEndlag,
	f32 JumpDamage, f32 JumpStartup, f32 JumpInterval, f32 JumpEndlag) 
	: Boss_FSM(Boss), attackPhase{ ATTACK_NIL }, target{}, counter{0}, canWalk{true},
	chargeDamage{ ChargeDamage }, chargeStartup{ ChargeStartup }, chargeInterval{ ChargeInterval }, chargeEndlag{ ChargeEndlag },
	jumpDamage{ JumpDamage }, jumpStartup{ JumpStartup }, jumpInterval{ JumpInterval }, jumpEndlag{ JumpEndlag }
{}

void Boss1_FSM::Update(Player& player, f32 dt) {
	//std::cout << "FSM linked\n";
	Vector2 direction{};
	switch (currentState) {
		case BOSS_IDLE:
			boss->sprite.color = Color{ 0.0f, 1.0f, 0.0f, 1.0f };
			std::cout << "Boss Idle\n";
			interval += dt;
			if (interval >= 3.0f && canWalk) {
				canWalk = false;
				if (AERandFloat() >= 0.75f) {
					if (AERandFloat() >= 0.5f) target = boss->sprite.position + Vector2{ AERandFloat() * 100 + 75, AERandFloat() * 100 + 75};
					else target = boss->sprite.position - Vector2{ AERandFloat() * 100 + 75, AERandFloat() * 100 + 75};
					currentState = BOSS_WALK;
				}
			}
			if (interval >= 5.0f) {
				interval = 0.0f;
				canWalk = true;
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
		case BOSS_WALK:
			std::cout << "Boss Walk\n";
			direction = target - boss->sprite.position;
			boss->sprite.position += direction.Normalized() * 90 * dt;
			boss->shadow.position = Vector2{ boss->sprite.position.x, boss->sprite.position.y - 35 };
			if ((abs(boss->sprite.position.x - target.x) <= 1.0f && abs(boss->sprite.position.y - target.y) <= 1.0f) || 
				CollisionBoundary_Static(boss->sprite.position, boss->sprite.scale, 1600, 900)) {
				currentState = BOSS_IDLE;
			}

			break;
		case BOSS_CHARGE:
			std::cout << "Boss Charge\n";
			ChargeAttack(player, dt);
			break;
		case BOSS_JUMP:
			std::cout << "Boss Jump\n";
			JumpAttack(player, dt);
			break;
	}
}

void Boss1_FSM::ChargeAttack(Player& player, f32 dt) {
	//Vector2 direction{};
	float collisionTime{ 0.0f };
	boss->sprite.color = Color{ 1.0f, 0.0f, 0.0f, 1.0f };
	switch (attackPhase) {
	case ATTACK_CHARGE:
		std::cout << "Boss Charge Starting\n";
		target = player.position;

		interval += dt;
		if (interval >= chargeStartup) {
			interval = 0.0f;
			chargeDirection = target - boss->sprite.position;
			attackPhase = ATTACK_ATTACK;
		}

		break;

	case ATTACK_ATTACK:
		std::cout << "Boss Charge Attack\n";
		//direction = target - boss->sprite.position;

		if (CollisionIntersection_RectRect(boss->sprite.position, boss->sprite.scale, chargeDirection.Normalized() * 150 * dt,
			player.position, player.sprite.scale, player.direction * player.speed * dt, collisionTime)) {
			playerTakesDamage(player);
		}

		boss->sprite.position += chargeDirection.Normalized() * 150 * dt;
		boss->shadow.position = Vector2{ boss->sprite.position.x, boss->sprite.position.y - 35 };

		interval += dt;
		//boss->sprite.UpdateTransform();
		if (interval >= chargeInterval || CollisionBoundary_Static(boss->sprite.position, boss->sprite.scale, 1600, 900)) {
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
	//Vector2 direction{};
	float collisionTime{ 0.0f };
	boss->sprite.color = Color{ 0.0f, 0.0f, 1.0f, 1.0f };
	switch (attackPhase) {
	case ATTACK_CHARGE:
		std::cout << "Boss Jump Starting\n";
		target = player.position;

		interval += dt;
		if (interval >= 0.4f * jumpStartup) {
			boss->sprite.position += Vector2{ 0, 1 } * 180 * dt;
		}
		if (interval >= jumpStartup) {
			interval = 0.0f;
			attackPhase = ATTACK_ATTACK;
		}

		break;

	case ATTACK_ATTACK:
		std::cout << "Boss Jump Attack\n";
		interval += dt;
		boss->sprite.position = Vector2{ target.x, target.y + 180 * jumpInterval } + Vector2{ 0, -1 } * 180 * interval;
		boss->shadow.position = Vector2{ target.x, target.y - 35 };

		if (interval >= 0.9f * jumpInterval) {
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
		std::cout << "Boss Jump Ending\n";
		interval += dt;
		if (interval >= jumpEndlag) {
			interval = 0.0f;
			currentState = BOSS_IDLE;
		}

		break;
	}
}

Boss2_FSM::Boss2_FSM(Boss* Boss, f32 ChargeDamage, f32 ChargeStartup, f32 ChargeInterval, f32 ChargeEndlag,
	f32 ProjectileDamage, f32 ProjectileSpeed, f32 ProjectileStartup, f32 ProjectileInterval, f32 ProjectileEndlag)
	: Boss_FSM(Boss), attackPhase{ ATTACK_NIL }, target{}, counter{ 0 }, canWalk{ true },
	chargeDamage{ ChargeDamage }, chargeStartup{ ChargeStartup }, chargeInterval{ ChargeInterval }, chargeEndlag{ ChargeEndlag },
	projectileDamage{ ProjectileDamage }, projectileSpeed{ ProjectileSpeed }, projectileStartup { ProjectileStartup }, projectileInterval{ ProjectileInterval }, projectileEndlag{ ProjectileEndlag }
{}

void Boss2_FSM::Update(Player& player, f32 dt) {
	//std::cout << "FSM linked\n";
	Vector2 direction{};
	switch (currentState) {
	case BOSS_IDLE:
		boss->sprite.color = Color{ 0.0f, 1.0f, 0.0f, 1.0f };
		std::cout << "Boss Idle\n";
		interval += dt;
		if (interval >= 3.0f && canWalk) {
			canWalk = false;
			if (AERandFloat() >= 0.75f) {
				if (AERandFloat() >= 0.5f) target = boss->sprite.position + Vector2{ AERandFloat() * 100 + 75, AERandFloat() * 100 + 75 };
				else target = boss->sprite.position - Vector2{ AERandFloat() * 100 + 75, AERandFloat() * 100 + 75 };
				currentState = BOSS_WALK;
			}
		}
		if (interval >= 5.0f) {
			interval = 0.0f;
			canWalk = true;
			if (counter == 2) {
				currentState = BOSS_SHOOT;
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
	case BOSS_WALK:
		std::cout << "Boss Walk\n";
		direction = target - boss->sprite.position;
		boss->sprite.position += direction.Normalized() * 90 * dt;
		boss->shadow.position = Vector2{ boss->sprite.position.x, boss->sprite.position.y - 35 };
		if ((abs(boss->sprite.position.x - target.x) <= 0.5f && abs(boss->sprite.position.y - target.y) <= 0.5f) ||
			CollisionBoundary_Static(boss->sprite.position, boss->sprite.scale, 1600, 900)) {
			currentState = BOSS_IDLE;
		}

		break;
	case BOSS_CHARGE:
		std::cout << "Boss Charge\n";
		ChargeAttack(player, dt);
		break;
	case BOSS_SHOOT:
		std::cout << "Boss Jump\n";
		ProjectileAttack(player, dt);
		break;
	}
}

void Boss2_FSM::ChargeAttack(Player& player, f32 dt) {
	//Vector2 direction{};
	float collisionTime{ 0.0f };
	boss->sprite.color = Color{ 1.0f, 0.0f, 0.0f, 1.0f };
	switch (attackPhase) {
	case ATTACK_CHARGE:
		std::cout << "Boss Charge Starting\n";
		target = player.position;

		interval += dt;
		if (interval >= chargeStartup) {
			interval = 0.0f;
			chargeDirection = target - boss->sprite.position;
			attackPhase = ATTACK_ATTACK;
		}

		break;

	case ATTACK_ATTACK:
		std::cout << "Boss Charge Attack\n";
		//direction = target - boss->sprite.position;

		if (CollisionIntersection_RectRect(boss->sprite.position, boss->sprite.scale, chargeDirection.Normalized() * 150 * dt,
			player.position, player.sprite.scale, player.direction * player.speed * dt, collisionTime)) {
			playerTakesDamage(player);
		}

		boss->sprite.position += chargeDirection.Normalized() * 150 * dt;
		boss->shadow.position = Vector2{ boss->sprite.position.x, boss->sprite.position.y - 35 };

		interval += dt;
		//boss->sprite.UpdateTransform();
		if (interval >= chargeInterval || CollisionBoundary_Static(boss->sprite.position, boss->sprite.scale, 1600, 900)) {
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

void Boss2_FSM::ProjectileAttack(Player& player, f32 dt) {
	//Vector2 direction{};
	float collisionTime{ 0.0f };
	boss->sprite.color = Color{ 0.0f, 0.0f, 1.0f, 1.0f };
	switch (attackPhase) {
	case ATTACK_CHARGE:
		std::cout << "Boss Jump Starting\n";
		target = player.position;

		interval += dt;
		if (interval >= 0.4f * projectileStartup) {
			boss->sprite.position += Vector2{ 0, 1 } *180 * dt;
		}
		if (interval >= projectileStartup) {
			interval = 0.0f;
			attackPhase = ATTACK_ATTACK;
		}

		break;

	case ATTACK_ATTACK:
		std::cout << "Boss Jump Attack\n";
		interval += dt;
		boss->sprite.position = Vector2{ target.x, target.y + 180 * projectileInterval } + Vector2{ 0, -1 } *180 * interval;
		boss->shadow.position = Vector2{ target.x, target.y - 35 };

		if (interval >= 0.9f * projectileInterval) {
			if (CollisionIntersection_RectRect(target, boss->sprite.scale, Vector2{},
				player.position, player.sprite.scale, player.direction * player.speed * dt, collisionTime)) {
				playerTakesDamage(player);
			}
		}
		if (interval >= projectileInterval) {
			interval = 0.0f;
			attackPhase = ATTACK_COOLDOWN;
		}

		break;

	case ATTACK_COOLDOWN:
		std::cout << "Boss Jump Ending\n";
		interval += dt;
		if (interval >= projectileEndlag) {
			interval = 0.0f;
			currentState = BOSS_IDLE;
		}

		break;
	}
}