#include "AEEngine.h"
#include "Enemy.h"
#include "Utils/Vector2.hpp"
#include "Traits.h"
#include "BoundaryCollision.h"
#include <set>
#include <map>
#include <Grid.h>
#include "Loaders/DataLoader.h"
#include "Audio.h"


Enemy::Enemy(const EnemyType& enemyType,  TexturedSprite enemySprite, TexturedSprite shadowSprite, EnemyStates initialState)
	: type{ enemyType }, sprite{ enemySprite }, currentHealth {enemyType.health}, state{ initialState }, currentBehavior{}, target{}
	, wanderTimer{}, waitTimer{}, shadow{ shadowSprite }, prevPos{ enemySprite.position }
	,speedModifier{1.f}, dmgModifier{1.f}
	,attackTimer{}, isActive{true}
	, onceWanderTime{false}, onceAttackTime{false}, onceWaitTime{false} 
	, collisionResolution{}, acknowledgeCollision{false}
{
		ChangeState(initialState);
}



Enemy::Target::Target() : 
	position{nullptr}, initialPosition{}, isPlayer{false}, isActive{nullptr}
	, speedMod{ nullptr }, dmgMod{ nullptr }, info{}, health{nullptr}
{ }

Enemy::Target::~Target() {
	position = nullptr;
	isPlayer = false;
	isActive = nullptr;
}

Enemy::Target& Enemy::Target::operator=(Enemy& them) {
	position = &them.sprite.position;	
	initialPosition = them.sprite.position;
	isActive = &them.isActive;
	isPlayer = false;
	health = &them.currentHealth;
	speedMod = &them.speedModifier;
	dmgMod = &them.dmgModifier;
	return *this;
}
Enemy::Target& Enemy::Target::operator=(Player& them) {

	isActive = &them.isTargetable;
	isPlayer = true;
	position = &them.sprite.position;
	initialPosition = them.sprite.position;
	speedMod = &them.speed;
	health = nullptr;
	dmgMod = nullptr;
	return *this;
}

Enemy::Target& Enemy::Target::operator=(Boss& them)
{
	
	isActive = &them.isActive;
	isPlayer = false;
	position = &them.sprite.position;
	initialPosition = them.sprite.position;

	health = &them.currentHealth;
	
	speedMod = &them.speedModifier;
	//The boss doesn't need a damage modifier
	dmgMod = nullptr;
	return *this;
}
Enemy::Target& Enemy::Target::operator=(Vector2 const& pos)
{
	//by design, the target will be forever alive
	static bool trueVar = true;
	isActive = &trueVar;
	isPlayer = false;
	initialPosition = pos;
	this->position = &initialPosition;
	//Keeping the target as dumb as possible.
	info = 0.0f;
	health = nullptr;
	speedMod = &info;
	dmgMod = &info;
	return *this;
}

Enemy::Target::operator bool() const
{
	if (isActive) //check for nullptr
		return *isActive;
	else
		return false;
}

bool Enemy::Target::GetActive() const
{
	return static_cast<bool>(*this);
}

Vector2 Enemy::Target::GetPosition() const
{
	if (!position) {
		std::cout << "Position is nullptr! Did you set position correctly?\n";
		return initialPosition;
	}
	return *position;
}

float& Enemy::Target::GetSpeedMod()
{
	if (!speedMod) return info;
	return *speedMod;
}

float Enemy::Target::GetSpeedMod() const
{
	if (!speedMod) return 0.0f;
	return *speedMod;
}

float& Enemy::Target::GetDmgMod()
{
	if (!dmgMod) return info;
	return *dmgMod;
}

float Enemy::Target::GetDmgMod() const
{
	if (!dmgMod) return 0.0f;
	return *dmgMod;
}

void Enemy::Target::DealDamage(float dmg)
{

	if (health) {
		*health -= dmg;
	}
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
		if (type.name == "Tom") TomWhateverAudio();
		sprite.color = { 0.f,1.f,0.f,1.f };

		//When I become a friend, the others in the room will start judging
		for (Enemy* enemy : roomData->enemyList) {
			if (enemy->type.name == type.name) continue; //Siblings do not judge.
			enemy->AssessTraits(this->type.traits, false);
		}

		currentBehavior = enemyType.happy;
		break;
	case ES_NEUTRAL:
		sprite.color = { 1.f,1.f,1.f,1.f };
		currentBehavior = enemyType.neutral;
		break;
	case ES_ANGRY:
		PlayMobSound(type.angrySound);
		this->attackTimer = 3 + ((AERandFloat() * 2.f) - 1.f);
		sprite.color = { 1.f,0.f,0.f,1.f };
		currentBehavior = enemyType.angry;
		break;
	}
	this->target = Target{};

}

void Enemy::Update(float dt) {

	if (!isActive) return;

	if (currentHealth <= EPSILON) {
		isActive = false;
	}

	for (auto& combi : currentBehavior) {
		if ( combi.first(*this)) { //so we are doing the check one at a time
			for (Command& actions : combi.second){
				actions(*this);
			}
			break;//don't do the other checks
		}
	}

	//update movement here
	prevPos = sprite.position;
	sprite.position += velocity.Normalized() * type.speed * dt * speedModifier;
	
	shadow.position = sprite.position - Vector2(0,40);
	shadow.UpdateTransform();

	if (wanderTimer > EPSILON) {
		wanderTimer -= dt;
	} 

	if (attackTimer > EPSILON) {
		attackTimer -= dt;
	}

	if (waitTimer > EPSILON) {
		waitTimer -= dt;
	}

	//do collision resolution
	int prevCell = roomData->grid.WorldToCell(sprite.position.x, sprite.position.y);
	int collisionRes = roomData->grid.CheckMapGridCollision(sprite.position.x, sprite.position.y, sprite.scale.x, sprite.scale.y, prevCell);
	//gonna try to make it dynamic..

	float gridWidth = roomData->grid.GetTileWidth();
	float gridHeight = roomData->grid.GetTileHeight();

	//I really hate hotspot collision
	// Anyways, this whole portion of the code is interacting with grid

	if (collisionRes & COLLISION_RIGHT && velocity.Normalized().x > EPSILON) {
		sprite.position.x = roomData->grid.CellToWorldCenter(prevCell).x + gridWidth*0.5f - sprite.scale.x*0.5f - 0.10f;
	}
	if (collisionRes & COLLISION_LEFT && velocity.Normalized().x < -EPSILON) {
		sprite.position.x = roomData->grid.CellToWorldCenter(prevCell).x - gridWidth * 0.5f + sprite.scale.x * 0.5f + 0.10f;
	}
	if (collisionRes & COLLISION_BOTTOM && velocity.Normalized().y < -EPSILON) {
		sprite.position.y = roomData->grid.CellToWorldCenter(prevCell).y - gridHeight * 0.5f + sprite.scale.y * 0.5f + 0.10f;
	}
	if ( collisionRes & COLLISION_TOP && velocity.Normalized().y > EPSILON) {
		sprite.position.y = roomData->grid.CellToWorldCenter(prevCell).y + gridHeight * 0.5f - sprite.scale.y * 0.5f - 0.10f;
	}

	//I take half of grid boundary because that's the position of each side
	Vector2 roomBounds = roomData->grid.GetBoundary() * 0.98f * 0.5f;

	if (CollisionBoundary_Static(sprite.position, sprite.scale, roomData->grid.GetBoundary().x * 0.99f, roomData->grid.GetBoundary().y * 0.99f)) {

		collisionRes |= (sprite.position.x - sprite.scale.x * 0.5f) <= -roomBounds.x ? COLLISION_LEFT : 0;
		if (collisionRes & COLLISION_LEFT && velocity.Normalized().x > EPSILON) sprite.position.x = -roomBounds.x + sprite.scale.x * 0.5f;
		collisionRes |= (sprite.position.x + sprite.scale.x * 0.5f) >= roomBounds.x ? COLLISION_RIGHT : 0;
		if (collisionRes & COLLISION_RIGHT && velocity.Normalized().x < -EPSILON) sprite.position.x = roomBounds.x - sprite.scale.x * 0.5f;
		collisionRes |= (sprite.position.y + sprite.scale.y*0.5f) >= roomBounds.y ? COLLISION_TOP : 0;
		if (collisionRes & COLLISION_TOP && velocity.Normalized().y < -EPSILON) sprite.position.y = roomBounds.y - sprite.scale.y * 0.5f;
		collisionRes |= (sprite.position.y - sprite.scale.y*0.5f) <= -roomBounds.y ? COLLISION_BOTTOM : 0;
		if (collisionRes & COLLISION_BOTTOM && velocity.Normalized().y > EPSILON) sprite.position.y = -roomBounds.y + sprite.scale.y * 0.5f;
	}

	this->collisionResolution = collisionRes;


	if (collisionRes) acknowledgeCollision = !acknowledgeCollision;
	//After i update my movement, i reset back the speed modifier but keep the sign
	speedModifier = speedModifier / abs(speedModifier);
	sprite.UpdateTransform();

}

void Enemy::AssessTraits(Labels labels, bool giftCheck){
		for (std::string thing : type.dislikes) {
			std::cout << "I hate " << thing << " ";
		}
	if (giftCheck && HasCommonTrait(labels,type.likes) && !HasCommonTrait(labels,type.dislikes)){
		ChangeState(ES_HAPPY);
	} else if (HasCommonTrait(labels,type.dislikes))
	{
		/* code */
		
		std::cout << type.name << " is Angry!\n";
		ChangeState(ES_ANGRY);
	}
	
}

EnemyType::EnemyType(std::string name, f32 health, f32 damage, const Labels& traits,
	const Labels& likes, const Labels& dislikes)
	: name{ name }, health{ health }, damage{ damage }, traits{ traits }, likes{ likes }, dislikes{ dislikes }, neutral{}, angry{}, happy{}, detectionRadius{}, safeRadius{}, speed{}, attackRate{}, wanderTime{3.f}, waitTime{3.f},
	happyProjectile{}, angryProjectile{}, neutralProjectile{}
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
