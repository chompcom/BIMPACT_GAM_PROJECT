#pragma once
#include "AEEngine.h"
#include "Sprite.h"
#include <string>

#include <set>

enum EnemyStates {
	ES_HAPPY,
	ES_NEUTRAL,
	ES_ANGRY
};

class EnemyType;
class Enemy;
typedef void Behaviour(Enemy&, float dt);
typedef Behaviour* Command;

class Enemy {
	public:
		const EnemyType& type;
		TexturedSprite sprite;
		f32 currentHealth;
		EnemyStates state;

		Command currentBehavior;

		Enemy(const EnemyType& enemyType, TexturedSprite enemySprite, EnemyStates initialState);
};

using Labels = std::set<std::string>;
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