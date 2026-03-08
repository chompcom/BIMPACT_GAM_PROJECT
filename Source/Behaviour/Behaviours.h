#pragma once

#include "Enemy.h"

Command GetCommand(std::string name);
FlagCheck GetFlag(std::string name);
void InitCommands();