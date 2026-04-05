/* Start Header ************************************************************************/
/*!
\file     HUD.cpp
\author   Lee Hng Hwee Celest, hnghweecelest.l, 2502385
\par      hnghweecelest.l@digipen.edu
\brief    This file defines some functions that render and deal with the HUD

Copyright (C) 2026 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited.
*/
/* End Header **************************************************************************/

#include "AEEngine.h"
#include "Player.h"
#include "Sprite.h"
#include "Almanac.h"
#include "Utils/Utils.h"
#include <iostream>
#include <vector>

//to check if the red arrow should move up or down
static bool arrowGoingUp = false;

//Shows how much health the player has left
void RenderPlayerLives(Player const & player, std::vector<TexturedSprite> healthIcons, s8 font)
{
	//set the proper render mode to properly render textures
	AEGfxSetRenderMode(AE_GFX_RM_TEXTURE);
	//lives text
	AEGfxPrint(font, "Lives:", -0.95f, -0.8f, 1.f, 1.f, 1.f, 1.f, 1.f);

	//render the heart icons depending on how much health the player has left
	if (player.health <= 0) return;
	for (int i{ 0 }; i < player.health; ++i)
	{
		healthIcons[i].RenderSprite();
	}
}

//checks if the almanac icon has been clicked
void CheckIfAlmanacClicked(TexturedSprite const& almanacSprite, Almanac & almanac)
{
	//open the almananc if the icon has been clicked
	if (AEInputCheckTriggered(AEVK_LBUTTON) && IsCursorInSquare(almanacSprite.position, almanacSprite.scale.x))
	{
		almanac.isOpen = true;
		almanac.hasBeenOpened = true;
	}
}

//moves the big red arrow that appears above the almanac icon to make it more noticable
void MoveArrow(TexturedSprite& arrowSprite, Almanac& almanac, f32 deltaTime)
{
	//move the arrow is the almanac has not been opened in this playthrough
	if (!almanac.hasBeenOpened)
	{
		//alternate between moving the arrow up and down
		if (arrowGoingUp)
		{
			arrowSprite.position.y += deltaTime * 50.f;
			if (arrowSprite.position.y > -170) arrowGoingUp = false;
		}
		else
		{
			arrowSprite.position.y -= deltaTime * 50.f;
			if (arrowSprite.position.y < -190) arrowGoingUp = true;
		}
		arrowSprite.UpdateTransform();
	}

}

//renders the almanac icon, and a big red arrow if the player has not opened the almanac in this playthrough
void RenderAlmanacIcon(Almanac& almanac, TexturedSprite & almanacSprite, TexturedSprite& almanacLitUpSprite, TexturedSprite & arrowSprite, Player & player)
{
	//if the player's cursor is hovering over the almanac icon, display the lit up version. Or else, display the normal version
	if (IsCursorInSquare(almanacSprite.position, almanacSprite.scale.x) && !almanac.isOpen && player.health > 0) almanacLitUpSprite.RenderSprite();
	else almanacSprite.RenderSprite();

	//render the big red arrow if the player has not opened the almanac in this playthrough
	if (!almanac.hasBeenOpened)
	{
		arrowSprite.RenderSprite();
	}
}
