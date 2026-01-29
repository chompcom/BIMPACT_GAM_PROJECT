#include "TestScene.h"

#include "AEEngine.h"
#include "Sprite.h"

AEGfxVertexList* sqmesh = nullptr;


void TestLoad()
{
	sqmesh = CreateSquareMesh();
}

void TestInit()
{
}

void TestDraw()
{
}

void TestFree()
{
}

void TestUnload()
{
	AEGfxMeshFree(sqmesh);
}

void TestUpdate()
{
}
