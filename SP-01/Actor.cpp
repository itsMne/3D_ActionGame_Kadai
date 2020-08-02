#include "Actor.h"
#define GRAVITY_FORCE 0.35f

Actor::Actor(int Model, int Type):Object3D(GetMainLight(), Model, GO_ACTOR)
{
	nActorType = Type;
	bUseGravity = false;
	bGravityIsActive = false;
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
