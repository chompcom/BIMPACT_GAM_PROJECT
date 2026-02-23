//
// Spritesheet animation 
// 
// AUTHORS: 
//   Gerald Wong
//  
// DESCRIPTION:
//   The code below shows how to swap between different sprites
//   within a spritesheet over time, emulating sprite animation. 
//
//   Press [0] to decrease animation speed.
//   Press [1] to increase animation speed.
//
//   Students should not take this code as a reference to
//   "good architecture". That's not the point of this demo.
//


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


int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,
    _In_ int       nCmdShow)
{
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);


    int gGameRunning = 1;


    // Initialization of your own variables go here

    // Using custom window procedure
    AESysInit(hInstance, nCmdShow, 1600, 900, 1, 60, true, NULL);

    // Changing the window title
    AESysSetWindowTitle("UI Demo!");
    GSM_Initialize(GS_LEVEL1);
    // Game Loop
    while (gGameRunning)
    {
       
        
        if (current != GS_RESTART) {
            GSM_Update();
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
            if (AEInputCheckTriggered(AEVK_ESCAPE) || 0 == AESysDoesWindowExist()) {
                gGameRunning = 0;
                next = GS_QUIT;
            }
        }

        fpFree();

        if (next != GS_RESTART) {
            fpUnload();
        }
        previous = current;
        current = next;
        
        //super scuffed fps check
        //std::cout << AEFrameRateControllerGetFrameRate() << std::endl;
        

        

        // check if forcing the application to quit
        
    }

    // free the system
    AESysExit();

}
