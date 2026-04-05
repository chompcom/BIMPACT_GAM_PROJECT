/* Start Header ************************************************************************/
/*!
\file     HUD.h
\author   Lee Hng Hwee Celest, hnghweecelest.l, 2502385
\par      hnghweecelest.l@digipen.edu
\brief    This file declares some functions that render and deal with the HUD

Copyright (C) 2026 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited.
*/
/* End Header **************************************************************************/

#pragma once
#include <vector>
#include "Almanac.h"

//Shows how much health the player has left
void RenderPlayerLives(Player const& player, std::vector<TexturedSprite> healthIcons, s8 font);

//checks if the almanac icon has been clicked
void CheckIfAlmanacClicked(TexturedSprite const& almanacSprite, Almanac & almanac);

//moves the big red arrow that appears above the almanac icon to make it more noticable
void MoveArrow(TexturedSprite& arrowSprite, Almanac& almanac, f32 deltaTime);

//renders the almanac icon, and a big red arrow if the player has not opened the almanac in this playthrough
void RenderAlmanacIcon(Almanac& almanac, TexturedSprite& almanacSprite, TexturedSprite& almanacLitUpSprite, TexturedSprite& arrowSprite, Player& player);
