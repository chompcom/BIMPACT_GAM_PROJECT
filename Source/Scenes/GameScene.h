#include "AEEngine.h"
#include "Player.h"
#include "Gift.h"
#include "Vector2.hpp"
#include "Enemy.h"

/*
AEGfxVertexList* squareMesh;// = CreateSquareMesh();

AEGfxVertexList* circleMesh;// = CreateCircleMesh();

AEGfxTexture* bearTexture;// = AEGfxTextureLoad("Assets/player.png");
AEGfxTexture* popRocksTexture;// = AEGfxTextureLoad("Assets/poprocks.png");

Labels bearTraits{ "Cold", "Smart" };
Labels bearLikes{ "Cold", "Clean" };
Labels bearDislikes{ "Heaty", "Gross" };
EnemyType bear = EnemyType("bear", 40.f, 5.f, bearTraits, bearLikes, bearDislikes);

Enemy bear1 = Enemy(bear, TexturedSprite(squareMesh, bearTexture, Vector2{ -700.f, -100.f }, Vector2{ 80.f, 80.f }, Color{ 1.f,1.f,1.f,1.f }),
    ES_ANGRY);
Enemy bear2 = Enemy(bear, TexturedSprite(squareMesh, bearTexture, Vector2{ 300.f, 400.f }, Vector2{ 80.f, 80.f }, Color{ 1.f,1.f,1.f,1.f }),
    ES_NEUTRAL);

Labels popRocksTraits{ "Sweet", "Explosive", "Heaty" };
Labels popRocksLikes{ "Heaty", "Sweet", "Fragrant" };
Labels popRocksDislikes{ "Boring", "Bitter", "Cold" };
EnemyType popRocks = EnemyType("poprocks", 20.f, 15.f, popRocksTraits, popRocksLikes, popRocksDislikes);

//popRocks.neutral = WalkLeft;
//popRocks.happy = WalkRight;
Enemy popRocks1 = Enemy(popRocks, TexturedSprite(squareMesh, popRocksTexture, Vector2{ 100.f, -250.f }, Vector2{ 80.f, 80.f }, Color{ 1.f,1.f,1.f,1.f }),
    ES_NEUTRAL);

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
    Gift("Nugget",{"Cheap","Tasty"},testGiftsSprites[0], (testGiftsSprites[0]).position),
    Gift("Candy",{"Sweet","Tasty"},testGiftsSprites[1], (testGiftsSprites[1]).position)
};


void GameSceneUpdate(float deltaTime) {

    UpdatePlayer(player, deltaTime);
    player.sprite.UpdateTransform();
    //set the direction test to appear in front of the player
    directionTest.position = player.position + (player.direction * 100.0f);
    directionTest.UpdateTransform();

    //this is just to test throwing
    for (int i = 0; i < 2; i++)
    {
        UpdateGift(testGifts[i], player, deltaTime);
        (testGifts[i]).sprite.UpdateTransform();
    }

    AEGfxSetBackgroundColor(1.0f, 1.0f, 1.0f);

    AEGfxSetRenderMode(AE_GFX_RM_COLOR);
    AEGfxSetBlendMode(AE_GFX_BM_BLEND);
    AEGfxSetTransparency(1.0f);

    testGifts[0].sprite.RenderSprite();
    testGifts[1].sprite.RenderSprite();
    player.sprite.RenderSprite();
    directionTest.RenderSprite();

    AEGfxSetRenderMode(AE_GFX_RM_TEXTURE);
    bear1.sprite.RenderSprite();
    bear2.sprite.RenderSprite();
    if (AEInputCheckTriggered(AEVK_0)) {
        popRocks1.ChangeState(EnemyStates::ES_HAPPY);
    }
    else if (AEInputCheckTriggered(AEVK_1)) {
        popRocks1.ChangeState(ES_NEUTRAL);
    }
    else if (AEInputCheckTriggered(AEVK_9)) {
        popRocks1.ChangeState(ES_ANGRY);
    }
    popRocks1.Update(deltaTime);
    popRocks1.sprite.RenderSprite();

}

*/