#include "AEEngine.h"
#include "Boss.h"
#include "GameStateList.h"

#include "ProjectileManager.h"

extern LV_STATES gameState;

Boss::Boss(std::string enemyName, f32 enemyHealth, f32 enemyDamage, TexturedSprite enemySprite, TexturedSprite shadowSprite, 
	 RoomData& currentRoom, std::vector<AttackData> attackData)
	: name{ enemyName }, health{ enemyHealth }, damage{ enemyDamage }, currentHealth{ enemyHealth }, sprite{ enemySprite }, shadow{shadowSprite},
	roomData{ currentRoom }, bossStateMachine{ std::make_unique<Boss_FSM>(this) }, isActive{true}
{
	if (enemyName == "Boss 1") {
		bossStateMachine = std::make_unique<Boss1_FSM>(this, attackData[0].attackDamage, attackData[0].attackStartup, attackData[0].attackInterval, attackData[0].attackEndlag,
			attackData[1].attackDamage, attackData[1].attackStartup, attackData[1].attackInterval, attackData[1].attackEndlag);
		std::cout << typeid(bossStateMachine).name() << '\n';
	}
}

Boss::~Boss() {}


void Boss::Update(Player& player, f32 dt) {
	if (currentHealth > 0) {
		//collideWall = false;
		bossStateMachine->Update(player, dt);
		CollideGift();
		CollideProjectile();
		if (invulnerableTimer > 0.f) invulnerableTimer -= dt;
		speedModifier = 1.0f;
	}
	else {
		isActive = false;
		gameState = WIN;
	}
}

void Boss::CollideProjectile() {
	float collisionTime{ 0.0f };
	for (Projectile* proj : roomData.projectileList) {
		if (!proj->IsAlive()) continue;

		if (CollisionIntersection_RectRect(sprite.position, sprite.scale, velocity,
			proj->GetPosition(), proj->GetScale(), proj->GetVelocity(), collisionTime)) {
			if (invulnerableTimer <= 0.f) {
				currentHealth -= proj->GetDmg();
				std::cout << "Taken Damage: " << proj->GetDmg();
				std::cout << "Remaining Health: " << currentHealth;
				ProjectileParticleExplode(roomData, *proj);
				proj->RemoveProjectile();
				invulnerableTimer = 1.0f;
			}
		}

	}
}

void Boss::CollideGift() {
	float collisionTime{ 0.0f };
	for (Gift* gift : roomData.giftList) {
		if (gift->velocity == Vector2{}) continue;

		if (CollisionIntersection_RectRect(sprite.position, sprite.scale, velocity,
			gift->position, gift->giftType.sprite.scale, gift->velocity, collisionTime)) {
			if (invulnerableTimer <= 0.f) {
				currentHealth -= 1;
				std::cout << "Taken Damage: " << 1;
				std::cout << "Remaining Health: " << currentHealth;
				gift->velocity = -gift->velocity;
			}
		}

	}
}

void Boss::ResetBoss() {
	currentHealth = health;
	sprite.position = Vector2{};
	shadow.position = Vector2{ 0, -35 };

	bossStateMachine->currentState = bossStateMachine->initialState;
	bossStateMachine->attackPhase = ATTACK_NIL;
	bossStateMachine->interval = 0.0f;
	bossStateMachine->counter = 0;
}

