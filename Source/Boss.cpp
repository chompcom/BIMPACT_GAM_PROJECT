#include "AEEngine.h"
#include "Boss.h"
#include "GameStateList.h"

extern LV_STATES gameState;

Boss::Boss(std::string enemyName, f32 enemyHealth, f32 enemyDamage, TexturedSprite enemySprite, TexturedSprite shadowSprite, 
	const RoomData& currentRoom, std::vector<AttackData> attackData)
	: name{ enemyName }, health{ enemyHealth }, damage{ enemyDamage }, currentHealth{ enemyHealth }, sprite{ enemySprite }, shadow{shadowSprite},
	roomData{ currentRoom }, bossStateMachine{ std::make_unique<Boss_FSM>(this) }
{
	if (enemyName == "Boss 1") {
		std::cout << "true\n";
		bossStateMachine = std::make_unique<Boss1_FSM>(this, attackData[0].attackDamage, attackData[0].attackStartup, attackData[0].attackInterval, attackData[0].attackEndlag,
			attackData[1].attackDamage, attackData[1].attackStartup, attackData[1].attackInterval, attackData[1].attackEndlag);
		std::cout << typeid(bossStateMachine).name() << '\n';
	}
	if (enemyName == "Boss 2") {
		std::cout << "true\n";
		bossStateMachine = std::make_unique<Boss2_FSM>(this, attackData[0].attackDamage, attackData[0].attackStartup, attackData[0].attackInterval, attackData[0].attackEndlag,
			attackData[1].attackDamage, 0.0f, attackData[1].attackStartup, attackData[1].attackInterval, attackData[1].attackEndlag);
		std::cout << typeid(bossStateMachine).name() << '\n';
	}
}

Boss::~Boss() {}


void Boss::Update(Player& player, f32 dt) {
	if (currentHealth > 0) {
		bossStateMachine->Update(player, dt);
		CollideProjectile();
	}
	else {
		gameState = WIN;
	}
}

void Boss::CollideProjectile() {
	float collisionTime{ 0.0f };
	for (Projectile* proj : roomData.projectileList) {
		if (!proj->IsAlive()) continue;

		if (CollisionIntersection_RectRect(sprite.position, sprite.scale, velocity,
			proj->GetPosition(), proj->GetScale(), proj->GetVelocity(), collisionTime)) {
			currentHealth -= proj->GetDmg();
			proj->RemoveProjectile();
		}

	}
}

