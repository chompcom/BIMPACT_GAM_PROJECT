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

using FSM = std::vector<std::pair<FlagCheck, BundledBehaviour>>;

struct RoomData;

class Enemy {
	public:
		const EnemyType& type;
		TexturedSprite sprite;
		TexturedSprite shadow;
		f32 currentHealth;
		EnemyStates state;

		bool isActive;

		float speedModifier;
		float dmgModifier;

		Vector2 velocity;
		Vector2 prevPos;

		float wanderTimer;
		float waitTimer;
		float attackTimer;

		//Target contains information about the target so you can do things to it
		struct Target {
			Vector2 initialPosition; //!< The position when the target was found

			//Well there's only players and enemies as entities you see..
			bool isPlayer;



			Target();
			~Target();

			//So i can just set the target to the guy
			Target& operator=(Enemy& them);
			Target& operator=(Player& them);
			Target& operator=(Boss& them);
			//Assign to create a "dumb target" that doesn't move and is always alive
			Target& operator=(Vector2 const& position);

			//Conversion to bool to check if Target is even alive.
			operator bool() const;
			bool GetActive() const;

			Vector2 GetPosition() const;
			float& GetSpeedMod();
			float GetSpeedMod() const;
			float& GetDmgMod();
			float GetDmgMod() const;
			void DealDamage(float dmg);

			private: //We keep these private because I don't want to accidentally change them.
				//Sometimes the target is already dead. We don't care about them
				Vector2* position; //!< Points to target location
				float* speedMod;
				float* dmgMod;
				float* health;
				bool *isActive;
				//Any extra info will be stored here
				float info{};
		} target;

		//points to the room it should be inside, so that it knows whats going on inside!
		RoomData* roomData;

		FSM currentBehavior;

		Enemy(const EnemyType& enemyType, TexturedSprite enemySprite, TexturedSprite shadowSprite, EnemyStates initialState = EnemyStates::ES_NEUTRAL);
		~Enemy();

		void Update(float dt);
		void AssessTraits(Labels labelsToCheck, bool giftCheck = true);
		void ChangeState(EnemyStates state);
};


class EnemyType {
public:
	std::string name;
	std::string spritePath;
	f32 health;
	f32 damage;
	f32 speed;
	float attackRate;
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

	struct ProjectileInfo {
		float damage;
		float radius;
		float speed;
		float lifetime;
		std::string spritePath;

		ProjectileInfo() : damage{}, radius{}, speed{}, lifetime{}, spritePath{} {}
	}  happyProjectile, angryProjectile, neutralProjectile;


	EnemyType(std::string name, f32 health, f32 damage, const Labels& traits, const Labels& likes, const Labels& dislikes);

	void AddNeutral(FlagCheck const& checker, BundledBehaviour const& behaviours);

	void AddAngry(FlagCheck const& checker, BundledBehaviour const& behaviours);

	void AddHappy(FlagCheck const&checker, BundledBehaviour const&behaviours);

};



