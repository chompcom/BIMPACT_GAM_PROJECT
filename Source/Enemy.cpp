#include "AEEngine.h"
#include "Enemy.h"
#include "Utils/Vector2.hpp"
#include "Traits.h"
#include "BoundaryCollision.h"
#include <set>
#include <map>

Enemy::Enemy(const EnemyType& enemyType,  TexturedSprite enemySprite, TexturedSprite shadowSprite, EnemyStates initialState)
	: type{ enemyType }, sprite{ enemySprite }, currentHealth {enemyType.health}, state{ initialState }, currentBehavior{}, target{}
	,wanderTimer{}, shadow{shadowSprite}
	,speedModifier{1.f}, dmgModifier{1.f}
	,attackTimer{}
{
		ChangeState(initialState);
}



Enemy::Target::Target() : 
	position{nullptr}, initialPosition{}, isPlayer{false}, isActive{false}
	,speedMod{nullptr}, dmgMod{nullptr}
{ }

Enemy::Target::~Target() {
	position = nullptr;
	isPlayer = false;
	isActive = false;
}

Enemy::Target& Enemy::Target::operator=(Enemy& them) {
	position = &them.sprite.position;	
	initialPosition = them.sprite.position;
	isActive = true;
	isPlayer = false;
	speedMod = &them.speedModifier;
	dmgMod = &them.dmgModifier;
	return *this;
}
Enemy::Target& Enemy::Target::operator=(Player& them) {

	isActive = true;
	isPlayer = true;
	position = &them.sprite.position;
	initialPosition = them.sprite.position;
	speedMod = &them.speed;
	dmgMod = nullptr;
	return *this;
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
		FriendSuccessAudio();
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
	for (auto& combi : currentBehavior) {
		if ( combi.first(*this)) { //so we are doing the check one at a time
			for (Command& actions : combi.second){
				actions(*this,dt);
			}
			break;//don't do the other checks
		}
	}

	//update movement here
	sprite.position += velocity.Normalized() * type.speed * dt * speedModifier;
	speedModifier = 1.0f;
	sprite.UpdateTransform();
	
	shadow.position = sprite.position;
	shadow.UpdateTransform();

	if (wanderTimer > 0.f) 
		wanderTimer -= dt;

	if (attackTimer > 0.f) 
		attackTimer -= dt;
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
	: name{ name }, health{ health }, damage{ damage }, traits{ traits }, likes{ likes }, dislikes{ dislikes }, neutral{}, angry{}, happy{}, detectionRadius{}, safeRadius{}, speed{}
{
}

void EnemyType::AddNeutral(FlagCheck const& checker, BundledBehaviour const& behaviours)
{
	neutral.push_back({ checker, behaviours });
}

void EnemyType::AddAngry(FlagCheck const& checker, BundledBehaviour const& behaviours)
{
	angry.push_back({ checker, behaviours });
}

void EnemyType::AddHappy(FlagCheck const& checker, BundledBehaviour const& behaviours)
{
	happy.push_back({ checker, behaviours });
}
