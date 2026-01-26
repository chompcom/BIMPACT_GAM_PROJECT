#include "AEEngine.h"
#include "Enemy.h"

Enemy::Enemy(EnemyType& enemyType, TexturedSprite enemySprite, EnemyStates initialState)
	: type{ enemyType }, sprite{ enemySprite }, currentHealth {enemyType.health}, state{ initialState }, currentBehavior{ nullptr } {
		switch (initialState) {
			case ES_HAPPY:
				currentBehavior = enemyType.happy;
				break;
			case ES_NEUTRAL:
				currentBehavior = enemyType.neutral;
				break;
			case ES_ANGRY:
				currentBehavior = enemyType.angry;
				break;
		}
}

EnemyType::EnemyType(std::string name, f32 health, f32 damage, std::array<std::string, MAX_NO_TRAITS> traits,
	std::array<std::string, MAX_NO_TRAITS> likes, std::array<std::string, MAX_NO_TRAITS> dislikes)
	: name{ name }, health {health}, damage{ damage }, traits{ traits }, likes{ likes }, dislikes{ dislikes },
		neutral{ nullptr }, happy{ nullptr }, angry{ nullptr } {
}