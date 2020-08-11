#include "Enemy.h"



Enemy::Enemy(): Actor(ENEMY_MODEL, A_ENEMY)
{
	Init();
}


Enemy::~Enemy()
{
	End();
	Actor::~Actor();
}

void Enemy::Init()
{
}

void Enemy::Update()
{
	Actor::Update();
}

void Enemy::Draw()
{
}

void Enemy::End()
{
}
