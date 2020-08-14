#include "Actor.h"
#include "S_InGame3D.h"

Actor::Actor(int Model, int Type):Object3D(GetMainLight(), Model, GO_ACTOR), pFloor(nullptr)
{
	nActorType = Type;
	bUseGravity = false;
	bGravityIsActive = false;
	pGame = GetCurrentGame();
}


Actor::~Actor()
{
	End();
}

void Actor::Init()
{
}

void Actor::Update()
{
	Object3D::Update();
}

void Actor::Draw()
{
	Object3D::Draw();
}

void Actor::End()
{
	Object3D::~Object3D();
}

int Actor::GetActorType()
{
	return nActorType;
}
