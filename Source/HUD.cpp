#include "AEEngine.h"
#include "Player.h"
#include "Sprite.h"
#include <iostream>

void renderPlayerLives(Player const & player, TexturedSprite healthIcons[3], s8 font)
{
	AEGfxPrint(font, "Lives:", -0.95f, -0.8f, 1.f, 0.f, 0.f, 0.f, 1.f);
	if (player.health <= 0) return;
	for (int i{ 0 }; i < player.health; ++i)
	{
		if (i < player.health) healthIcons[i].RenderSprite();
	}
}
