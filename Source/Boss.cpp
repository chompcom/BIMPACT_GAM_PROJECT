#include "AEEngine.h"
#include "Boss.h"
#include "GameStateList.h"

#include "ProjectileManager.h"

extern LV_STATES gameState;

Boss::Boss(std::string enemyName, f32 enemyHealth, f32 enemyDamage, AnimatedSprite enemySprite, TexturedSprite shadowSprite, //Sprite hpBarSprite, 
	 RoomData& currentRoom, std::vector<AttackData> attackData, std::string const& filePath, s8 font, Vector2 pos, Vector2 size)
	: name{ enemyName }, health{ enemyHealth }, damage{ enemyDamage }, currentHealth{ enemyHealth }, sprite{ enemySprite }, shadow{ shadowSprite }, shadowOffset{}, //hpBar{hpBarSprite},
	roomData{ currentRoom }, bossStateMachine{ std::make_unique<Boss_FSM>(this) }, isActive{true}
{
	healthbarInitialized = healthbar.LoadFromFilePopUp(filePath, pos, size);
	UIElement *bossName = healthbar.FindById("boss_name");
	if (bossName)
		bossName->text = enemyName;
	healthbar.SetFont(font);
	//std::cout << healthbarInitialized << '\n';


	if (enemyName == "Chimera") {
		bossStateMachine = std::make_unique<Boss1_FSM>(this, attackData[0].attackDamage, attackData[0].attackStartup, attackData[0].attackInterval, attackData[0].attackEndlag,
			attackData[1].attackDamage, attackData[1].attackStartup, attackData[1].attackInterval, attackData[1].attackEndlag,
			attackData[2].attackDamage, attackData[2].attackStartup, attackData[2].attackInterval, attackData[2].attackEndlag);
		//std::cout << typeid(bossStateMachine).name() << '\n';
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
		//std::cout << sprite.current_animation_index << sprite.current_sprite_index << '\n';
		//std::cout << sprite.current_sprite_uv_offset_x << sprite.current_sprite_uv_offset_y << '\n';
	}
	else {
		isActive = false;
		gameState = WIN;
	}
	healthbar.Update();
}

void Boss::CollideProjectile() {
	float collisionTime{ 0.0f };
	for (Projectile* proj : roomData.projectileList) {
		if (!proj->IsAlive()) continue;

		if (CollisionIntersection_RectRect(sprite.position, sprite.scale, velocity,
			proj->GetPosition(), proj->GetScale(), proj->GetVelocity(), collisionTime)) {
			if (invulnerableTimer <= 0.f) {
				//currentHealth -= proj->GetDmg();
				//std::cout << "Taken Damage: " << proj->GetDmg();
				//std::cout << "Remaining Health: " << currentHealth;
				DamageBoss(proj->GetDmg());
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
		if (gift->velocity.LengthSq() <= EPSILON) continue;

		if (CollisionIntersection_RectRect(sprite.position, Vector2(abs(sprite.scale.x), abs(sprite.scale.y)), velocity,
			gift->position, gift->giftType.sprite.scale, gift->velocity, collisionTime)) {
			if (invulnerableTimer <= 0.f) {
				//currentHealth -= 1;
				//std::cout << "Taken Damage: " << 1;
				//std::cout << "Remaining Health: " << currentHealth;
				DamageBoss(1);
				//gift->velocity = -gift->velocity;
				Vector2 dirBtwnEnemyGift = sprite.position - gift->position;
				gift->velocity = -dirBtwnEnemyGift.Normalized() * gift->velocity.Length();
				invulnerableTimer = 0.5f;
			}
		}

	}
}

void Boss::DamageBoss(s32 dmg) {
	currentHealth -= dmg;
	if (currentHealth < 0) currentHealth = 0;
	
	if (healthbarInitialized) {
		UIElement* healthUI = healthbar.FindById("foreground");
		if (healthUI) {
			healthUI->sizeRatio.x = (currentHealth / health); //* 1000.f;
			healthUI->localPos.x = -0.5f + (currentHealth / health) * 0.5f;
		}
	}
	//hpBar.scale.x = (currentHealth / health) * 1000.f;
	//hpBar.position.x = -500.f + (currentHealth / health) * 500.f;
}

void Boss::ResetBoss() {
	currentHealth = health;
	sprite.position = Vector2{};
	sprite.GetAnimation("Idle");
	shadow.position = Vector2{ 0, -shadowOffset};

	bossStateMachine->currentState = bossStateMachine->initialState;
	bossStateMachine->attackPhase = ATTACK_NIL;
	bossStateMachine->interval = 0.0f;
	bossStateMachine->counter = 0;

	if (healthbarInitialized) {
		UIElement* healthUI = healthbar.FindById("foreground");
		if (healthUI) {
			healthUI->sizeRatio.x = 1.f;
			healthUI->localPos.x = 0.f;
		}
	}
}

