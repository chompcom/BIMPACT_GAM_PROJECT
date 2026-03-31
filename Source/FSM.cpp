#include "AEEngine.h"
#include "FSM.h"
#include "BoundaryCollision.h"
#include "Collision.h"
#include "Boss.h"
#include "Sprite.h"
#include <iostream>

Boss_FSM::Boss_FSM(Boss* Boss) : boss{Boss}, interval { 0.0f }, initialState{ BOSS_IDLE }, currentState{ BOSS_IDLE }, 
	attackPhase{ ATTACK_NIL }, target{}, counter{ 0 } {}

Boss1_FSM::Boss1_FSM(Boss* Boss, f32 ChargeDamage, f32 ChargeStartup, f32 ChargeInterval, f32 ChargeEndlag,
	f32 JumpDamage, f32 JumpStartup, f32 JumpInterval, f32 JumpEndlag,
	f32 FollowDamage, f32 FollowStartup, f32 FollowInterval, f32 FollowEndlag)
	: Boss_FSM(Boss),  canWalk{true},
	chargeDamage{ ChargeDamage }, chargeStartup{ ChargeStartup }, chargeInterval{ ChargeInterval }, chargeEndlag{ ChargeEndlag },
	jumpDamage{ JumpDamage }, jumpStartup{ JumpStartup }, jumpInterval{ JumpInterval }, jumpEndlag{ JumpEndlag },
	followDamage{ FollowDamage }, followStartup{ FollowStartup }, followInterval{ FollowInterval }, followEndlag{ FollowEndlag }
{}

void Boss1_FSM::Update(Player& player, f32 dt) {
	//std::cout << "FSM linked\n";
	//Vector2 direction{};
	switch (currentState) {
		case BOSS_IDLE:
			//boss->sprite.color = Color{ 0.0f, 1.0f, 0.0f, 1.0f };
			std::cout << "Boss Idle\n";
			interval += dt;
			if (interval >= 1.0f && canWalk) {
				canWalk = false;
				if (AERandFloat() >= 0.75f) {
					if (AERandFloat() >= 0.5f) target = boss->sprite.position + Vector2{ AERandFloat() * 100 + 75, AERandFloat() * 100 + 75};
					else target = boss->sprite.position - Vector2{ AERandFloat() * 100 + 75, AERandFloat() * 100 + 75};
					boss->direction = (target - boss->sprite.position).Normalized();
					boss->velocity = boss->direction * walkSpeed * boss->speedModifier;
					boss->sprite.GetAnimation("Run");
					currentState = BOSS_WALK;
				}
			}
			if (interval >= 3.0f) {
				interval = 0.0f;
				canWalk = true;
				if (counter == 2) {
					currentState = BOSS_JUMP;
					attackPhase = ATTACK_CHARGE;
					boss->sprite.GetAnimation("Jump Start");
					counter = 0;
				}
				else {
					currentState = AERandFloat() >= 0.25f ? BOSS_CHARGE : BOSS_FOLLOW;
					attackPhase = ATTACK_CHARGE;
					//boss->sprite.GetAnimation("Run Start");
					counter++;
				}
			}
			break;
		case BOSS_WALK:
			std::cout << "Boss Walk\n";
			
			boss->sprite.position += boss->velocity * dt;
			boss->shadow.position = Vector2{ boss->sprite.position.x, boss->sprite.position.y - 35 };

			if ((abs(boss->sprite.position.x - target.x) <= 1.0f && abs(boss->sprite.position.y - target.y) <= 1.0f) || 
				//CollisionBoundary_Static(boss->sprite.position, boss->sprite.scale, 1600, 900)) {
				boss->collideWall ) {
				boss->velocity = {};
				//boss->collideWall = false;
				boss->sprite.GetAnimation("Idle");
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
		case BOSS_FOLLOW:
			std::cout << "Boss Follow\n";
			FollowAttack(player, dt);
			break;
	}
}

void Boss1_FSM::ChargeAttack(Player& player, f32 dt) {
	//Vector2 direction{};
	float collisionTime{ 0.0f };
	//boss->sprite.color = Color{ 1.0f, 0.0f, 0.0f, 1.0f };
	switch (attackPhase) {
	case ATTACK_CHARGE:
		std::cout << "Boss Charge Starting\n";
		target = player.position;
		boss->direction = (target - boss->sprite.position).Normalized();

		interval += dt;
		if (interval >= chargeStartup) {
			interval = 0.0f;
			//boss->direction = (target - boss->sprite.position).Normalized();
			boss->velocity = boss->direction * chargeSpeed * boss->speedModifier;
			boss->sprite.GetAnimation("Run");
			attackPhase = ATTACK_ATTACK;
		}

		break;

	case ATTACK_ATTACK:
		std::cout << "Boss Charge Attack\n";
		//direction = target - boss->sprite.position;

		boss->sprite.position += boss->velocity * dt;
		boss->shadow.position = Vector2{ boss->sprite.position.x, boss->sprite.position.y - 35 };

		if (CollisionIntersection_RectRect(boss->sprite.position, boss->sprite.scale * 0.8, boss->velocity * dt,
			player.position, player.sprite.scale * 0.8, player.GetVelocity() * dt, collisionTime)) {
			playerTakesDamage(player);
		}

		interval += dt;
		//boss->sprite.UpdateTransform();
		if (interval >= chargeInterval || //CollisionBoundary_Static(boss->sprite.position, boss->sprite.scale, 1600, 900)) {
			boss->collideWall) {
			interval = 0.0f;
			boss->velocity = Vector2{};
			//boss->collideWall = false;
			boss->sprite.GetAnimation("Idle");
			attackPhase = ATTACK_COOLDOWN;
		}

		break;

	case ATTACK_COOLDOWN:
		std::cout << "Boss Charge Ending\n";
		interval += dt;
		if (interval >= chargeEndlag) {
			if (AERandFloat() >= 0.5f && additionalCharges < 2) {
				interval = chargeStartup - 0.5f;
				attackPhase = ATTACK_CHARGE;
				++additionalCharges;
			}
			else {
				interval = 0.0f;
				additionalCharges = 0;
				currentState = BOSS_IDLE;
			}
			
		}

		break;
	}
}

void Boss1_FSM::JumpAttack(Player& player, f32 dt) {
	//Vector2 direction{};
	float collisionTime{ 0.0f };
	//boss->sprite.color = Color{ 0.0f, 0.0f, 1.0f, 1.0f };
	switch (attackPhase) {
	case ATTACK_CHARGE:
		std::cout << "Boss Jump Starting\n";
		target = player.position;

		interval += dt;
		if (interval >= 0.25f * jumpStartup) {
			boss->invulnerableTimer = dt;
			boss->sprite.GetAnimation("Jump");
			boss->sprite.position += Vector2{ 0, 1 } * jumpSpeed * dt;
		}
		else {
			boss->direction = (target - boss->sprite.position).Normalized();
		}

		if (interval >= jumpStartup) {
			interval = 0.0f;
			boss->sprite.GetAnimation("Jump End");
			attackPhase = ATTACK_ATTACK;
		}

		break;

	case ATTACK_ATTACK:
		std::cout << "Boss Jump Attack\n";
		interval += dt;
		boss->sprite.position = Vector2{ target.x, target.y + jumpSpeed * jumpInterval } + Vector2{ 0, -1 } * jumpSpeed * interval;
		boss->shadow.position = Vector2{ target.x, target.y - 35 };
		
		if (boss->sprite.position.y - target.y > 10) boss->invulnerableTimer = dt;
		else {
			if (CollisionIntersection_RectRect(target, boss->sprite.scale * 0.8, Vector2{},
				player.position, player.sprite.scale * 0.8, player.GetVelocity(), collisionTime)) {
				playerTakesDamage(player);
			}
		}
		if (interval >= jumpInterval) {
			interval = 0.0f;
			boss->sprite.GetAnimation("Jump Start");
			attackPhase = ATTACK_COOLDOWN;
		}

		break;

	case ATTACK_COOLDOWN:
		std::cout << "Boss Jump Ending\n";
		interval += dt;
		if (interval >= jumpEndlag) {
			interval = 0.0f;
			boss->sprite.GetAnimation("Idle");
			currentState = BOSS_IDLE;
		}

		break;
	}
}

void Boss1_FSM::FollowAttack(Player& player, f32 dt) {
	//Vector2 direction{};
	float collisionTime{ 0.0f };
	//boss->sprite.color = Color{ 1.0f, 1.0f, 0.0f, 1.0f };
	switch (attackPhase) {
	case ATTACK_CHARGE:
		std::cout << "Boss Follow Starting\n";
		target = player.position;
		boss->direction = (target - boss->sprite.position).Normalized();

		interval += dt;
		if (interval >= chargeStartup) {
			interval = 0.0f;
			//chargeDirection = target - boss->sprite.position;
			//boss->velocity = chargeDirection.Normalized() * chargeSpeed * boss->speedModifier;
			boss->sprite.GetAnimation("Run");
			attackPhase = ATTACK_ATTACK;
		}

		break;

	case ATTACK_ATTACK:
		std::cout << "Boss Follow Attack\n";
		//direction = target - boss->sprite.position;
		target = player.position;
		boss->direction = (target - boss->sprite.position).Normalized();
		boss->velocity = boss->direction * followSpeed * boss->speedModifier;

		boss->sprite.position += boss->velocity * dt;
		boss->shadow.position = Vector2{ boss->sprite.position.x, boss->sprite.position.y - 35 };

		if (CollisionIntersection_RectRect(boss->sprite.position, boss->sprite.scale * 0.8, boss->velocity * dt,
			player.position, player.sprite.scale * 0.8, player.GetVelocity() * dt, collisionTime)) {
			playerTakesDamage(player);
			interval = 0.0f;
			boss->velocity = Vector2{};
			boss->sprite.GetAnimation("Idle");
			attackPhase = ATTACK_COOLDOWN;
		}

		interval += dt;
		//boss->sprite.UpdateTransform();
		if (interval >= followInterval || //CollisionBoundary_Static(boss->sprite.position, boss->sprite.scale, 1600, 900)) {
			boss->collideWall) {
			interval = 0.0f;
			boss->velocity = Vector2{};
			//boss->collideWall = false;
			boss->sprite.GetAnimation("Idle");
			attackPhase = ATTACK_COOLDOWN;
		}

		break;

	case ATTACK_COOLDOWN:
		std::cout << "Boss Follow Ending\n";
		interval += dt;
		if (interval >= followEndlag) {
				interval = 0.0f;
				currentState = BOSS_IDLE;
		}

		break;
	}
}

/*void Boss1_FSM::ProjectileAttack(Player& player, f32 dt) {
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
}*/