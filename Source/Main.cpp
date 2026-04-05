/* Start Header ************************************************************************/
/*!
\file		Main.h

\author     Yee Kiat Lim, yeekiat.lim, 2503993
\par        yeekiat.lim@digipen.edu

\author 	Hong Josiah Qin, hong.j, 2501239
\par  		hong.j@digipen.edu

\author		Lee Hng Hwee Celest, hnghweecelest.l, 2502385
\par		hnghweecelest.l@digipen.edu

\author     Brandon Choo, b.choo, 2501888
\par        b.choo@digipen.edu

\author		Ming Jun, m.quah, 2501182
\par		m.quah@digipen.edu

\brief		Where the main game loop lies.
This is the starting point of the program and everything that needs to be initilised for the whole
program will be done here.

Copyright (C) 2026 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited.
*/
/* End Header **************************************************************************/

#include <crtdbg.h> // To check for memory leaks
#include "AEEngine.h"
#include "Sprite.h"
#include "Utils/Vector2.hpp"
#include "Utils/Utils.h"
#include "Player.h"
#include "Gift.h"
#include "Enemy.h"
#include "HUD.h"
#include "GameStateList.h"
#include "GameStateManager.h"

#include <iostream>

#include <Grid.h>

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,
    _In_ int       nCmdShow)
{
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // Using custom window procedure
    AESysInit(hInstance, nCmdShow, 1600, 900, 1, 60, true, NULL);

    // Changing the window title
    AESysSetWindowTitle("Friend Dungeon!");
    GSM_Initialize(GS_LOGO);
    // Game Loop
    while (current != GS_QUIT)
    {
        if (current != GS_RESTART) {
            GSMUpdate();
            fpLoad();
        }
        else {
            next = previous;
            current = previous;
        }

        fpInitialize();

        while (next == current) {
            AESysFrameStart();
            fpUpdate(static_cast<float>(AEFrameRateControllerGetFrameTime()));
            fpDraw();
            AESysFrameEnd();
            if (0 == AESysDoesWindowExist()) {
                next = GS_QUIT;
            }
        }

        fpFree();

        if (next != GS_RESTART) {
            fpUnload();
        }
        previous = current;
        current = next;
        
        
    }

    // free the system
    AESysExit();
    return 0;

}
