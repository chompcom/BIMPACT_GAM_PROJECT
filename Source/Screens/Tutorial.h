/* Start Header ************************************************************************/
/*!
\file     Tutorial.h
\author   Lee Hng Hwee Celest, hnghweecelest.l, 2502385
\par      hnghweecelest.l@digipen.edu
\brief    This file declares some functions that implement the tutorial using MJ's UI
          manager

Copyright (C) 2026 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited.
*/
/* End Header **************************************************************************/

#pragma once

//Initialise the tutorial's UIManager variables 
void InitTutorial();

//Renders the tutorial UI
void RenderTutorial();

//Handles the tutorial input
void TutorialInput();

//clear the tutorial's UI mananger
void TutorialUnload();
