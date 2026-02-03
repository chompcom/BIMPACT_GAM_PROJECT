#pragma once
#include "AEEngine.h"
#include "Sprite.h"
#include <string>
#include "Traits.h"

enum EnemyStates {
	ES_HAPPY,
	ES_NEUTRAL,
	ES_ANGRY
};

class EnemyType;
class Enemy;
// forward declaration of room
class Room;
typedef void Behaviour(Enemy&,  float dt);
typedef Behaviour* Command;

class Enemy {
	public:
		const EnemyType& type;
		TexturedSprite sprite;
		f32 currentHealth;
		EnemyStates state;
		Vector2 target;

		Command currentBehavior;

		Enemy(const EnemyType& enemyType, TexturedSprite enemySprite, EnemyStates initialState = EnemyStates::ES_NEUTRAL);

		void Update(float dt);
		void AssessTraits(Labels labelsToCheck);
		void ChangeState(EnemyStates state);
};


class EnemyType {
public:
	std::string name;
	f32 health;
	f32 damage;

	Labels traits;
	Labels likes;
	Labels dislikes;

	Command neutral;
	Command happy;
	Command angry;

	EnemyType(std::string name, f32 health, f32 damage, const Labels& traits,
		const Labels& likes, const Labels& dislikes);
};


void WalkLeft(Enemy& me, float dt);
void WalkRight(Enemy& me, float dt);
void WalkToTarget(Enemy& me, float dt);