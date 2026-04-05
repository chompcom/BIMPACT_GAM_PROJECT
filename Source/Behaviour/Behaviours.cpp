/* Start Header ************************************************************************/
/*!
\file		Behaviours.cpp
\author 	Hong Josiah Qin, hong.j, 2501239
\par  		hong.j@digipen.edu
\brief		Contains the various actions that the enemies can do. 
All actions that the enemies can make should be put into the key-value pair called "CommandPair"
in InitCommands, where the DataLoader can access the names of the function via a string instead.

Copyright (C) 2026 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited.
*/
/* End Header **************************************************************************/


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

//An unordered map with key-value pair of string and Command.
//Command is a void function that looks like "void func(Enemy& me)"
using CommandList = std::unordered_map<std::string, Command>;

//A key-value pair of string and Command
//Command is a void function that looks like "void func(Enemy& me)"
using CommandPair = std::pair<std::string, Command>;

//An unordered map with key-value pair of string and FlagCheck.
//FlagCheck is a bool function that looks like "bool func(Enemy& me)"
using FlagList = std::unordered_map<std::string, FlagCheck>;


// ******************************

// HELPER FUNCTIONS

// ******************************
namespace {

	//! Helper function to determine if the enemy is targetable usually
	//! Returns true when enemy is Angry or is Disliked. 
	bool CheckIfHated(Enemy const &me, Enemy const &enemy)
	{
		return (enemy.state == ES_ANGRY || HasCommonTrait(me.type.dislikes, enemy.type.traits));
	}


	//This is for the function that shoots projectiles
	std::pair<EnemyType::ProjectileInfo, bool> AssessProjectileType(Enemy& me) {
		
		bool amIFriendsWithThePlayer = false;
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

		//hacky thing to make player collide with healing projectiles
		if (proj.damage < 0.0f) amIFriendsWithThePlayer = false;
		return std::make_pair(proj, amIFriendsWithThePlayer);
		
	}

	// I need this function because 
	using FireFunc = void(TexturedSprite, RoomData&, Vector2, Vector2, float, float, int, Vector2, Color, void*, bool);

	//There's too many different projectile shoots, and this is the only way to choose between them without needing to copy code.
	void FireSomething(Enemy &me, FireFunc shootProjFunc)
	{



		if (!me.target) {
			std::cout << "No target to fire at" << AEFrameRateControllerGetFrameCount() <<"\n";
			return;
		}


		if (me.attackTimer <= EPSILON)
		{
			std::pair<EnemyType::ProjectileInfo, bool> answer = AssessProjectileType(me);
			EnemyType::ProjectileInfo proj = answer.first;
			bool amIFriendsWithThePlayer = answer.second;
			Vector2 direction = me.prevPos - (me.target.GetPosition());
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

//! Checks if the target is touching me
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

//! Checks if me is not within player's safeRadius
bool IsNotFollowingPlayer(Enemy& me) {

	if (AreCirclesIntersecting(me.prevPos, me.type.safeRadius,
				me.roomData->player->position, 0) ) {

		return false;

	}
	else {
		return true;
	}
}

//! checks if me is within player's safeRadius
bool IsFollowingPlayer(Enemy& me) {
	return !IsNotFollowingPlayer(me);
}

//! Checks if target (could be anyone) is within detectionRadius
bool IsTargetInDetectionRadius(Enemy& me) {
	if (!me.target) return false;

	return (AreCirclesIntersecting(me.prevPos, me.type.detectionRadius,
		me.target.GetPosition(), 0));
}
//! Checks if target (could be anyone) is not within detectionRadius
bool IsTargetNotInDetectionRadius(Enemy& me) {
	return !IsTargetInDetectionRadius(me);
}
//! Checks if the Wander Timer is up
//! you usually want to use this as a second type of timer
bool IsWanderTimerUp(Enemy& me) {
	return me.wanderTimer <= EPSILON;
}

//! Checks if i'm still in "wandering"
//! This is usually if you want to have extra behaviour while the guy is moving around. but only specifically when he's is moving because of wander.
bool IsWandering(Enemy& me) {
	return me.wanderTimer > EPSILON;
}

//! Calls this once when the wander timer is up
//! It's pretty useful I use it a lot because IsWanderTimerUp is always true until Wander is called.
bool OnceWanderTimerIsUp(Enemy& me) {
	if (me.onceWanderTime && IsWanderTimerUp(me)) {
		me.onceWanderTime = false;
		return true;
	}
	return false;
}
//! Checks if waitTimer is 0 or lesser
bool IsWaitTimerUp(Enemy& me) {
	return me.waitTimer <= EPSILON;
}

//! Called only once wait timer is up.
//! similar to OnceWanderTimerIsUp, it's really useful for the actions you want to do 
//! *only* when the wait timer has ticked down and nothing else is needed of the timer.
bool OnceWaitTimerIsUp(Enemy& me) {
	if (me.onceWaitTime && IsWaitTimerUp(me)) {
		me.onceWaitTime = false;
		return true;
	}
	return false;
}

//! Checks if the wait timer is active
//! You do not need to use the wait time to actually wait for anything,
//! You could very well have entire behaviours done during the wait.
bool IsWaiting(Enemy& me) {
	return me.waitTimer > 0.f;
}

//! Returns if the attackTimer is active. 
//! Usually triggered after "FireProjectile" and its children, or "DamageTarget"
bool IsAttackOnCooldown(Enemy& me) {
	return me.attackTimer > EPSILON;
}

//! Returns if the target is not active
//! It's useful for state management
bool IsNotTargeting(Enemy& me) {
	return !me.target;
}

//By right, attacks automatically are on attack cooldown so placing 
//fire projectile in these is a little pointless
bool IsAttackCooldownUp(Enemy& me) {
	return me.attackTimer <= EPSILON;
}

//! only called once the attackTimer is ticked down
//! Useful for clean up actions you want to do before firing again i suppose
bool OnceAttackCooldownUp(Enemy& me) {
	if (me.onceAttackTime && IsAttackCooldownUp(me)) {
		me.onceAttackTime = false;
		return true;
	}
	return false;
}

//! Returns true when enemy is hitting a wall.
bool IsHittingWall(Enemy& me) {
	return me.collisionResolution;
}

//as weird as this one is, it's actually quite straight forward
// returns true on the first bounce, then false on the second. And subsequently true after and false and so on.
bool IsOnEvenBounce(Enemy& me) {
	return me.acknowledgeCollision;
}

// ***************************************
//               ACTIONS
// ***************************************

//! Enemy velocity set to left
void WalkLeft(Enemy& me) {
	me.velocity += Vector2(-1, 0);
}

//! Enemy velocity set to right
void WalkRight(Enemy& me){
	me.velocity += Vector2(1,0);
}

//! Add velocity to be towards target
//! Note that it's the velocity, so it does not need to be called all the time for the enemy to move...
void MoveToTarget(Enemy& me) {
	if (me.target == false) {
		me.velocity = Vector2();
		return;
	}
	Vector2 direction{ (me.target.GetPosition() - me.prevPos)};
	me.velocity += direction;
	me.velocity = me.velocity.Normalized();
} 
//! The target's speed is set to be 20% of what it is..
void ApplySlowToTarget(Enemy& me) {
	if (me.target == false) return;
	if (me.state == ES_HAPPY && me.target.isPlayer) return;


	me.target.GetSpeedMod() = 0.2f;

}

//! Enemy will pick a random direction and then set its velocity there
//! It will move in the velocity until the wander time is up, and then change direction again.
void Wander(Enemy& me) {
	if (me.wanderTimer <= EPSILON) {
		me.wanderTimer = me.type.wanderTime;
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

//! Moves in a circle around the target.
void CircleMove(Enemy& me) {

	if (!me.target) return;

	Vector2 direction{ (me.target.GetPosition() - me.prevPos)};
	direction = Vector2{direction.y, -direction.x}; //the perpendicular
	if (me.acknowledgeCollision) {
		direction = -direction;
	}
	me.velocity = direction;
}

//! Targets an enemy in detection radius.
//! Well, targets the boss regardless of range.
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

//! Targets the player straight
//! Very useful because the player is the center of the universe
void TargetPlayer(Enemy& me) {

	if (me.roomData->player)
		me.target = *me.roomData->player;
}

//! Sets velocity to be away from the player, but only when i'm within the safe radius.
void SafeDistancePlayer(Enemy& me) {
	Vector2 playerPos = me.roomData->player->position;
	if (AreCirclesIntersecting(me.prevPos, me.sprite.scale.x, playerPos, me.type.safeRadius) ) {

		Vector2 direction{ (playerPos - me.prevPos) };
		direction = -direction;
		me.velocity = direction;
	}
}
//! Targets a random enemy, choosing the boss last.
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

//! Seriously targets only the nearest entity that isn't itself.
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
		if (!(enemy && enemy->isActive) || enemy == &me) continue;
		smol =  std::min(enemy->prevPos.DistanceSq(me.prevPos), smol);
		if (smol != tmp) {
			tempTarget = *me.roomData->boss;
			tmp = smol;
		}
	}
	me.target = tempTarget;
}

//! Chooses the middle of the room as target
void TargetMiddle(Enemy& me) {

	me.target = Vector2();
}

//! Chooses an arbitrary corner of the map as a target
//! Sorry it's really arbitrary 
void TargetCorner(Enemy& me) {

	me.target = Vector2(100, 100);
}

//! Fires a projectile according to the specs of the enemy type
void FireProjectile(Enemy& me) {
	FireSomething(me, ShootProjectile);
}

//! Fires in an AOE pattern
void FireAOE(Enemy& me) {
	if (!me.target)
		return;

	if (me.attackTimer <= EPSILON)
	{

		std::pair<EnemyType::ProjectileInfo, bool> answer = AssessProjectileType(me);
		EnemyType::ProjectileInfo proj = answer.first;
		bool amIFriendsWithThePlayer = answer.second;
		Vector2 direction = me.prevPos - (me.target.GetPosition());

		ShootAOE(DataLoader::CreateTexture(proj.spritePath), *me.roomData, me.prevPos, proj.speed, proj.lifetime, static_cast<int>(me.dmgModifier * proj.damage), Vector2(proj.radius, proj.radius), proj.color, &me, amIFriendsWithThePlayer);
		me.attackTimer = me.type.attackRate;
		me.onceAttackTime = true;
	}
}

//! Fires in a boomerang shape
void FireBoomerang(Enemy& me) {
	FireSomething(me, ShootBoomerang);
}

//! Fires in a scattershot
void FireScatter(Enemy& me) {
	FireSomething(me, ShootScatter);
}

//! Fires in a spiral shape
void FireSpirally(Enemy& me) {
	if (!me.target)
		return;

	if (me.attackTimer <= EPSILON)
	{
		std::pair<EnemyType::ProjectileInfo, bool> answer = AssessProjectileType(me);
		EnemyType::ProjectileInfo proj = answer.first;
		bool amIFriendsWithThePlayer = answer.second;
		Vector2 direction = me.prevPos - (me.target.GetPosition());
		ShootRounding(DataLoader::CreateTexture(proj.spritePath), *me.roomData, me.prevPos, -direction.Normalized(), proj.speed, proj.lifetime, static_cast<int>(me.dmgModifier * proj.damage), Vector2(proj.radius, proj.radius), proj.color, &me, amIFriendsWithThePlayer);
		me.attackTimer = me.type.attackRate;
		me.onceAttackTime = true;
	}
}
//! Moves like the DVD Logo, bumping into walls and changing velocity
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
// bounce against entities
void DVDBounce(Enemy& me) {

	if (!me.target) return; //there's no reason to bounce without a target to bounce off of.
	

	//i don't really feel like doing much square related math

	me.velocity = ( me.prevPos - me.target.GetPosition());

}

//! Become angry
//! You should not ever call this in happy behaviour, for design reasons.
void BecomeAngry(Enemy& me) {
	me.ChangeState(ES_ANGRY);
}

//! Become neutral
//! You should not ever call this in happy behaviour, for design reasons.
void BecomeNeutral(Enemy& me) {
	me.ChangeState(ES_NEUTRAL);
}

//! Damage the target directly.
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

//! Changes the speed modifier to negative
//! Needs to be called again to un-invert the velocity.
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

//! Targets the self
//! Useful for ApplySlow for changing speed, or for moving in place.
void TargetSelf(Enemy& me) {
	me.target = me; //wow!
}

//! Contrary to popular belief, does not actually cause the enemy to wait
//! It simply starts the wait timer, for the "IsWaiting" or "WaitTimerUp" contexts to catch.
void Wait(Enemy& me) {
	
	if (me.waitTimer <= EPSILON) {
		me.waitTimer = me.type.waitTime;
		me.onceWaitTime = true;
	}
}

//! Moves to the target's initial position. If it reaches there, clears the target
//! Still pretty hard to use, there might be better charging algorithms you can use with a combination of other actions.
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

//! Removes the target, allowing for targetless behaviour
void ClearTarget(Enemy& me) {
	//Set target to be empty
	me.target = Enemy::Target{};
}

//! Sets velocity to zero
void StopMoving(Enemy& me) {
	//Set target to be empty
	me.velocity = Vector2();
}

//unused template functions
bool DefaultFlag(Enemy& me){
	return me.isActive;
}

//Not a good function, prevents errors from occuring
void DefaultAction(Enemy& me) {
	std::cerr << me.type.name << " has called a default action which should not happen ever!\n";
	std::cerr << "Did you misspell an action?\n";
	return;
}


}//end functions namspace

static CommandList commands;
static FlagList flags;

//! Gets command for DataLoader
Command GetCommand(std::string name) {
	if (commands.find(name) == commands.end()) {
		std::cerr << "ERROR! " << name << " didn't load properly! Likely misspelling.\n";
		return commands["default"];
	}
    return commands[name];
}

//! Gets flag for DataLoader
FlagCheck GetFlag(std::string name) {
	if (flags.find(name) == flags.end()) {
		return flags["default"];
	}
    return flags[name];
}

//! Sets each function to have an associated string that matches its function name
//! This is specially for the JSON to be able to read
void InitCommands() {
    //just a guess as to how many commands we are expecting
    commands.reserve(35);
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
    

} //end anonymous namespace for reasons
