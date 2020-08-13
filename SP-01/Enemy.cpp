#include "Enemy.h"

Enemy::Enemy(): Actor(ENEMY_MODEL, A_ENEMY)
{
	Init();
	Model->SetRotation({ 0,XM_PI,0 });
	Model->SetScale({ 0.65f, 0.65f, 0.65f });
	Model->SetPosition({ 0,5,0 });
	Model->SwitchAnimation(EN_IDLE, 0, 1);
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
	Actor::Draw();
}

void Enemy::End()
{
}
