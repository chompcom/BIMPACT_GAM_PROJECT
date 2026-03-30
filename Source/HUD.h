#pragma once
#include <vector>
#include "Almanac.h"

void renderPlayerLives(Player const& player, std::vector<TexturedSprite> healthIcons, s8 font);

void checkIfAlmanacClicked(TexturedSprite const& almanacSprite, Almanac & almanac);

void MoveArrow(TexturedSprite& arrowSprite, Almanac& almanac, f32 deltaTime);

void RenderAlmanacIcon(Almanac& almanac, TexturedSprite& almanacSprite, TexturedSprite& almanacLitUpSprite, TexturedSprite& arrowSprite, Player& player);
