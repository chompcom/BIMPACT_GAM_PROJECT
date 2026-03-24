//#include "AEEngine.h"
//#include "Player.h"
//#include "Sprite.h"
//#include "Almanac.h"
//#include "Utils/Utils.h"
//#include <iostream>
//#include <vector>
#include "GameOver.h"

void RenderGameOverScreen(s8 font, std::vector<TexturedSprite>& buttonSprite, Sprite& screen, Player & player)
{
	if (player.health <= 0)
	{
		AEGfxSetRenderMode(AE_GFX_RM_COLOR);
		screen.RenderSprite(true);

		AEGfxSetRenderMode(AE_GFX_RM_TEXTURE);

		char buffer[50];
		f32 textWidth, textHeight;


		sprintf_s(buffer, 50, "Game Over");
		AEGfxGetPrintSize(font, buffer, 4.f, &textWidth, &textHeight);
		AEGfxPrint(font, buffer, -textWidth / 2, -textHeight / 2 + 0.3f, 4.f, 1.f, 1.f, 1.f, 1.f);

		for (int i{ 0 }; i < 2; ++i)
		{
			if (IsCursorInRect(Vector2(buttonSprite[i].position.x, buttonSprite[i].position.y), buttonSprite[i].scale.x, buttonSprite[i].scale.y))
				buttonSprite[i + 2].RenderSprite();
			else buttonSprite[i].RenderSprite();
		}

		sprintf_s(buffer, 50, "Quit");
		AEGfxGetPrintSize(font, buffer, 2.f, &textWidth, &textHeight);
		AEGfxPrint(font, buffer, -textWidth / 2 + (buttonSprite[0].position.x / 800.f),
			-textHeight / 2 + (buttonSprite[0].position.y / 450.f) + 0.01f, 2.f, 1.f, 1.f, 1.f, 1.f);

		sprintf_s(buffer, 50, "Restart");
		AEGfxGetPrintSize(font, buffer, 2.f, &textWidth, &textHeight);
		AEGfxPrint(font, buffer, -textWidth / 2 + (buttonSprite[1].position.x / 800.f),
			-textHeight / 2 + (buttonSprite[1].position.y / 450.f) + 0.01f, 2.f, 1.f, 1.f, 1.f, 1.f);
	}


}

void HandleGameOverInputs(std::vector<TexturedSprite>& buttonSprite)
{
	if (IsCursorInRect(Vector2(buttonSprite[0].position.x, buttonSprite[0].position.y), buttonSprite[0].scale.x, buttonSprite[0].scale.y)
		&& AEInputCheckReleased(AEVK_LBUTTON))
	{
		//next = GS_MAINMENU;
		ChangeState(GS_MAINMENU);
	}

	if (IsCursorInRect(Vector2(buttonSprite[1].position.x, buttonSprite[1].position.y), buttonSprite[1].scale.x, buttonSprite[1].scale.y)
		&& AEInputCheckReleased(AEVK_LBUTTON))
	{
		//next = GS_RESTART;
		ChangeState(GS_RESTART);
	}
}

