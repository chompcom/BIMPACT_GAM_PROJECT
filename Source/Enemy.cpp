#include "AEEngine.h"
#include "Enemy.h"
#include "Utils/Vector2.hpp"
#include "Traits.h"
#include <set>
Enemy::Enemy(const EnemyType& enemyType, TexturedSprite enemySprite, EnemyStates initialState)
	: type{ enemyType }, sprite{ enemySprite }, currentHealth {enemyType.health}, state{ initialState }, currentBehavior{ nullptr }, target{}
{
		ChangeState(initialState);
//		mesh = CreateSquareMesh();
//		sprite.mesh = mesh;
}

Enemy::~Enemy() {
	//AEGfxMeshFree(mesh);
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

void Enemy::AssessTraits(Labels labels){
	if (HasCommonTrait(labels,type.likes)){
		ChangeState(ES_HAPPY);

	} else if (HasCommonTrait(labels,type.dislikes))
	{
		/* code */
		ChangeState(ES_ANGRY);
	}
	

}

EnemyType::EnemyType(std::string name, f32 health, f32 damage, const Labels& traits,
	const Labels& likes, const Labels& dislikes)
	: name{ name }, health {health}, damage{ damage }, traits{ traits }, likes{ likes }, dislikes{ dislikes },
		neutral{ nullptr }, happy{ nullptr }, angry{ nullptr } {
}
void EnemyType::AddNeutral(Command cmd){
	//neutral.push_back(cmd);
}
void EnemyType::AddNeutral(std::vector<Command> bunch){
	//for (Command thing : bunch)
		//neutral.push_back(thing);
}

void EnemyType::AddHappy(Command cmd){
	//happy.push_back(cmd);
}
void EnemyType::AddHappy(std::vector<Command> bunch){
}

void EnemyType::AddAngry(Command cmd){
	//neutral.push_back(cmd);
}
void EnemyType::AddAngry(std::vector<Command> bunch){
}

void WalkLeft(Enemy& me, float dt) {

	me.sprite.position += Vector2(-50, 0) * dt;
	me.sprite.UpdateTransform();
}

void WalkRight(Enemy& me, float dt){
	me.sprite.position += Vector2(50,0) * dt;
	me.sprite.color = Color{ 1.0f,0.0f,0.0f,1.0f };
	me.sprite.UpdateTransform();
}

void WalkToTarget(Enemy& me, float dt) {
	me.sprite.position += (me.target - me.sprite.position).Normalized() * 20 * dt;
	me.sprite.color = Color{ 0.0f,1.0f,0.0f,1.0f };
	me.sprite.UpdateTransform();
} 