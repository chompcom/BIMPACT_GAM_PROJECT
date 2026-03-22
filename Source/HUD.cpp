#include "AEEngine.h"
#include "Player.h"
#include "Sprite.h"
#include "Almanac.h"
#include "Utils/Utils.h"
#include <iostream>
#include <vector>

static bool arrowGoingUp = false; //to check if the arrow should move up or down

void renderPlayerLives(Player const & player, std::vector<TexturedSprite> healthIcons, s8 font)
{
	AEGfxSetRenderMode(AE_GFX_RM_TEXTURE);

	AEGfxPrint(font, "Lives:", -0.95f, -0.8f, 1.f, 1.f, 1.f, 1.f, 1.f);
	if (player.health <= 0) return;
	for (int i{ 0 }; i < player.health; ++i)
	{
		//TexturedSprite heart = healthIcons[0];
		//heart.position.x += i * 100;
		//heart.UpdateTransform();
		//heart.RenderSprite();
		//healthIcons[i].UpdateTransform();
		healthIcons[i].RenderSprite();
	}
}

void checkIfAlmanacClicked(TexturedSprite const& almanacSprite, Almanac & almanac)
{
	s32 cursorPositionX, cursorPositionY;
	//AEInputGetCursorPosition(&cursorPositionX, &cursorPositionY);

	if (AEInputCheckReleased(AEVK_LBUTTON) && IsCursorInSquare(almanacSprite.position, almanacSprite.scale.x))
	{
		//std::cout << "click!\n";
		almanac.isOpen = true;
		almanac.hasBeenOpened = true;
	}
}

void MoveArrow(TexturedSprite& arrowSprite, Almanac& almanac, f32 deltaTime)
{
	if (!almanac.hasBeenOpened)
	{
		if (arrowGoingUp)
		{
			arrowSprite.position.y += deltaTime * 50.f;
			if (arrowSprite.position.y > -190) arrowGoingUp = false;
		}
		else
		{
			arrowSprite.position.y -= deltaTime * 50.f;
			if (arrowSprite.position.y < -210) arrowGoingUp = true;
		}
		arrowSprite.UpdateTransform();
		//std::cout << arrowSprite.position.y  << "\n";
	}

}

void RenderAlmanacIcon(Almanac& almanac, TexturedSprite & almanacSprite, TexturedSprite& almanacLitUpSprite, TexturedSprite & arrowSprite)
{
	if (IsCursorInSquare(almanacSprite.position, almanacSprite.scale.x) && !almanac.isOpen) almanacLitUpSprite.RenderSprite();
	else almanacSprite.RenderSprite();

	if (!almanac.hasBeenOpened)
	{
		arrowSprite.RenderSprite();
	}
}
