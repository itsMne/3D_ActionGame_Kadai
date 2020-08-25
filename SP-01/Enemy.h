#pragma once
#include "Actor.h"
#include "Cube3D.h"
#include "Billboard2D.h"

#define MAX_HIT_EFFECTS 4
#define MAX_ENEMY_HEART 8
#define MAX_ENEMY_HP 150
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
	EN_RUNNING,
	ENEMY_MAX
};
enum ENEMY_HITBOXES
{
	ENEMY_HB_FEET = 0,
	ENEMY_HB_ATTACK,
	ENEMY_HB_BODY,
	ENEMY_HB_MAX
};

enum ENEMY_STATES
{
	EN_STATE_IDLE,
	EN_STATE_DAMAGED,
	EN_STATE_SENDOFF,
	EN_STATE_REDHOTKICKED,
	EN_DEAD,
	EN_ATTACKING,
	EN_MOVING,
	EN_STATE_TELEPORTING,
	EN_STATE_MAX
};

class Enemy :
	public Actor
{
private:
	//ヒットボックス
	Box			Hitboxes[ENEMY_HB_MAX];
	Cube3D*		pVisualHitboxes[ENEMY_HB_MAX];
	//プレイヤーのポインター
	Actor*		pPlayer;
	Billboard*	pHit[MAX_HIT_EFFECTS];
	Billboard*	pHearts;
	//状態
	int			nState;
	int			nSendoffAttack;
	int			nSendOffFrames;
	float		fSendOffAcceleration;
	bool		bCanBeAttacked;
	bool		bAlternatePunchAnim;
	bool		bFollowRoulette;
	int			nCancellingGravityFrames;
	float		fHeartPosHealth[MAX_ENEMY_HEART];
	float		fHeartPosLockOn;
	int			nHP;
	int			nDeathFrameCount;
	int			nPlayerTouchFramesCount;
	int			nIdleWaitFramesCount;
	int			nMaxIdleWaitFrames;
	float		fSpeed;
	Object3D*	pAngrySign;
	int			nEnragedFrames;
public:
	Enemy();
	void SetHitEffect();
	~Enemy();
	void Init();
	void Update();
	void HeartsControl();
	void RedHotKickedStateControl();
	void SendOffStateControl();
	void Jump(float fJumpForce);
	void InitialAttackedAnimation(int currentattack);
	void DamagedStateControl();
	void CameraRumbleControl(int nAttackAnim);
	void GravityControl();
	void Draw();
	void DrawHearts();
	void End();
	Box GetHitboxEnemy(int i);
	bool IsEnemyDead();
	int GetState() { return nState; };
	void Enrage(int Frames) { if (nEnragedFrames > 0)return; nEnragedFrames = Frames; nIdleWaitFramesCount = nMaxIdleWaitFrames; };
};

