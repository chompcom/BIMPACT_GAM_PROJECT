#pragma once

//#include "Almanac.h"
#include "AEEngine.h"
#include "Sprite.h"
#include "Player.h"
#include "GameStateList.h"
#include "GameStateManager.h"
#include "Utils/Utils.h"
#include <vector>
#include <iostream>

void RenderGameOverScreen(s8 font, std::vector<TexturedSprite>& buttonSprite, Sprite& screen, Player& player);

void HandleGameOverInputs(std::vector<TexturedSprite>& buttonSprite);
