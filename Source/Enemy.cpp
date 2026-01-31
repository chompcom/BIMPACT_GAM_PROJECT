#include "AEEngine.h"
#include "Enemy.h"
#include "Vector2.hpp"

Enemy::Enemy(const EnemyType& enemyType, TexturedSprite enemySprite, EnemyStates initialState)
	: type{ enemyType }, sprite{ enemySprite }, currentHealth {enemyType.health}, state{ initialState }, currentBehavior{ nullptr } {
		ChangeState(initialState);
}

void Enemy::ChangeState(EnemyStates newstate)
{
	const EnemyType &enemyType = this->type;
	this->state = newstate;
	switch (newstate)
	{
	case ES_HAPPY:
		currentBehavior = enemyType.happy;
		break;
	case ES_NEUTRAL:
		currentBehavior = enemyType.neutral;
		break;
	case ES_ANGRY:
		currentBehavior = enemyType.angry;
		break;
	}
}

void Enemy::Update(float dt) {
	if (currentBehavior == nullptr) return;
	this->currentBehavior(*this,dt);
}

EnemyType::EnemyType(std::string name, f32 health, f32 damage, const Labels& traits,
	const Labels& likes, const Labels& dislikes)
	: name{ name }, health {health}, damage{ damage }, traits{ traits }, likes{ likes }, dislikes{ dislikes },
		neutral{ nullptr }, happy{ nullptr }, angry{ nullptr } {
}

void WalkLeft(Enemy& me, float dt) {
	me.sprite.position += Vector2(-100, 0) * dt;
	me.sprite.UpdateTransform();
}

void WalkRight(Enemy& me, float dt){
	me.sprite.position += Vector2(100,0) * dt;
	me.sprite.UpdateTransform();
}