#include "AEEngine.h"
#include "Boss.h"

Boss::Boss(std::string enemyName, f32 enemyHealth, f32 enemyDamage, TexturedSprite enemySprite, const RoomData& currentRoom, std::vector<AttackData> attackData)
	: name{ enemyName }, health{ enemyHealth }, damage{ enemyDamage }, currentHealth{ enemyHealth }, sprite{ enemySprite },
	roomData{ currentRoom }, bossStateMachine{ std::make_unique<Boss_FSM>(this) }
{
	if (enemyName == "Boss 1") {
		std::cout << "true\n";
		bossStateMachine = std::make_unique<Boss1_FSM>(this, attackData[0].attackDamage, attackData[0].attackStartup, attackData[0].attackInterval, attackData[0].attackEndlag,
			attackData[1].attackDamage, attackData[1].attackStartup, attackData[1].attackInterval, attackData[1].attackEndlag);
		std::cout << typeid(bossStateMachine).name() << '\n';
	}
}

Boss::~Boss() {}


void Boss::Update(Player& player, f32 dt) {
	bossStateMachine->Update(player, dt);
}

