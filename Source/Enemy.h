#pragma once
#include "AEEngine.h"
#include "Sprite.h"
#include <string>
#include "Traits.h"
#include "RoomData.h"
#include <map>
#include "Player.h"

enum EnemyStates {
	ES_HAPPY,
	ES_NEUTRAL,
	ES_ANGRY
};


class EnemyType;
class Enemy;
// forward declaration of room
typedef void Behaviour(Enemy&,  float dt);
typedef Behaviour* Command;

//it's for checking flags and stuff
//i don't think i need delta time? i really doubt it.
typedef bool FlagCheckItself(Enemy&);
typedef FlagCheckItself* FlagCheck;

using BundledBehaviour = std::vector<Command>;

using FSM = std::vector<std::pair<FlagCheck, BundledBehaviour>>;

struct RoomData;

class Enemy {
	public:
		const EnemyType& type;
		TexturedSprite sprite;
		TexturedSprite shadow;
		f32 currentHealth;
		EnemyStates state;

		float speedModifier;
		float dmgModifier;

		Vector2 velocity;

		float wanderTimer;
		float attackTimer;

		//Target contains information about the target so you can do things to it
		struct Target {
			Vector2* position; //!< Points to target location
			Vector2 initialPosition; //!< The position when the target was found

			//Well there's only players and enemies as entities you see..
			bool isPlayer;

			//Sometimes the target is already dead. We don't care about them.
			bool isActive;

			float* speedMod;
			float* dmgMod;

			Target();
			~Target();

			//So i can just set the target to the guy
			Target& operator=(Enemy& them);
			Target& operator=(Player& them);


		} target;

		//points to the room it should be inside, so that it knows whats going on inside!
		RoomData* roomData;

		FSM currentBehavior;

		Enemy(const EnemyType& enemyType, TexturedSprite enemySprite, TexturedSprite shadowSprite, EnemyStates initialState = EnemyStates::ES_NEUTRAL);
		~Enemy();

		void Update(float dt);
		void AssessTraits(Labels labelsToCheck);
		void ChangeState(EnemyStates state);
};


class EnemyType {
public:
	std::string name;
	f32 health;
	f32 damage;
	f32 speed;
	
	//! Radius used for "Target___InDetectionRadius"
	f32 detectionRadius;

	//! Radius used for "FollowingPlayer" or anything related to friendly range
	f32 safeRadius;

	//! 


	Labels traits;
	Labels likes;
	Labels dislikes;

	FSM happy;
	FSM angry;
	FSM neutral;



	EnemyType(std::string name, f32 health, f32 damage, const Labels& traits, const Labels& likes, const Labels& dislikes);

	void AddNeutral(FlagCheck const& checker, BundledBehaviour const& behaviours);

	void AddAngry(FlagCheck const& checker, BundledBehaviour const& behaviours);

	void AddHappy(FlagCheck const&checker, BundledBehaviour const&behaviours);

};



