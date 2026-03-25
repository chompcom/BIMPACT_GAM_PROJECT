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

using CommandList = std::unordered_map<std::string, Command>;

using CommandPair = std::pair<std::string, Command>;

using FlagList = std::unordered_map<std::string, FlagCheck>;


// ******************************

// HELPER FUNCTIONS

// ******************************
namespace {
	Vector2 PathFind(Vector2 const& pos, Vector2 const& end, Grid const& grid) {
		//BFS I suppose

		//snap to cell
		
		UNREFERENCED_PARAMETER(pos);
		UNREFERENCED_PARAMETER(grid);

		return Vector2{};
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
	if (CollisionIntersection_RectRect(me.sprite.position,me.sprite.scale * 0.5f, 
		me.velocity, 
			me.target.GetPosition(), me.sprite.scale * 0.5f,
			me.target.GetPosition() - me.target.initialPosition
			, collTime) ) {
				return true;
	}
		
    
    return false;
}

bool IsNotFollowingPlayer(Enemy& me) {

	if (AreCirclesIntersecting(me.sprite.position, me.type.safeRadius,
				me.roomData->player->sprite.position, 0) ) {

		return false;

	}
	else {
		return true;
	}
}

bool IsTargetInDetectionRadius(Enemy& me) {
	if (!me.target) return false;

	return (AreCirclesIntersecting(me.sprite.position, me.type.detectionRadius,
		me.target.GetPosition(), 0));
}


// ***************************************
//               ACTIONS
// ***************************************

void WalkLeft(Enemy& me) {
	me.velocity += Vector2(-50, 0);
	if (CollisionBoundary_Static(me.sprite.position, me.sprite.scale, 1600, 900))
	me.sprite.UpdateTransform();
}

void WalkRight(Enemy& me){
	me.sprite.position += Vector2(50,0);
	me.sprite.color = Color{ 1.0f,0.0f,0.0f,1.0f };
	me.sprite.UpdateTransform();
}

void MoveToTarget(Enemy& me) {
	if (me.target == false) return;
	Vector2 direction{ (me.target.GetPosition() - me.sprite.position)};
	me.velocity += direction;
	me.velocity = me.velocity.Normalized();
	CollisionBoundary_Static(me.sprite.position, me.sprite.scale, 1600, 900);
	me.sprite.UpdateTransform();
} 

void ApplySlowToTarget(Enemy& me) {
	if (me.target == false) return;
	if (me.state == ES_HAPPY && me.target.isPlayer) return;

	me.target.GetSpeedMod() = 0.1f;

}

void Wander(Enemy& me) {
	if (me.wanderTimer <= 0.f) {
		me.wanderTimer = 3.f;
		me.velocity = Vector2{};
	}
	if (me.velocity == Vector2{}) {
		float randx = AERandFloat() * 2 - 1;
		float randy = AERandFloat() * 2 - 1;
		me.velocity = Vector2{randx, randy};
	}
	if (CollisionBoundary_Static(me.sprite.position, me.sprite.scale, 1600, 900))
		me.velocity = -me.velocity;

	
}

void CircleMove(Enemy& me) {

	if (!me.target) return;

	Vector2 direction{ (me.target.GetPosition() - me.sprite.position)};
	direction = Vector2{direction.y, -direction.x}; //the perpendicular

	me.velocity = direction;

	CollisionBoundary_Static(me.sprite.position, me.sprite.scale, 1600, 900);
	me.sprite.UpdateTransform();


}

void TargetEnemyInDetectionRadius(Enemy& me){
	for (Enemy* guy : me.roomData->enemyList){
		if (!guy->isActive) continue;
		if (AreCirclesIntersecting(me.sprite.position, me.type.detectionRadius,
			guy->sprite.position, guy->sprite.scale.x)) {
			std::cout << "Found " << guy->type.name << std::endl;

				me.target = *guy;
				return; //found a guy
			}
	}
	//If the boss exists.. perhaps target them
	if (me.roomData->boss) me.target = *me.roomData->boss;
	//if i didn't find anything, DON'T retarget...

}

void TargetPlayer(Enemy& me) {

	if (me.roomData->player)
		me.target = *me.roomData->player;
}

void SafeDistancePlayer(Enemy& me) {
	Vector2 playerPos = me.roomData->player->sprite.position;
	if (AreCirclesIntersecting(me.sprite.position, me.sprite.scale.x, playerPos, me.type.safeRadius) ) {

		Vector2 direction{ (playerPos - me.sprite.position) };
		direction = -direction;
		me.velocity = direction;
	}
}
void TargetRandomEnemy(Enemy& me) {
	
	std::vector<Enemy*> aliveList{};
	//check if all dead
	for (Enemy* me : me.roomData->enemyList) {
		if (me->isActive) aliveList.push_back(me);
	}
	//Very special case, this behaviour specifically targets enemies first! 
	//Once the enemies are gone then the boss is targeted!
	if (aliveList.empty()) {
		if (me.roomData->boss) me.target = *me.roomData->boss;
		return;
	}


	do {
		me.target = *me.roomData->enemyList[
			std::rand() % aliveList.size()];
	} while (!me.target.GetActive());
}
void TargetMiddle(Enemy& me) {
	me.target.initialPosition = Vector2();
}
void TargetCorner(Enemy& me) {

	me.target.initialPosition = Vector2(100,100);
}
void FireProjectile(Enemy& me) {

	if (!me.target) return;
	if (me.attackTimer <= 0) {
		EnemyType::ProjectileInfo proj;
		switch (me.state) {
		case ES_ANGRY:
			proj = me.type.angryProjectile;
			break;
		case ES_HAPPY:
			proj = me.type.happyProjectile;
			break;
		default:
			proj = me.type.neutralProjectile;
			break;
		}

		Vector2 direction = me.sprite.position - (me.target.GetPosition());

		bool amIFriendsWithThePlayer = me.state == ES_HAPPY;

		ShootProjectile(DataLoader::CreateTexture(proj.spritePath), *me.roomData, me.sprite.position, -direction.Normalized(), proj.speed, proj.lifetime, me.dmgModifier * proj.damage, Vector2(proj.radius,proj.radius), {1.f,1.f,1.f,1.f}, &me, amIFriendsWithThePlayer);
		me.attackTimer = 3;
	}
}
void DVDMove(Enemy& me) {
	//UNREFERENCED_PARAMETER(me);
}
void DVDBounce(Enemy& me) {

}
void BecomeAngry(Enemy& me) {
	me.ChangeState(ES_ANGRY);
}
void BecomeNeutral(Enemy& me) {

	me.ChangeState(ES_NEUTRAL);
}
void DamageTarget(Enemy& me) {
//now this gets tricky!!
//hard code for now!!
	if (me.attackTimer <= 0) {
		me.attackTimer = 3;
		
		if (me.target.isPlayer) {
			playerTakesDamage(*me.roomData->player);
		}

	}

}

void PullTarget(Enemy& me) {

}
void PushTarget(Enemy& me) {

}

//unused template functions
bool DefaultFlag(Enemy& me){
	return true;
}

void DefaultAction(Enemy& me) {

}


}//end functions namspace

static CommandList commands;
static FlagList flags;
Command GetCommand(std::string name) {
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
		{"TargetPlayer",TargetPlayer},
		{"SafeDistancePlayer",SafeDistancePlayer},
		{"TargetRandomEnemy",TargetRandomEnemy},
		{"TargetMiddle", TargetMiddle},
		{"TargetCorner", TargetCorner},
		{"FireProjectile", FireProjectile},
		{"DVDMove", DVDMove},
		{"DVDBounce", DVDBounce},
		{"BecomeAngry", BecomeAngry},
		{"BecomeNeutral", BecomeNeutral},
		{"DamageTarget", DamageTarget},
		{"PullTarget", PullTarget},
		{"PushTarget", PushTarget}

    };
}


//anonymous namespace for reasons
namespace {
    

    
   
    

} //end anonymous namespace
