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
}

void LogoUpdate(float dt)
{
    if (g_time <= 0.f) {
        ChangeState(GS_MAINMENU);
    }
    g_time -= dt;
}
