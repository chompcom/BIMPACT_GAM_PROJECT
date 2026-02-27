#include "AEEngine.h"
#include "Player.h"
#include "Sprite.h"
#include <iostream>
#include <vector>

void renderPlayerLives(Player const & player, std::vector<TexturedSprite> healthIcons, s8 font)
{
	AEGfxSetRenderMode(AE_GFX_RM_TEXTURE);

	AEGfxPrint(font, "Lives:", -0.95f, -0.8f, 1.f, 1.f, 1.f, 1.f, 1.f);
	if (player.health <= 0) return;
	for (int i{ 0 }; i < player.health; ++i)
	{
		healthIcons[i].UpdateTransform();
		healthIcons[i].RenderSprite();
	}
}
