#include "Actor.h"
#include "S_InGame3D.h"

Actor::Actor(int Model, int Type):Object3D(GetMainLight(), Model, GO_ACTOR), pFloor(nullptr)
{
	nActorType = Type;
	bUseGravity = false;
	bGravityIsActive = false;
	fGravityForce = 0;
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

void Actor::FaceActor(Actor* pActor)
{
	if (!pActor)
		return;
	XMFLOAT3 a;
	XMFLOAT3 calc = GetVectorDifference(pActor->GetPosition(), Position);
	a.x = sin(GetRotation().y);
	a.y = sin(GetRotation().x);
	a.z = cos(GetRotation().y);
	XMFLOAT3 b = NormalizeVector(calc);
	XMVECTOR dot = XMVector3Dot(XMLoadFloat3(&a), XMLoadFloat3(&b));


	float rotationAngle = (float)acos(XMVectorGetX(dot));
	rotationAngle = ceilf(rotationAngle * 10) / 10;
	if (pActor->GetPosition().x < Position.x)
		Model->SetRotationY(-rotationAngle);
	else
		Model->SetRotationY(rotationAngle);
}
