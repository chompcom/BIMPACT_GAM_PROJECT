/* Start Header ************************************************************************/
/*!
\file		LogoScene.cpp
\author 	Hong Josiah Qin, hong.j, 2501239
\par  		hong.j@digipen.edu
\brief		This scene is only used to display the Digipen logo

Copyright (C) 2026 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited.
*/
/* End Header **************************************************************************/

#include "AEEngine.h"
#include "LogoScene.h"
#include "Sprite.h"
#include "Loaders/DataLoader.h"
#include "GameStateManager.h"


static float g_time;


void LogoLoad()
{
    
}

void LogoInit()
{
    //set it to 3 seconds
    g_time = 3.f;
}

void LogoDraw()
{

    AEGfxSetRenderMode(AE_GFX_RM_TEXTURE);
    AEGfxSetBlendMode(AE_GFX_BM_BLEND);
    AEGfxSetTransparency(1.0f);		
    static TexturedSprite logo = DataLoader::CreateTexture("Assets/logo.png");
    float scaleOffsetX = 0.95f;
    float scaleOffsetY = 0.8f;
    logo.scale = Vector2{ AEGfxGetWindowWidth() * scaleOffsetX, AEGfxGetWindowHeight() * scaleOffsetY};
    logo.UpdateTransform();
    logo.RenderSprite();

}

void LogoFree()
{
}

void LogoUnload()
{
    //Should always unload data loader
    DataLoader::Unload();
}

void LogoUpdate(float dt)
{
    //When the 3 seconds are up, go back to main menu
    if (g_time <= 0.f) {
        ChangeState(GS_MAINMENU);
    }
    g_time -= dt;
}
