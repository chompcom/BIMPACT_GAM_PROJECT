#include "Behaviours.h"

#include <string>

#include <unordered_map>
#include "Enemy.h"
#include "BoundaryCollision.h"
#include "Collision.h"
#include "Utils/Utils.h"

#include <iostream>

using CommandList = std::unordered_map<std::string, Command>;

using CommandPair = std::pair<std::string, Command>;

using FlagList = std::unordered_map<std::string, FlagCheck>;


//list of all the various functions
namespace { //functions namespace begin
    
bool IsTouchingTarget(Enemy& me) {

	if (!me.target.isActive) return false;

	float collTime;
	if (CollisionIntersection_RectRect(me.sprite.position,me.sprite.scale, Vector2{}, 
			*me.target.position, me.sprite.scale, Vector2{}, collTime) ) {
				std::cout << "touching!!" << std::endl;
				return true;
	}
		
    
    return false;
}

bool IsNotFollowingPlayer(Enemy& me) {
	if (!me.target.isActive) return false;

	if (AreCirclesIntersecting(me.sprite.position, me.type.safeRadius,
				me.roomData->player->sprite.position, 0) ) {

		std::cout <<"in player radius" << std::endl;
		return false;

	}
	else {
		return true;
	}
}

void WalkLeft(Enemy& me, float dt) {
	me.sprite.position += Vector2(-50, 0) * dt;
	if (CollisionBoundary_Static(me.sprite.position, me.sprite.scale, 1600, 900))
	me.sprite.UpdateTransform();
}

void WalkRight(Enemy& me, float dt){
	me.sprite.position += Vector2(50,0) * dt;
	me.sprite.color = Color{ 1.0f,0.0f,0.0f,1.0f };
	me.sprite.UpdateTransform();
}

void MoveToTarget(Enemy& me, float dt) {
	Vector2 direction{ (*me.target.position - me.sprite.position) };
	me.sprite.position += direction.Normalized() * me.type.speed * dt;
	CollisionBoundary_Static(me.sprite.position, me.sprite.scale, 1600, 900);
	me.sprite.UpdateTransform();
} 

void ApplySlowToTarget(Enemy& me, float dt) {
	std::cout << me.type.name << " Applying slow!" << std::endl;
}

void Wander(Enemy& me, float dt) {

}

void CircleMove(Enemy& me, float dt) {
	Vector2 direction{ (*me.target.position - me.sprite.position) };
	direction = Vector2{direction.y, -direction.x}; //the perpendicular

	me.sprite.position += direction.Normalized() * me.type.speed * dt;
	CollisionBoundary_Static(me.sprite.position, me.sprite.scale, 1600, 900);
	me.sprite.UpdateTransform();

}

void TargetEnemyInDetectionRadius(Enemy& me, float dt){
	for (Enemy* guy : me.roomData->enemyList){
		if (AreCirclesIntersecting(me.sprite.position, me.type.detectionRadius,
			guy->sprite.position, guy->sprite.scale.x)) {
				me.target = *guy;
				std::cout << me.type.name << " is targetting " << guy->type.name << std::endl;
				return; //found a guy
			}
	}
	//if i didn't find anything, DON'T retarget...

}

void TargetPlayer(Enemy& me, float dt) {

	if (me.roomData->player)
		me.target = *me.roomData->player;
}


//unused template functions
bool DefaultFlag(Enemy& me){
	return true;
}

void DefaultAction(Enemy& me, float dt) {

}


}//end functions namspace

static CommandList commands;
static FlagList flags;
Command GetCommand(std::string name) {
    return commands[name];
}

FlagCheck GetFlag(std::string name) {
    return flags[name];
}

void InitCommands() {
    //just a guess as to how many commands we are expecting
    commands.reserve(30);
    flags.reserve(30);

    flags = {
        {"IsTouchingTarget",IsTouchingTarget},
		{"IsNotFollowingPlayer", IsNotFollowingPlayer},
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
		{"TargetPlayer",TargetPlayer}
    };
}


//anonymous namespace for reasons
namespace {
    

    
   
    

} //end anonymous namespace
