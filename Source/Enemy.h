/* Start Header ************************************************************************/
/*!
\file		Enemy.cpp
\author 	Hong Josiah Qin, hong.j, 2501239
\par  		hong.j@digipen.edu
\brief		Contains the Enemy and EnemyType.
EnemyType is a data class that defines what the enemy can do, and its base values
Enemy is defined such that it takes in an enemy type and behaves as the type does

Copyright (C) 2026 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited.
*/
/* End Header **************************************************************************/

#pragma once
#include "AEEngine.h"
#include "Sprite.h"
#include <string>
#include "Traits.h"
#include "RoomData.h"
#include <map>
#include "Player.h"
#include "Boss.h"


enum EnemyStates {
	ES_HAPPY,
	ES_NEUTRAL,
	ES_ANGRY
};


class EnemyType;
class Enemy;
class Boss;
// forward declaration of room
typedef void Behaviour(Enemy&);
typedef Behaviour* Command;

//it's for checking flags and stuff
//i don't think i need delta time? i really doubt it.
typedef bool FlagCheckItself(Enemy&);
typedef FlagCheckItself* FlagCheck;

using BundledBehaviour = std::vector<Command>;


//! an FSM is a vector consisting of FlagCheck and BundledBehaviour
//! BundledBehaviour is the set of actions that the enemy will perform
//! FlagCheck is a function that looks like "bool func(Enemy&)" that returns true or false depending on the context assigned to it
//! The basis of the FSM is that if the FlagCheck function returns true, the enemy will then perform the actions in BundledBehaviour in order.
using FSM = std::vector<std::pair<FlagCheck, BundledBehaviour>>;

struct RoomData;

//! Enemy class that does the brunt of the hard work.
//! It takes an EnemyType and behaves according to the behaviours of the type.
//! It will be Updated in the Scene and an active member of the game.
class Enemy {
	public:
		const EnemyType& type; //!< The blueprint for how the enemy behaves
		AnimatedSprite sprite; //!< What it looks like
		TexturedSprite shadow; //!< The shadow that follows it always
		TexturedSprite hitbox; //!< Hitbox of the sprite for debug mode
		f32 currentHealth;	   //!< Health as it is. Starts off with enemyType's health.
		EnemyStates state;	   //!< current state, whether it is Happy, Angry or Neutral. Happy means its a friend.
		bool isActive;		   //!< Checks for active. Non-active will be not updated or rendered.

		float speedModifier;   //!< Modifier of speed. Will always reset to 1 in Update. Keeps its sign value.
		float dmgModifier;	   //!< Damage Modifier if I would ever use it.

		Vector2 velocity;	   //!< Current velocity of the enemy. It should always be normalized actually so it's not actually the velocity.
		Vector2 prevPos;	   //!< Previous position 

		float wanderTimer;	   //!< The time to wander that ticks down
		bool onceWanderTime;   //!< bool that turns true when wanderTime is set to active. Turns false when called with "OnceWanderTimeUp"
		float waitTimer;	   //!< The time to wait that ticks down
		bool onceWaitTime;	   //!< bool that turns true when waitTime is set to active. Turns false when called with "OnceWaitTimeUp"
		float attackTimer;	   //!< The time for the attack to tick down. Is effectively the attackRate.
		bool onceAttackTime;   //!< bool that turns true when attackTimer is set to active. Turns false when called with "OnceAttackCooldownUp"

		//Target contains information about the target so you can do things to it
		//Object itself returns whether it is active or not
		struct Target {
			Vector2 initialPosition; //!< The position when the target was found

			//Well there's only players and enemies (+boss) as entities you see..
			bool isPlayer; //!< Helpful bool to check if target is player



			Target();
			~Target();

			//! Operator overloads that allow for easy setting of target
			//! Sets target to another enemy
			Target& operator=(Enemy& them);
			//! Operator overloads that allow for easy setting of target
			//! Sets target to the player
			Target& operator=(Player& them);
			//! Operator overloads that allow for easy setting of target
			//! Sets target to the boss
			Target& operator=(Boss& them);
			//! Operator overloads that allow for easy setting of target
			//! Assign to create a "dumb target" that doesn't move and is always alive
			Target& operator=(Vector2 const& position);

			//! Conversion to bool to check if Target is even alive.
			operator bool() const;
			//! Gets the active state of the target
			bool GetActive() const;

			//! Gets the position of the target
			Vector2 GetPosition() const;
			//! Gets the speed mod and allows for altering of the speed
			float& GetSpeedMod();
			//! Retrieves the speed modifier
			float GetSpeedMod() const;
			//! Gets the damage modifier and allows for altering of the damage
			float& GetDmgMod();
			//! Retrieves the damage modifier
			float GetDmgMod() const;
			//! Deals damage directly to target health
			void DealDamage(float dmg);

			private: //We keep these private because I don't want to accidentally change them.
				//Sometimes the target is already dead. We don't care about them

				//These variables are pointers because we want to keep track of their data as they change.
				//And even alter them as required

				Vector2* position; //!< Points to target location
				float* speedMod;   //!< Points to the speedMod of the target.
				float* dmgMod;	   //!< Points to the dmgMod of the target.
				float* health;	   //!< Points to the health of the target.
				bool *isActive;    //!< Points to the active state of target. Important for knowing if the target is alive.
				//Any extra info will be stored here
				float info{};      //!< Stores any extra info (not likely to be used)
		} target;

		//! points to the room it should be inside, so that it knows whats going on inside!
		//! As dangerous as it may be, we can also alter the state of the elements in the room.
		RoomData* roomData;

		
		FSM currentBehavior;         //!< The current set of behaviours the enemy will go through. Is set by the mood of the enemy.
		int collisionResolution;	//!< Contains the state of collision it has with the static environment. 
		bool acknowledgeCollision; //!< Used for "OnEvenBounce" and also for letting the enemy switch direction on circlemove.

		Enemy(const EnemyType& enemyType, AnimatedSprite enemySprite, TexturedSprite shadowSprite, TexturedSprite hitboxSprite, EnemyStates initialState = EnemyStates::ES_NEUTRAL);
		~Enemy();

		//! \brief Updates the enemy behaviour 
		//! In great detail: 
		//! 1. The enemy will go through its FSM
		//! 2. The enemy will move according to its velocity
		//! 3. All its timers will tick down by dt
		//! 4. Collision resolution with static environment occurs
		//! 5. Its speed Modifier resets.
		//! \param dt The deltaTime that the game is running in.
		void Update(float dt);

		//! \brief Checks through the labels, and determines if it should be happy or angry about it.
		//! \param labelsToCheck The traits that the enemy will check against its type's likes and dislikes list.
		//! \param giftCheck This check is for whether it is for gifts. Set to false for enemy to enemy trait checks. (Or more accurately, assessment that doesn't go to happy)
		void AssessTraits(Labels labelsToCheck, bool giftCheck = true);

		//! \brief Change state to ES_HAPPY, ES_NEUTRAL or ES_ANGRY
		//! Used for hard state changes
		void ChangeState(EnemyStates state);
};

/*! 
\brief Keeps the enemy's data such as its name, health and damage.
Contains the enemy's behaviour as well.
*/
class EnemyType {
public:
	std::string name;			//<! the name of the enemy
	std::string spritePath;		//<! the path of the sprite that the enemy looks like
	f32 health;					//<! the base health of the enemy
	f32 damage;					//<! base damage of enemy
	f32 speed;					//<! base speed of enemy
	//! rate at which attacks are done (seconds to wait)
	float attackRate;
	//! seconds you are wandering in one direction
	float wanderTime;
	//! seconds you are waiting for
	float waitTime;
	//! path to the sound the enemy makes when angry
	std::string angrySound;
	//! Radius used for "TargetInDetectionRadius"
	f32 detectionRadius;
	//! Radius used for "FollowingPlayer" or anything related to friendly range
	f32 safeRadius;
	//! Traits that the enemy has
	Labels traits;
	//! What the enemy likes
	Labels likes;
	//! What they don't like
	Labels dislikes;

	
	FSM happy;	   //!< The Finite State Machine for the enemy's happy state
	FSM angry;	   //!< The Finite State Machine for the enemy's angry state
	FSM neutral;   //!< The Finite State Machine for the enemy's neutral state

	//! Contains information about the projectiles that the enemy shoots for happy, angry and neutral
	struct ProjectileInfo {
		float damage; //!< damage of projectile
		float radius; //!< size of projectile
		float speed;  //!< speed at which it travels
		float lifetime; //!< how long it is active for
		Color color;  //!< What color is it, and its particles
		std::string spritePath;  //!< the path of its sprite

		//! default constructor
		ProjectileInfo() : damage{}, radius{}, speed{}, lifetime{}, spritePath{}, color{ 1.f,1.f,1.f,1.f } {}
	}  happyProjectile, angryProjectile, neutralProjectile;

	//! Constructor for the enemy type.
	//! Only useful for testing
	EnemyType(std::string name, f32 health, f32 damage, const Labels& traits, const Labels& likes, const Labels& dislikes);

	//! Adds a neutral action to its FSM
	void AddNeutral(FlagCheck const& checker, BundledBehaviour const& behaviours);

	//! Adds an angry action to its FSM
	void AddAngry(FlagCheck const& checker, BundledBehaviour const& behaviours);

	//! Adds a happy action to its FSM
	void AddHappy(FlagCheck const&checker, BundledBehaviour const&behaviours);

};



