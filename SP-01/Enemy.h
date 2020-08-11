#pragma once
#include "Actor.h"
class Enemy :
	public Actor
{
public:
	Enemy();
	~Enemy();
	void Init();
	void Update();
	void Draw();
	void End();
	
};

