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
#include "Vector2.hpp"
#include "Utils.h"
#include "Player.h"
#include "Gift.h"


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

    AEGfxVertexList* squareMesh = CreateSquareMesh();

    AEGfxVertexList* circleMesh = CreateCircleMesh();

    AEGfxTexture* playerTexture = AEGfxTextureLoad("Assets/player.png");

    //Sprite damageArea(circleMesh, Vector2{ 400.f, -50.f }, Vector2{ 400.f, 400.f }, Color{ 1.f, 0.f, 0.f, 1.f });
    //Sprite healArea(circleMesh, Vector2{ -400.f, -50.f }, Vector2{ 400.f, 400.f }, Color{ 0.f, 1.f, 0.f, 1.f });

    //Sprite player(circleMesh, Vector2{ 0.f, -50.f }, Vector2{ 80.f, 80.f }, Color{ 0.f, 0.f, 1.f, 1.f });

    TexturedSprite testSprite(squareMesh, playerTexture, Vector2{ 0.f, 0.f }, Vector2{ 200.f, 200.f }, Color{ 1.f, 1.f, 1.f, 1.f });

    Sprite playerSprite(squareMesh, Vector2{ 0.f, -50.f }, Vector2{ 80.f, 80.f }, Color{ 0.f, 0.f, 1.f, 1.f });

    Player player(playerSprite, 25000.f, 600.f, Vector2{ 0.f, -50.f });

    //This is just to see what direction the player is facing
    Sprite directionTest(circleMesh, player.direction, Vector2{ 20.f, 20.f }, Color{ 0.f, 0.f, 0.5f, 1.f });

    //for gift throw testing
    Sprite testGiftsSprites[2]{
        Sprite(squareMesh, Vector2{ 100.f, 100.f }, Vector2{ 80.f, 80.f }, Color{ 0.75f, 0.f, 0.f, 1.f }),
        Sprite(squareMesh, Vector2{ -100.f, -100.f }, Vector2{ 80.f, 80.f }, Color{ 0.f, 0.75f, 0.f, 1.f })
    };

    Gift testGifts[2]{
        Gift(testGiftsSprites[0], (testGiftsSprites[0]).position),
        Gift(testGiftsSprites[1], (testGiftsSprites[1]).position)
    };

    //Sprite healthBarBack(squareMesh, Vector2{ 0.f, 340.f }, Vector2{ 1285.f, 45.f }, Color{ 0.6f, 0.f, 0.f, 1.f });
    //Sprite healthBarFore(squareMesh, Vector2{ 0.f, 340.f }, Vector2{ 1285.f, 45.f }, Color{ 1.f, 0.f, 0.f, 1.f });

    ////Sprite healthIcon(squareMesh, Vector2{-607.5f,450.f}, Vector2{70.f, 45.f}, Color{ 1.f, 0.f, 0.f, 1.f });

    //Sprite healthIcons[10]{
    //    Sprite(squareMesh, Vector2{-607.5f,250.f}, Vector2{70.f,45.f}, Color{1.f,0.f,0.f,1.f}),
    //    Sprite(squareMesh, Vector2{-472.5f,250.f}, Vector2{70.f,45.f}, Color{1.f,0.f,0.f,1.f}),
    //    Sprite(squareMesh, Vector2{-337.5f,250.f}, Vector2{70.f,45.f}, Color{1.f,0.f,0.f,1.f}),
    //    Sprite(squareMesh, Vector2{-202.5f,250.f}, Vector2{70.f,45.f}, Color{1.f,0.f,0.f,1.f}),
    //    Sprite(squareMesh, Vector2{-67.5f,250.f}, Vector2{70.f,45.f}, Color{1.f,0.f,0.f,1.f}),
    //    Sprite(squareMesh, Vector2{67.5f,250.f}, Vector2{70.f,45.f}, Color{1.f,0.f,0.f,1.f}),
    //    Sprite(squareMesh, Vector2{202.5f,250.f}, Vector2{70.f,45.f}, Color{1.f,0.f,0.f,1.f}),
    //    Sprite(squareMesh, Vector2{337.5f,250.f}, Vector2{70.f,45.f}, Color{1.f,0.f,0.f,1.f}),
    //    Sprite(squareMesh, Vector2{472.5f,250.f}, Vector2{70.f,45.f}, Color{1.f,0.f,0.f,1.f}),
    //    Sprite(squareMesh, Vector2{607.5f,250.f}, Vector2{70.f,45.f}, Color{1.f,0.f,0.f,1.f}),
    //};

    //f32 playerHealth = 100.0f;

    // Game Loop
    while (gGameRunning)
    {

        // Informing the system about the loop's start
        AESysFrameStart();

        f32 current_Time = (f32)AEFrameRateControllerGetFrameTime();
        //f32 speed = 600.0f * current_Time;

        //if (AEInputCheckCurr(AEVK_W)) {
        //    player.position.y += speed;
        //    player.UpdateTransform();
        //}
        //else if (AEInputCheckCurr(AEVK_S)) {
        //    player.position.y -= speed;
        //    player.UpdateTransform();
        //}
        //else if (AEInputCheckCurr(AEVK_A)) {
        //    player.position.x -= speed;
        //    player.UpdateTransform();
        //}
        //else if (AEInputCheckCurr(AEVK_D)) {
        //    player.position.x += speed;
        //    player.UpdateTransform();
        //}

        UpdatePlayer(player, current_Time);
        player.sprite.UpdateTransform();
        //set the direction test to appear in front of the player
        directionTest.position = player.position + (player.direction * 100.0f);
        directionTest.UpdateTransform();

        //this is just to test throwing
        for (int i = 0; i < 2; i++)
        {
            UpdateGift(testGifts[i], player, current_Time);
            (testGifts[i]).sprite.UpdateTransform();
        }

        //// Player in damage AOE
        //if (AreCirclesIntersecting(player.position, 40.f, damageArea.position, 200.0f))
        //{
        //    playerHealth -= 15.f * current_Time;
        //    if (playerHealth < 0.f) playerHealth = 0.f;
        //}
        //// Player in heal AOE
        //else if (AreCirclesIntersecting(player.position, 40.f, healArea.position, 200.0f))
        //{
        //    playerHealth += 15.f * current_Time;
        //    if (playerHealth > 100.f) playerHealth = 100.f;
        //}
        //
        //healthBarFore.position = Vector2{ 0.f - (100.f - playerHealth)/100.f * 642.5f, 340.f };
        //healthBarFore.scale = Vector2{ playerHealth/100.f * 1285.f, 45.f};
        //healthBarFore.UpdateTransform();

        AEGfxSetBackgroundColor(1.0f, 1.0f, 1.0f);

        AEGfxSetRenderMode(AE_GFX_RM_COLOR);
        AEGfxSetBlendMode(AE_GFX_BM_BLEND);
        AEGfxSetTransparency(1.0f);

        //damageArea.RenderSprite();
        //healArea.RenderSprite();
        testGifts[0].sprite.RenderSprite();
        testGifts[1].sprite.RenderSprite();
        player.sprite.RenderSprite();
        directionTest.RenderSprite();

        AEGfxSetRenderMode(AE_GFX_RM_TEXTURE);
        testSprite.RenderSprite();

        //healthBarBack.RenderSprite();
        //healthBarFore.RenderSprite();

        //s32 n = playerHealth == 0.f ? -1 : playerHealth == 100.f ? 10 : (s32)playerHealth / 10 + 1;
        //for (s32 i{}; i < n; i++) {
        //    healthIcons[i].RenderSprite();
        //}

        // Informing the system about the loop's end
        AESysFrameEnd();

        // check if forcing the application to quit
        if (AEInputCheckTriggered(AEVK_ESCAPE) || 0 == AESysDoesWindowExist())
            gGameRunning = 0;
    }

    AEGfxMeshFree(squareMesh);
    AEGfxMeshFree(circleMesh);
    AEGfxTextureUnload(playerTexture);
    // free the system
    AESysExit();

}
