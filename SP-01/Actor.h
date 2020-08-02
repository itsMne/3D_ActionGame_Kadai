#pragma once
#include "Object3D.h"

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
public:
	Actor(int Model, int Type);
	~Actor();
	virtual void Init();
	virtual void Update();
	virtual void Draw();
	virtual void End();
	int GetActorType();
};

