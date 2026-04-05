/* Start Header ************************************************************************/
/*!
\file		Behaviours.h
\author 	Hong Josiah Qin, hong.j, 2501239
\par  		hong.j@digipen.edu
\brief		Contains the various actions and contexts that the enemies will use.
Their implementation is hidden and only accessible via "GetCommand" where you can access
the function via a string version of its name.

Copyright (C) 2026 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited.
*/
/* End Header **************************************************************************/

#pragma once

#include "Enemy.h"

//! \brief Returns a function pointer to command, which performs an action related to an enemy entity.
//! Command is a function that looks like "void func(Enemy& me)" where each behaviour alters the enemy in some way
//! to perform the action.
Command GetCommand(std::string name);

//! \brief Returns a function pointer to a context, which is checked before performing actions typically.
//! FlagCheck is a function that returns a bool value that looks like "bool func(Enemy& me)" where it typically checks the state of the enemy
//! and returns a boolean value depending on what the function wants to check.
FlagCheck GetFlag(std::string name);

//! \brief Initializes the functions to have a string name equivalent
//! This needs to be called early on so that GetCommand and GetFlag can be used properly.
void InitCommands();