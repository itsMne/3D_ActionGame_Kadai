#pragma once
#include "Actor.h"
#include "Cube3D.h"

enum ENEMY_ANIMATIONS
{
	EN_IDLE,
	EN_PUNCHED_A,
	EN_PUNCHED_B,
	EN_KICKED_A,
	EN_KICKED_B,
	EN_SENDTOAIR_AIRIDLE,
	EN_JUMPED_ABOVE,
	EN_LAUNCHED_FORWARD,
	EN_STRONG_HITFORWARD,
	EN_ATTACK_1,
	EN_ATTACK_2,
	EN_DEATH,
	EN_WALKING,
	ENEMY_MAX
};
enum ENEMY_HITBOXES
{
	ENEMY_HB_FEET = 0,
	ENEMY_HB_ATTACK,
	ENEMY_HB_BODY,
	ENEMY_HB_MAX
};
class Enemy :
	public Actor
{
private:
	//ヒットボックス
	Box Hitboxes[ENEMY_HB_MAX];
	Cube3D* pVisualHitboxes[ENEMY_HB_MAX];
	//プレイヤーのポインター
	Actor* pPlayer;
public:
	Enemy();
	~Enemy();
	void Init();
	void Update();
	void Draw();
	void End();

	Box GetHitboxEnemy(int i);
	
};

