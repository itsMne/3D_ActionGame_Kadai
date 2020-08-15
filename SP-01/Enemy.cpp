#include "Enemy.h"
#include "input.h"
#include "S_InGame3D.h"
#include "TechnicalCamera.h"
#include "Player3D.h"

#define SHOW_ENEMY_HITBOX true
#define SHOW_SPECIFIC_PLAYER_HITBOX ENEMY_HB_BODY
#define GRAVITY_FORCE 0.98f

float fEnemyAnimations[ENEMY_MAX] =
{
	1,
	1,
	1,
};

Enemy::Enemy(): Actor(ENEMY_MODEL, A_ENEMY), pPlayer(nullptr), bCanBeAttacked(true)
{
	Init();
	Model->SetRotation({ 0,XM_PI,0 });
	Model->SetScale({ 0.65f, 0.65f, 0.65f });
	Model->SetPosition({ 0,5,0 });
	SetAnimation(EN_IDLE, fEnemyAnimations[EN_IDLE]);
	nState = EN_STATE_IDLE;
}

Enemy::~Enemy()
{
	End();
	Actor::~Actor();
}

void Enemy::Init()
{
	for (int i = 0; i < ENEMY_HB_MAX; i++)
	{
		pVisualHitboxes[i] = nullptr;
		Hitboxes[i] = { 0 };
	}
	Hitboxes[ENEMY_HB_FEET] = { 0, 14.0f, 0, 5.0f, 15.0f, 5.0f };
	Hitboxes[ENEMY_HB_ATTACK] = { 0, 25.0f, 0, 15.0f, 20.0f, 10.0f };
	Hitbox = Hitboxes[ENEMY_HB_BODY] = { 0, 80.0f, 0, 15.0f, 90.0f, 10.0f };
#if SHOW_HITBOX && SHOW_ENEMY_HITBOX
	for (int i = 0; i < ENEMY_HB_MAX; i++)
	{
		pVisualHitboxes[i] = new Cube3D();
		pVisualHitboxes[i]->Init("data/texture/hbox.tga");
		pVisualHitboxes[i]->SetScale({ Hitboxes[i].SizeX, Hitboxes[i].SizeY, Hitboxes[i].SizeZ });
		pVisualHitboxes[i]->SetPosition({ Hitboxes[i].PositionX,Hitboxes[i].PositionY,Hitboxes[i].PositionZ });
		if (SHOW_SPECIFIC_PLAYER_HITBOX == -1 || SHOW_SPECIFIC_PLAYER_HITBOX == i)
			continue;
		pVisualHitboxes[i]->SetInvisible(true);
	}
#endif
}

void Enemy::Update()
{
	Actor::Update();
	if (!pPlayer)
	{
		pPlayer = GetPlayer();
		if (!pPlayer)
			return;
	}
	Player3D* Player = (Player3D*)pPlayer;
#ifdef DEBUG_GRAVITY
	if (GetKeyPress(VK_UP)) {
		Position.y++; return;
	}
#endif
	GravityControl();
	if (IsInCollision3D(Player->GetHitboxPlayer(PLAYER_HB_ATTACK), GetHitboxEnemy(ENEMY_HB_BODY)) && nState != EN_STATE_DAMAGED) {
		nState = EN_STATE_DAMAGED;
		if (!bAlternatePunchAnim)
			SetAnimation(EN_PUNCHED_A, fEnemyAnimations[EN_PUNCHED_A]);
		else
			SetAnimation(EN_PUNCHED_B, fEnemyAnimations[EN_PUNCHED_B]);
		FaceActor(pPlayer);
		bCanBeAttacked = false;
		bAlternatePunchAnim ^= true;
		PLAYER_ATTACK_MOVE* pPlayerAttack = Player->GetCurrentAttack();
		CameraRumbleControl(pPlayerAttack->Animation);
	}
	switch (nState)
	{
	case EN_STATE_IDLE:
		SetAnimation(EN_IDLE, fEnemyAnimations[EN_IDLE]);
		break;
	case EN_STATE_DAMAGED:
		DamagedStateControl();
		break;
	default:
		break;
	}

}

void Enemy::DamagedStateControl()
{
	if (Model->GetLoops() > 0)
	{
		nState = EN_STATE_IDLE;
		SetAnimation(EN_IDLE, fEnemyAnimations[EN_IDLE]);
	}
	Player3D* Player = (Player3D*)pPlayer;
	PLAYER_ATTACK_MOVE* pPlayerAttack = Player->GetCurrentAttack();
	float PosY = Position.y;
	Camera3D* pCamera = (Camera3D*)(GetMainCamera()->GetFocalPoint());
	if (IsInCollision3D(Player->GetHitboxPlayer(PLAYER_HB_ATTACK), GetHitboxEnemy(ENEMY_HB_BODY)) && bCanBeAttacked) {
		bAlternatePunchAnim ^= true;
		FaceActor(pPlayer);
		bCanBeAttacked = false;
		CameraRumbleControl(pPlayerAttack->Animation);
	}
	if(!IsInCollision3D(Player->GetHitboxPlayer(PLAYER_HB_ATTACK), GetHitboxEnemy(ENEMY_HB_BODY)))
		bCanBeAttacked = true;
	if (!pPlayerAttack) {
		bCanBeAttacked = true;
		return;
	}
	switch (pPlayerAttack->Animation)
	{
	default:
		if(!bAlternatePunchAnim)
			SetAnimation(EN_PUNCHED_A, fEnemyAnimations[EN_PUNCHED_A]);
		else
			SetAnimation(EN_PUNCHED_B, fEnemyAnimations[EN_PUNCHED_B]);
		Position = Player->GetHitboxPos(PLAYER_HB_ATTACK);
		Position.y = PosY;
		pCamera->SetZooming(60, 15, 2, 2);
		break;
	}
}

void Enemy::CameraRumbleControl(int nAttackAnim)
{
	Camera3D* pCamera = (Camera3D*)(GetMainCamera()->GetFocalPoint());
	switch (nAttackAnim)
	{
	default:
		//pCamera->SetShaking(1.5f, 7, 3);
		pCamera->SetShaking(6.0f, 7, 2);
		break;
	}
}

void Enemy::GravityControl()
{
	if (pFloor) {
		fGravityForce = 0;
		if (!IsInCollision3D(pFloor->GetHitbox(), GetHitboxEnemy(ENEMY_HB_FEET)))
			pFloor = nullptr;
		return;
	}
	else {
		fGravityForce += GRAVITY_FORCE;
		Position.y -= fGravityForce;
		
		pFloor = ((S_InGame3D*)pGame)->GetList(GO_FLOOR)->CheckCollision(GetHitboxEnemy(ENEMY_HB_FEET));
		if (pFloor)
		{
			while (IsInCollision3D(pFloor->GetHitbox(), GetHitboxEnemy(ENEMY_HB_FEET)))
				Position.y += 0.1f;
			Position.y -= 0.1f;
		}
	}
}

void Enemy::Draw()
{
	Actor::Draw();
#if SHOW_HITBOX && SHOW_ENEMY_HITBOX
	GetMainLight()->SetLightEnable(false);
	for (int i = 0; i < ENEMY_HB_MAX; i++)
	{
		if (!pVisualHitboxes[i])continue;
		Box pHB = GetHitboxEnemy(i);
		pVisualHitboxes[i]->SetScale({ pHB.SizeX, pHB.SizeY, pHB.SizeZ });
		pVisualHitboxes[i]->SetPosition({ pHB.PositionX, pHB.PositionY, pHB.PositionZ });
		pVisualHitboxes[i]->Draw();
	}
	GetMainLight()->SetLightEnable(true);
#endif

}

void Enemy::End()
{
}

Box Enemy::GetHitboxEnemy(int hb)
{
	return { Hitboxes[hb].PositionX + Position.x, Hitboxes[hb].PositionY + Position.y,Hitboxes[hb].PositionZ + Position.z, Hitboxes[hb].SizeX,Hitboxes[hb].SizeY,Hitboxes[hb].SizeZ };
}