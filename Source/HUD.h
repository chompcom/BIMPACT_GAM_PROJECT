#pragma once
#include <vector>
#include "Almanac.h"

void renderPlayerLives(Player const& player, std::vector<TexturedSprite> healthIcons, s8 font);

void checkIfAlmanacClicked(TexturedSprite const& almanacSprite, Almanac & almanac);
