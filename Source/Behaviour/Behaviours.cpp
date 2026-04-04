
//Need to define this because algorithm don't work
#define NOMINMAX
#include "Behaviours.h"

#include <string>

#include <unordered_map>
#include "Enemy.h"
#include "BoundaryCollision.h"
#include "Collision.h"
#include "Utils/Utils.h"
#include "ProjectileManager.h"
#include "Grid.h"

#include <iostream>
#include <algorithm>

using CommandList = std::unordered_map<std::string, Command>;

using CommandPair = std::pair<std::string, Command>;

using FlagList = std::unordered_map<std::string, FlagCheck>;


// ******************************

// HELPER FUNCTIONS

// ******************************
namespace {
	bool CheckIfHated(Enemy const &me, Enemy const &enemy)
	{
		return (enemy.state == ES_ANGRY || HasCommonTrait(me.type.dislikes, enemy.type.traits));
	}

	
	using FireFunc = void(TexturedSprite, RoomData&, Vector2, Vector2, float, float, int, Vector2, Color, void*, bool);

	//template <typename FireFunc>
	void FireSomething(Enemy &me, FireFunc shootProjFunc)
	{

		//ShootProjectile(TexturedSprite{}, RoomData{}, Vector2, Vector2, float, float, int, Vector2, Color, void*, bool);


		if (!me.target) {
			std::cout << "No target to fire at" << AEFrameRateControllerGetFrameCount() <<"\n";
			return;
		}

		bool amIFriendsWithThePlayer = false;
		if (me.attackTimer <= EPSILON)
		{
			EnemyType::ProjectileInfo proj;
			switch (me.state)
			{
			case ES_ANGRY:
				proj = me.type.angryProjectile;
				break;
			case ES_HAPPY:
				proj = me.type.happyProjectile;
				amIFriendsWithThePlayer = true;
				break;
			default:
				proj = me.type.neutralProjectile;
				break;
			}

			Vector2 direction = me.prevPos - (me.target.GetPosition());

			//hacky thing to make player collide with healing projectiles
			if (proj.damage < 0.0f) amIFriendsWithThePlayer = false;

			shootProjFunc(DataLoader::CreateTexture(proj.spritePath), *me.roomData, me.prevPos, -direction.Normalized(), proj.speed, proj.lifetime, static_cast<int>(me.dmgModifier * proj.damage), Vector2(proj.radius, proj.radius), proj.color, &me, amIFriendsWithThePlayer);
			me.attackTimer = me.type.attackRate;
			me.onceAttackTime = true;
		}
	}
}


// ******************************

// BEHAVIOUR FUNCTIONS

// ******************************
namespace { //functions namespace begin
   



// ***************************************
//               CONTEXTS
// ***************************************

bool IsTouchingTarget(Enemy& me) {

	if (me.target == false) return false;

	float collTime;
	if (CollisionIntersection_RectRect(me.prevPos,me.sprite.scale * 0.5f, 
		me.velocity, 
			me.target.GetPosition(), me.sprite.scale * 0.5f,
			me.target.GetPosition() - me.target.initialPosition
			, collTime) ) {
				return true;
	}
		
    
    return false;
}

bool IsNotFollowingPlayer(Enemy& me) {

	if (AreCirclesIntersecting(me.prevPos, me.type.safeRadius,
				me.roomData->player->position, 0) ) {

		return false;

	}
	else {
		return true;
	}
}

bool IsFollowingPlayer(Enemy& me) {
	return !IsNotFollowingPlayer(me);
}

bool IsTargetInDetectionRadius(Enemy& me) {
	if (!me.target) return false;

	return (AreCirclesIntersecting(me.prevPos, me.type.detectionRadius,
		me.target.GetPosition(), 0));
}
bool IsTargetNotInDetectionRadius(Enemy& me) {
	return !IsTargetInDetectionRadius(me);
}

bool IsWanderTimerUp(Enemy& me) {
	return me.wanderTimer <= EPSILON;
}

bool IsWandering(Enemy& me) {
	return me.wanderTimer > EPSILON;
}
bool OnceWanderTimerIsUp(Enemy& me) {
	if (me.onceWanderTime && IsWanderTimerUp(me)) {
		me.onceWanderTime = false;
		return true;
	}
	return false;
}

bool IsWaitTimerUp(Enemy& me) {
	return me.waitTimer <= EPSILON;
}

bool OnceWaitTimerIsUp(Enemy& me) {
	if (me.onceWaitTime && IsWaitTimerUp(me)) {
		me.onceWaitTime = false;
		return true;
	}
	return false;
}

bool IsWaiting(Enemy& me) {
	return me.waitTimer > 0.f;
}

bool IsAttackOnCooldown(Enemy& me) {
	return me.attackTimer > EPSILON;
}

bool IsNotTargeting(Enemy& me) {
	return !me.target;
}

//By right, attacks automatically are on attack cooldown so placing 
//fire projectile in these is a little pointless
bool IsAttackCooldownUp(Enemy& me) {
	return me.attackTimer <= EPSILON;
}
bool OnceAttackCooldownUp(Enemy& me) {
	if (me.onceAttackTime && IsAttackCooldownUp(me)) {
		me.onceAttackTime = false;
		return true;
	}
	return false;
}

bool IsHittingWall(Enemy& me) {
	return me.collisionResolution;
}

//as weird as this one is, it's actually quite straight forward
bool IsOnEvenBounce(Enemy& me) {
	return me.acknowledgeCollision;
}

// ***************************************
//               ACTIONS
// ***************************************

void WalkLeft(Enemy& me) {
	me.velocity += Vector2(-1, 0);
}

void WalkRight(Enemy& me){
	me.velocity += Vector2(1,0);
}

void MoveToTarget(Enemy& me) {
	if (me.target == false) {
		me.velocity = Vector2();
		return;
	}
	Vector2 direction{ (me.target.GetPosition() - me.prevPos)};
	me.velocity += direction;
	me.velocity = me.velocity.Normalized();
} 

void ApplySlowToTarget(Enemy& me) {
	if (me.target == false) return;
	if (me.state == ES_HAPPY && me.target.isPlayer) return;


	me.target.GetSpeedMod() = 0.2f;

}

void Wander(Enemy& me) {
	if (me.wanderTimer <= EPSILON) {
		me.wanderTimer = 3.f;
		me.onceWanderTime = true;
		me.velocity = Vector2{};
	}
	if (me.velocity == Vector2{}) {
		float randx = AERandFloat() * 2 - 1;
		float randy = AERandFloat() * 2 - 1;
		me.velocity = Vector2{randx, randy};
	}
	if (me.collisionResolution)
		me.velocity = -me.velocity;

	
}

void CircleMove(Enemy& me) {

	if (!me.target) return;

	Vector2 direction{ (me.target.GetPosition() - me.prevPos)};
	direction = Vector2{direction.y, -direction.x}; //the perpendicular
	if (me.acknowledgeCollision) {
		direction = -direction;
	}
	me.velocity = direction;
}

void TargetEnemyInDetectionRadius(Enemy& me){
	for (Enemy* guy : me.roomData->enemyList){
		if (!guy->isActive || !CheckIfHated(me,*guy)) continue;
		if (AreCirclesIntersecting(me.prevPos, me.type.detectionRadius,
			guy->prevPos, guy->sprite.scale.x)) {

				me.target = *guy;
				return; //found a guy
			}
	}
	//If the boss exists.. perhaps target them
	if (me.roomData->boss) me.target = *me.roomData->boss;
	//if i didn't find anything, DON'T retarget...

}

//Dumb version that includes neutral mobs
void TargetEnemyInDetectionRadiusDumb(Enemy& me) {
	for (Enemy* guy : me.roomData->enemyList) {
		if (!guy->isActive || (guy == &me)) continue;
		if (AreCirclesIntersecting(me.prevPos, me.type.detectionRadius,
			guy->prevPos, guy->sprite.scale.x)) {

			me.target = *guy;
			return; //found a guy
		}
	}
	//If the boss exists
	if (me.roomData->boss) me.target = *me.roomData->boss;

}

void TargetPlayer(Enemy& me) {

	if (me.roomData->player)
		me.target = *me.roomData->player;
}

void SafeDistancePlayer(Enemy& me) {
	Vector2 playerPos = me.roomData->player->position;
	if (AreCirclesIntersecting(me.prevPos, me.sprite.scale.x, playerPos, me.type.safeRadius) ) {

		Vector2 direction{ (playerPos - me.prevPos) };
		direction = -direction;
		me.velocity = direction;
	}
}
void TargetRandomEnemy(Enemy& me) {
	
	std::vector<Enemy*> aliveList{};
	//check if all dead
	for (Enemy* enemy : me.roomData->enemyList) {
		if (enemy->isActive && CheckIfHated(me,*enemy)) aliveList.push_back(enemy);
	}
	//Very special case, this behaviour specifically targets enemies first! 
	//Once the enemies are gone then the boss is targeted!
	if (aliveList.empty()) {
		if (me.roomData->boss) me.target = *me.roomData->boss;
		return;
	}
	me.target = *me.roomData->enemyList[std::rand() % aliveList.size()];
}


//Dumb version for neutral mobs
void TargetRandomEnemyDumb(Enemy& me) {

	std::vector<Enemy*> aliveList{};
	//check if all dead
	for (Enemy* enemy : me.roomData->enemyList) {
		if (enemy->isActive) aliveList.push_back(enemy);
	}

	//Very special case, this behaviour specifically targets enemies first! 
	//Once the enemies are gone then the boss is targeted!
	if (aliveList.empty()) {
		if (me.roomData->boss) me.target = *me.roomData->boss;
		return;
	}
	me.target = *me.roomData->enemyList[std::rand() % aliveList.size()];
}

void TargetNearestThing(Enemy& me) {
	float smol = me.roomData->player->position.DistanceSq(me.prevPos);
	float tmp{ smol };
	Enemy::Target tempTarget{};
	tempTarget = *me.roomData->player;
	smol = me.roomData->boss ? (std::min(me.roomData->boss->sprite.position.DistanceSq(me.prevPos), smol)) : smol;
	if (smol != tmp) {
		tempTarget = *me.roomData->boss;
		tmp = smol;
	}

	for (Enemy* enemy : me.roomData->enemyList) {
		if (!(enemy && enemy->isActive)) continue;
		smol =  std::min(enemy->prevPos.DistanceSq(me.prevPos), smol);
		if (smol != tmp) {
			tempTarget = *me.roomData->boss;
			tmp = smol;
		}
	}
	me.target = tempTarget;
}

void TargetMiddle(Enemy& me) {
	//when the commented out version was used it doesnt work? i think?
	//me.target.initialPosition = Vector2();
	me.target = Vector2();
}
void TargetCorner(Enemy& me) {

	//me.target.initialPosition = Vector2(100,100);
	me.target = Vector2(100, 100);
}
void FireProjectile(Enemy& me) {
	FireSomething(me, ShootProjectile);
}

void FireAOE(Enemy& me) {
	if (!me.target)
		return;

	bool amIFriendsWithThePlayer = false;
	if (me.attackTimer <= EPSILON)
	{
		EnemyType::ProjectileInfo proj;
		switch (me.state)
		{
		case ES_ANGRY:
			proj = me.type.angryProjectile;
			break;
		case ES_HAPPY:
			proj = me.type.happyProjectile;
			amIFriendsWithThePlayer = true;
			break;
		default:
			proj = me.type.neutralProjectile;
			break;
		}

		Vector2 direction = me.prevPos - (me.target.GetPosition());

		ShootAOE(DataLoader::CreateTexture(proj.spritePath), *me.roomData, me.prevPos, proj.speed, proj.lifetime, static_cast<int>(me.dmgModifier * proj.damage), Vector2(proj.radius, proj.radius), proj.color, &me, amIFriendsWithThePlayer);
		me.attackTimer = me.type.attackRate;
		me.onceAttackTime = true;
	}
}

void FireBoomerang(Enemy& me) {
	FireSomething(me, ShootBoomerang);
}

void FireScatter(Enemy& me) {
	FireSomething(me, ShootScatter);
}

void FireSpirally(Enemy& me) {
	if (!me.target)
		return;

	bool amIFriendsWithThePlayer = false;
	if (me.attackTimer <= EPSILON)
	{
		EnemyType::ProjectileInfo proj;
		switch (me.state)
		{
		case ES_ANGRY:
			proj = me.type.angryProjectile;
			break;
		case ES_HAPPY:
			proj = me.type.happyProjectile;
			amIFriendsWithThePlayer = true;
			break;
		default:
			proj = me.type.neutralProjectile;
			break;
		}

		Vector2 direction = me.prevPos - (me.target.GetPosition());

		ShootRounding(DataLoader::CreateTexture(proj.spritePath), *me.roomData, me.prevPos, -direction.Normalized(), proj.speed, proj.lifetime, static_cast<int>(me.dmgModifier * proj.damage), Vector2(proj.radius, proj.radius), proj.color, &me, amIFriendsWithThePlayer);
		me.attackTimer = me.type.attackRate;
		me.onceAttackTime = true;
	}
}
void DVDMove(Enemy& me) {
	if (me.velocity.LengthSq() <= EPSILON) {
		me.velocity = Vector2(cosf(AERandFloat() * 2.f * PI), -sinf(AERandFloat() * 2.f * PI));
	}

	if (me.collisionResolution) {

		//yeah otherwise just take whatever velocity i alr had and run with it.
		if (me.collisionResolution & COLLISION_TOP) {

			me.velocity.y = -1.f;

		}

		if (me.collisionResolution & COLLISION_BOTTOM) {

			me.velocity.y = 1.f;

		}

		if (me.collisionResolution & COLLISION_LEFT)
		{

			me.velocity.x = 1.f;

		}
		if (me.collisionResolution & COLLISION_RIGHT) {

	
			me.velocity.x = -1.f;
		}

		float width = me.roomData->grid.GetWidth() * me.roomData->grid.GetTileWidth();
		float height = me.roomData->grid.GetHeight() * me.roomData->grid.GetTileHeight();

		//door bs
		if (CollisionBoundary_Static(me.prevPos, me.sprite.scale, (int)width, (int)height)) {

			if (me.prevPos.x < -EPSILON) {
				//left wall
				me.velocity.x = 1.f;
			}
			if (me.prevPos.x > EPSILON) {
				//right wall
				me.velocity.x = -1.f;
			}
			if (me.prevPos.y < -EPSILON) {
				//bot wall
				me.velocity.y = 1.f;
			}
			if (me.prevPos.y > EPSILON) {
				//tob wall
				me.velocity.y = -1.f;
			}
		}



	}
}
// bounce against enemies
void DVDBounce(Enemy& me) {

	if (!me.target) return; //there's no reason to bounce without a target to bounce off of.
	

	//i don't really feel like doing much square related math

	me.velocity = ( me.prevPos - me.target.GetPosition());

}
void BecomeAngry(Enemy& me) {
	me.ChangeState(ES_ANGRY);
}
void BecomeNeutral(Enemy& me) {
	me.ChangeState(ES_NEUTRAL);
}

//Warning about this guy, it's like free damage. Make sure the context is appropriate!
void DamageTarget(Enemy& me) {

	if (!me.target) return;

	if (me.attackTimer <= EPSILON) {
		me.attackTimer = me.type.attackRate;
		
		if (me.target.isPlayer) {
			if (me.type.damage < 0.f) {
				PlayerHealsDamage(*me.roomData->player);
			}
			else if (me.state != ES_HAPPY) {
				PlayerTakesDamage(*me.roomData->player);
			}
		}
		me.target.DealDamage(me.type.damage);
	}

}

void InvertVelocity(Enemy& me) {
	//me.velocity = -me.velocity; doesn't work
	std::cout << "Velocity Inverted" << std::endl;
	me.speedModifier *= -1.f;
	
}

//dangerous, handle with care
void DizzyTarget(Enemy& me) {
	if (!me.target) return;
	//specifically make them go backwards
	me.target.GetSpeedMod() = me.target.GetSpeedMod() > 0.f ? me.target.GetSpeedMod() * -1.f : me.target.GetSpeedMod();
}
//not dangerous, handle without care
void UndizzyTarget(Enemy& me) {
	if (!me.target) return;
	//specifically make them go forwards
	me.target.GetSpeedMod() = me.target.GetSpeedMod() < 0.f ? me.target.GetSpeedMod() * -1.f : me.target.GetSpeedMod();
}

void TargetSelf(Enemy& me) {
	me.target = me; //wow!
}

void Wait(Enemy& me) {
	
	if (me.waitTimer <= EPSILON) {
		me.waitTimer = 3;
		me.onceWaitTime = true;
	}
}

void ChargeAtTarget(Enemy& me) {
	if (!me.target) {
		//again, don't move if we don't have a position to go to
		me.velocity = Vector2();
		return;
	}
	me.speedModifier *= 2.0f;
	me.velocity += me.target.initialPosition - me.prevPos;
	me.velocity = me.velocity.Normalized() * me.speedModifier;
	float pos;
	if (
		CollisionIntersection_RectRect(me.prevPos, me.sprite.scale, me.velocity, me.target.initialPosition, Vector2(1, 1), Vector2(), pos)
		) me.target = Enemy::Target();
	std::cout << me.velocity.x << " " << me.velocity.y << std::endl;
}

void ClearTarget(Enemy& me) {
	//Set target to be empty
	me.target = Enemy::Target{};
}

void StopMoving(Enemy& me) {
	//Set target to be empty
	me.velocity = Vector2();
}

//unused template functions
bool DefaultFlag(Enemy& me){
	return me.isActive;
}

void DefaultAction(Enemy& me) {
	std::cerr << me.type.name << " has called a default action which should not happen ever!\n";
	std::cerr << "Did you misspell an action?\n";
	return;
}


}//end functions namspace

static CommandList commands;
static FlagList flags;
Command GetCommand(std::string name) {
	if (commands.find(name) == commands.end()) {
		std::cerr << "ERROR! " << name << " didn't load properly! Likely misspelling.\n";
		return commands["default"];
	}
    return commands[name];
}

FlagCheck GetFlag(std::string name) {
	if (flags.find(name) == flags.end()) {
		return flags["default"];
	}
    return flags[name];
}

void InitCommands() {
    //just a guess as to how many commands we are expecting
    commands.reserve(30);
    flags.reserve(30);

	flags = {
		{"IsTouchingTarget",IsTouchingTarget},
		{"IsNotFollowingPlayer", IsNotFollowingPlayer},
		{"IsFollowingPlayer", IsFollowingPlayer},
		{"IsTargetInDetectionRadius", IsTargetInDetectionRadius},
		{"IsTargetNotInDetectionRadius", IsTargetNotInDetectionRadius},
		{"IsWaitTimerUp", IsWaitTimerUp},
		{"OnceWaitTimerIsUp", OnceWaitTimerIsUp}, 
		{"IsWanderTimerUp", IsWanderTimerUp},
		{"OnceWanderTimerIsUp", OnceWanderTimerIsUp}, 
		{"IsWandering", IsWandering },
		{ "IsWaiting" , IsWaiting },
		{"IsAttackOnCooldown", IsAttackOnCooldown},
		{"IsAttackCooldownUp", IsAttackOnCooldown},
		{"OnceAttackCooldownUp", OnceAttackCooldownUp}, 
		{"IsHittingWall", IsHittingWall}, 
		{"IsNotTargeting",IsNotTargeting},
		{"IsOnEvenBounce", IsOnEvenBounce},
		{"default", DefaultFlag}
    };


    commands = {
        {"WalkLeft", WalkLeft},
        {"WalkRight", WalkRight},
		{"ApplySlowToTarget",ApplySlowToTarget},
		{"MoveToTarget", MoveToTarget},
		{"Wander", Wander},
		{"CircleMove", CircleMove},
		{"TargetEnemyInDetectionRadius", TargetEnemyInDetectionRadius},
		{"TargetEnemyInDetectionRadiusDumb", TargetEnemyInDetectionRadiusDumb},
		{"TargetPlayer",TargetPlayer},
		{"SafeDistancePlayer",SafeDistancePlayer},
		{"TargetRandomEnemy",TargetRandomEnemy},
		{"TargetRandomEnemyDumb",TargetRandomEnemyDumb},
		{"TargetMiddle", TargetMiddle},
		{"TargetCorner", TargetCorner},
		{"FireProjectile", FireProjectile},
		{"FireBoomerang", FireBoomerang},
		{"FireScatter", FireScatter},
		{"FireSpirally", FireSpirally},
		{"DVDMove", DVDMove},
		{"DVDBounce", DVDBounce},
		{"BecomeAngry", BecomeAngry},
		{"BecomeNeutral", BecomeNeutral},
		{"DamageTarget", DamageTarget},
		{"Wait", Wait},
		{"ClearTarget", ClearTarget},
		{"TargetSelf", TargetSelf},
		{"ChargeAtTarget", ChargeAtTarget},
		{"TargetNearestThing", TargetNearestThing},
		{"InvertVelocity", InvertVelocity},
		{"DizzyTarget", DizzyTarget},
		{"UndizzyTarget", UndizzyTarget},
		{"StopMoving", StopMoving},
		{"FireAOE", FireAOE},


		{"default", DefaultAction} //This should Never be called!
    };
}


//anonymous namespace for reasons
namespace {
    

} //end anonymous namespace
