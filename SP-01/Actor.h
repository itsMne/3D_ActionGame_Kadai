#pragma once
#include "Object3D.h"
#include "S_Scene3D.h"

enum eActors
{
	A_PLAYER=0,
	A_ENEMY,
	A_NPC,
	A_MAX
};

class Actor :
	public Object3D
{
private:
	int nActorType;
	bool bUseGravity;
	bool bGravityIsActive;
protected:
	Object3D* pFloor;
	float fGravityForce;
	Scene3D* pGame;
public:
	Actor(int Model, int Type);
	~Actor();
	virtual void Init();
	virtual void Update();
	virtual void Draw();
	virtual void End();
	int GetActorType();
};

