#include "AEEngine.h"
#include "Player.h"
#include "Sprite.h"
#include "Almanac.h"
#include "Utils/Utils.h"
#include <iostream>
#include <vector>

void renderPlayerLives(Player const & player, std::vector<TexturedSprite> healthIcons, s8 font)
{
	AEGfxSetRenderMode(AE_GFX_RM_TEXTURE);

	AEGfxPrint(font, "Lives:", -0.95f, -0.8f, 1.f, 1.f, 1.f, 1.f, 1.f);
	if (player.health <= 0) return;
	for (int i{ 0 }; i < player.health; ++i)
	{
		TexturedSprite heart = healthIcons[0];
		heart.position.x += i * 100;
		heart.UpdateTransform();
		heart.RenderSprite();
		//healthIcons[i].UpdateTransform();
		//healthIcons[i].RenderSprite();
	}
}

void checkIfAlmanacClicked(TexturedSprite const& almanacSprite, Almanac & almanac)
{
	s32 cursorPositionX, cursorPositionY;
	AEInputGetCursorPosition(&cursorPositionX, &cursorPositionY);

	if (AEInputCheckReleased(AEVK_LBUTTON) && IsCursorInSquare(almanacSprite.position, almanacSprite.scale.x))
	{
		//std::cout << "click!\n";
		almanac.isOpen = true;
	}
}
