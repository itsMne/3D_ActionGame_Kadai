#include "Enemy.h"

enum EnemyAnimations
{
	ENEMY_IDLE,
	ENEMY_MAX
};

Enemy::Enemy(): Actor(ENEMY_MODEL, A_ENEMY)
{
	Init();
	Model->SetRotation({ 0,XM_PI,0 });
	Model->SetScale({ 0.65f, 0.65f, 0.65f });
	Model->SetPosition({ 0,5,0 });
	Model->SwitchAnimation(ENEMY_IDLE, 0, 1);
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
