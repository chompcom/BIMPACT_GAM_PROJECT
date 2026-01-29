#pragma once
#include "AEEngine.h"
#include "GameStateList.h"

typedef void(*FP)();

using Update = void(*)(float);

extern int current, previous, next;

extern FP fpLoad, fpInitialize, fpDraw, fpFree, fpUnload;

extern Update fpUpdate;

void GSM_Initialize(int startingState);
void GSM_Update();
void ChangeState(GS_STATES thing);