#pragma once
#include "AEEngine.h"
#include "Sprite.h"
#include <string>
#include <array>

#define MAX_NO_TRAITS (10)

/*class Enemy {
	public:
		std::string name;
		Sprite sprite;
		int health;

		std::string[10] traits;
		std::string[10] likes;
		std::string[10] dislikes;

		int state;
		void (*neutral)(void);
		void (*happy)(void);
		void (*angry)(void);

		Enemy
};*/

enum EnemyStates {
	ES_HAPPY,
	ES_NEUTRAL,
	ES_ANGRY
};

class EnemyType;

class Enemy {
	public:
		EnemyType& type;
		TexturedSprite sprite;
		f32 currentHealth;
		EnemyStates state;

		void (*currentBehavior)(void);

		Enemy(EnemyType& enemyType, TexturedSprite enemySprite, EnemyStates initialState);
};

class EnemyType {
	public:
		std::string name;
		f32 health;
		f32 damage;

		std::array<std::string,MAX_NO_TRAITS> traits;
		std::array<std::string,MAX_NO_TRAITS> likes;
		std::array<std::string,MAX_NO_TRAITS> dislikes;

		void (*neutral)(void);
		void (*happy)(void);
		void (*angry)(void);

		EnemyType(std::string name, f32 health, f32 damage, std::array<std::string, MAX_NO_TRAITS> traits, 
			std::array<std::string, MAX_NO_TRAITS> likes, std::array<std::string, MAX_NO_TRAITS> dislikes);
};
