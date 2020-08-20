#include "Enemy.h"
#include "input.h"
#include "S_InGame3D.h"
#include "TechnicalCamera.h"
#include "InputManager.h"
#include "Player3D.h"

#define SHOW_ENEMY_HITBOX false
#define SHOW_SPECIFIC_PLAYER_HITBOX ENEMY_HB_BODY
#define GRAVITY_FORCE 0.98f*2

float fEnemyAnimations[ENEMY_MAX] =
{
	1,//EN_IDLE,
	2,//EN_PUNCHED_A,
	2,//EN_PUNCHED_B,
	1.5f,//EN_KICKED_A,
	1.5f,//EN_KICKED_B,
	1,//EN_SENDTOAIR_AIRIDLE,
	1,//EN_JUMPED_ABOVE,
	1,//EN_LAUNCHED_FORWARD,
	1,//EN_STRONG_HITFORWARD,
	1,//EN_ATTACK_1,
	1,//EN_ATTACK_2,
	1,//EN_DEATH,
	1,//EN_WALKING,
};

Enemy::Enemy(): Actor(ENEMY_MODEL, A_ENEMY), pPlayer(nullptr), bCanBeAttacked(true)
{
	Init();
	Model->SetRotation({ 0,XM_PI,0 });
	Model->SetScale({ 0.65f, 0.65f, 0.65f });
	Model->SetPosition({ 0,5,0 });
	SetAnimation(EN_IDLE, fEnemyAnimations[EN_IDLE]);
	nState = EN_STATE_IDLE;
	nCancellingGravityFrames = 0;
	for (int i = 0; i < MAX_HIT_EFFECTS; pHit[i] = nullptr, i++);
	pHearts = new Billboard(BB_HEART, { 0.5f,0.5f });
	pHearts->SetUVFrames(3, 1, 15);
	pHearts->SetPosition({ 0, 0, 0 });
	pHearts->SetUnusableAfterAnimation(false);
	fHeartPosLockOn = 0;
	for (int i = 0; i < MAX_ENEMY_HEART; i++)
	{
		fHeartPosHealth[i] = 0;
	}
}

void Enemy::SetHitEffect()
{
	for (int i = 0; i < MAX_HIT_EFFECTS; i++)
	{
		if (pHit[i])
			continue;
		pHit[i] = new Billboard(BB_HIT, {3.2f, 3.2f});
		pHit[i]->SetUVFrames(4, 5, 0);
		pHit[i]->SetUnusableAfterAnimation(true);
		return;
	}
}

Enemy::~Enemy()
{
	End();
	Actor::~Actor();
}

void Enemy::Init()
{
	fSendOffAcceleration = nSendOffFrames = 0;
	for (int i = 0; i < ENEMY_HB_MAX; i++)
	{
		pVisualHitboxes[i] = nullptr;
		Hitboxes[i] = { 0 };
	}
	Hitboxes[ENEMY_HB_FEET] = { 0, 14.0f, 0, 5.0f, 15.0f, 5.0f };
	Hitboxes[ENEMY_HB_ATTACK] = { 0, 25.0f, 0, 15.0f, 20.0f, 10.0f };
	Hitbox = Hitboxes[ENEMY_HB_BODY] = { 0, 80.0f, 0, 50.0f, 90.0f, 50.0f };
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
		Jump(10);
	}
#endif
	
	if (IsInCollision3D(Player->GetHitboxPlayer(PLAYER_HB_ATTACK), GetHitboxEnemy(ENEMY_HB_BODY)) && nState != EN_STATE_DAMAGED && nState != EN_STATE_REDHOTKICKED) {
		nState = EN_STATE_DAMAGED;
		
		FaceActor(pPlayer);
		bCanBeAttacked = false;
		bAlternatePunchAnim ^= true;
		PLAYER_ATTACK_MOVE* pPlayerAttack = Player->GetCurrentAttack();
		if (pPlayerAttack) {
			CameraRumbleControl(pPlayerAttack->Animation);
			InitialAttackedAnimation(pPlayerAttack->Animation);
		}
		if (pPlayerAttack && pPlayerAttack->Animation == ROULETTE)
			bFollowRoulette = true;
		else 
			bFollowRoulette = false;
		if (pPlayerAttack && pPlayerAttack->Animation == RED_HOT_KICK)
			Player->RedHotKicked();
		fGravityForce = 0;
		nCancellingGravityFrames = 70;
		SetHitEffect();
	}
	if (nState != EN_STATE_SENDOFF)
		fSendOffAcceleration = nSendOffFrames = 0;
	while (IsInCollision3D(Player->GetHitboxPlayer(PLAYER_HB_OBJECT_COL), GetHitboxEnemy(ENEMY_HB_BODY)) && nState != EN_STATE_DAMAGED)
	{
		Translate({ -sinf(XM_PI + Player->GetModel()->GetRotation().y) * 2, 0, -cosf(XM_PI + Player->GetModel()->GetRotation().y) * 2 });
		Player->Translate({ sinf(XM_PI + Player->GetModel()->GetRotation().y) * 2, 0, cosf(XM_PI + Player->GetModel()->GetRotation().y) * 2 });
	}
	switch (nState)
	{
	case EN_STATE_IDLE:
		SetAnimation(EN_IDLE, fEnemyAnimations[EN_IDLE]);
		break;
	case EN_STATE_DAMAGED:
		DamagedStateControl();
		break;
	case EN_STATE_SENDOFF:
		SendOffStateControl();
		break;
	case EN_STATE_REDHOTKICKED:
		RedHotKickedStateControl();
		break;
	default:
		break;
	}
	if (!pFloor && Model->GetCurrentAnimation() == EN_SENDTOAIR_AIRIDLE && Model->GetCurrentFrame()>=729)
		Model->SetFrameOfAnimation(729);
	else if(pFloor && Model->GetCurrentAnimation() == EN_SENDTOAIR_AIRIDLE && Model->GetCurrentFrame() < 773)
		Model->SetFrameOfAnimation(779);
	for (int i = 0; i < MAX_HIT_EFFECTS; i++)
	{
		if (pHit[i]) {
			pHit[i]->Update();
			pHit[i]->SetPosition(SumVector(Position, { -sinf(XM_PI + Model->GetRotation().y) * 10,-20,-cosf(XM_PI + Model->GetRotation().y) * 10 }));
			if (!(pHit[i]->GetUse()))
				SAFE_DELETE(pHit[i]);
		}
	}

	HeartsControl();
	GravityControl();
}

void Enemy::HeartsControl()
{
	pHearts->Update();
	static float fAccel = 0;
	Player3D* Player = (Player3D*)pPlayer;
	if (Player->GetLockedEnemy() == this)
	{
		if (fHeartPosLockOn < 40)
			fHeartPosLockOn += fAccel;
		else
		{
			fHeartPosLockOn = 40;
			fAccel = 0;
		}
		fAccel += 0.5f;
	}
	else {
		if (fHeartPosLockOn > 0)
			fHeartPosLockOn -= fAccel;
		else
		{
			fHeartPosLockOn = 0;
			fAccel = 0;
		}
		fAccel += 0.5f;
	}
}

void Enemy::RedHotKickedStateControl()
{
	Player3D* Player = (Player3D*)pPlayer;
	FaceActor(Player);
	Camera3D* pCamera = (Camera3D*)(GetMainCamera()->GetFocalPoint());
	if (Player->GetModel()->GetCurrentFrame() > 4356 && Player->GetModel()->GetCurrentFrame() < 4388)
	{
		pCamera->SetZooming(150, 40, 2, 4);
	}
	else if(Player->GetModel()->GetCurrentFrame() > 4388) {
		pCamera->SetZooming(-150, 40, 2, 4);
		nState = EN_STATE_SENDOFF;
		nSendoffAttack = RED_HOT_KICK;
	}
}

void Enemy::SendOffStateControl()
{
	nSendOffFrames++;
	switch (nSendoffAttack)
	{
	case AIR_PUNCHC:
		SetAnimation(EN_STRONG_HITFORWARD, fEnemyAnimations[EN_STRONG_HITFORWARD]);
		if (Model->GetCurrentFrame() > 1147)
			Model->SetFrameOfAnimation(1137);
		fSendOffAcceleration += 4.25f;
		Translate({ sinf(XM_PI + GetModel()->GetRotation().y) * (5 + fSendOffAcceleration), 0, cosf(XM_PI + GetModel()->GetRotation().y) * (5 + fSendOffAcceleration) });
		if (nSendOffFrames > 8)
			nState = EN_IDLE;
		break;
	case HEADBUTT:
		SetAnimation(EN_STRONG_HITFORWARD, fEnemyAnimations[EN_STRONG_HITFORWARD]);
		fSendOffAcceleration += 0.25f;
		if (Model->GetCurrentFrame() < 1195)
			Translate({ sinf(XM_PI + GetModel()->GetRotation().y) * (1.25f + fSendOffAcceleration), 0, cosf(XM_PI + GetModel()->GetRotation().y) * (1.25f + fSendOffAcceleration) });
		if (Model->GetLoops() > 0)
			nState = EN_IDLE;
		break;
	case KICKDOWN:
		SetAnimation(EN_LAUNCHED_FORWARD, fEnemyAnimations[EN_LAUNCHED_FORWARD] * 2.95f);
		fSendOffAcceleration += 0.2f;
		Translate({ sinf(XM_PI + GetModel()->GetRotation().y) * (5 + fSendOffAcceleration), 0, cosf(XM_PI + GetModel()->GetRotation().y) * (5 + fSendOffAcceleration) });
		if (Model->GetLoops() > 0)
			nState = EN_IDLE;
		break;
	default:
		SetAnimation(EN_LAUNCHED_FORWARD, fEnemyAnimations[EN_LAUNCHED_FORWARD] * 2.75f);
		fSendOffAcceleration += 0.3f;
		Translate({ sinf(XM_PI + GetModel()->GetRotation().y) * (5 + fSendOffAcceleration), 0, cosf(XM_PI + GetModel()->GetRotation().y) * (5 + fSendOffAcceleration) });
		if (Model->GetLoops() > 0)
			nState = EN_IDLE;
		break;
	}
}
void Enemy::Jump(float fJumpForce)
{
	pFloor = nullptr;
	Position.y += Hitboxes[ENEMY_HB_FEET].SizeY;
	fGravityForce = -fJumpForce;
}
void Enemy::InitialAttackedAnimation(int currentattack)
{
	switch (currentattack)
	{
	case SLIDE:
		SetAnimation(EN_KICKED_A, fEnemyAnimations[EN_KICKED_A]);
		break;
	case SLIDE_KICKUP: case UPPERCUT:
		SetAnimation(EN_SENDTOAIR_AIRIDLE, fEnemyAnimations[EN_SENDTOAIR_AIRIDLE]);
		break;
	default:
		if (!bAlternatePunchAnim)
			SetAnimation(EN_PUNCHED_A, fEnemyAnimations[EN_PUNCHED_A]);
		else
			SetAnimation(EN_PUNCHED_B, fEnemyAnimations[EN_PUNCHED_B]);
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
	if (!pPlayerAttack)
		bFollowRoulette = false;
	if (IsInCollision3D(Player->GetHitboxPlayer(PLAYER_HB_ATTACK), GetHitboxEnemy(ENEMY_HB_BODY)) && bCanBeAttacked) {
		bAlternatePunchAnim ^= true;
		if (pPlayerAttack->Animation != RED_HOT_KICK)
			FaceActor(pPlayer);
		bCanBeAttacked = false;
		CameraRumbleControl(pPlayerAttack->Animation);
		fGravityForce = 0;
		nCancellingGravityFrames = 70;
		if (!GetInput(INPUT_LOCKON) && !Player->IsSoftLocked()) {
			pPlayer->FaceActor(this);
			Player->SoftLock();
		}
		if (pPlayerAttack->Animation == ROULETTE)
			bFollowRoulette = true;
		else
			bFollowRoulette = false;
		if (pPlayerAttack->Animation == RED_HOT_KICK) {
			pPlayer->FaceActor(this);
			Player->RedHotKicked();
		}
		SetHitEffect();
	}
	if (bFollowRoulette)
	{
		Position.y = Player->GetHitboxPos(PLAYER_HB_ATTACK).y;
		Position.y -= 40;
	}
	if (!IsInCollision3D(Player->GetHitboxPlayer(PLAYER_HB_ATTACK), GetHitboxEnemy(ENEMY_HB_BODY)) || !pPlayerAttack) {
		bCanBeAttacked = true;
		return;
	}
	switch (pPlayerAttack->Animation)
	{
	case KNEEDASH:
		SetAnimation(EN_KICKED_A, fEnemyAnimations[EN_KICKED_A]);
		Player->StopAttack();
		Player->Translate({ sinf(XM_PI + Player->GetModel()->GetRotation().y) * 25, 0, cosf(XM_PI + Player->GetModel()->GetRotation().y) * 25 });
		Position = Player->GetHitboxPos(PLAYER_HB_ATTACK);
		Position.y = PosY;
		pCamera->SetZooming(-150, 60, 2, 5);
		break;
	case SLIDE:
		SetAnimation(EN_KICKED_A, fEnemyAnimations[EN_KICKED_A]);
		Player->StopAttack();
		Player->Translate({ sinf(XM_PI + Player->GetModel()->GetRotation().y) * 15, 0, cosf(XM_PI + Player->GetModel()->GetRotation().y) * 15 });
		Position = Player->GetHitboxPos(PLAYER_HB_ATTACK);
		Position.y = PosY;
		pCamera->SetZooming(-150, 60, 2, 5);
		break;
	case SLIDE_KICKUP:
		if (Player->GetModel()->GetCurrentFrame() < 630)
			break;
		SetAnimation(EN_SENDTOAIR_AIRIDLE, fEnemyAnimations[EN_SENDTOAIR_AIRIDLE]);
		Position.x += sinf(XM_PI + Model->GetRotation().y) * pPlayerAttack->ahsHitboxSize.speed *0.75f;
		Position.z += cosf(XM_PI + Model->GetRotation().y) * pPlayerAttack->ahsHitboxSize.speed *0.75f;
		Position.y = Player->GetHitboxPos(PLAYER_HB_ATTACK).y;
		pCamera->SetZooming(75, 15, 2, 5);
		fGravityForce = 0;
		nCancellingGravityFrames = 50;
		break;
	case UPPERCUT: case ROULETTE:
		if (Player->GetModel()->GetCurrentFrame() < 1813)
			break;
		SetAnimation(EN_SENDTOAIR_AIRIDLE, fEnemyAnimations[EN_SENDTOAIR_AIRIDLE]);
		Position.x += sinf(XM_PI + Model->GetRotation().y) * pPlayerAttack->ahsHitboxSize.speed *0.75f;
		Position.z += cosf(XM_PI + Model->GetRotation().y) * pPlayerAttack->ahsHitboxSize.speed *0.75f;
		Position.y = Player->GetHitboxPos(PLAYER_HB_ATTACK).y;
		Position.y -= 40;
		pCamera->SetZooming(-150, 90, 2, 5);
		break;
	case BASIC_CHAIN_B_KICKA: case BASIC_CHAIN_B_KICKB:
		if (!bAlternatePunchAnim)
			SetAnimation(EN_KICKED_A, fEnemyAnimations[EN_KICKED_A]);
		else
			SetAnimation(EN_KICKED_B, fEnemyAnimations[EN_KICKED_B]);
		if (!pFloor && IsInCollision3D(Player->GetHitboxPlayer(PLAYER_HB_ATTACK), GetHitboxEnemy(ENEMY_HB_BODY)))
			Player->Translate({ -sinf(XM_PI + Player->GetModel()->GetRotation().y) * 5, 0, -cosf(XM_PI + Player->GetModel()->GetRotation().y) * 25 });
		Position = Player->GetHitboxPos(PLAYER_HB_ATTACK);
		pCamera->SetZooming(60, 15, 2, 4);
		Position.y = PosY;
		break;
	case KICKDOWN: case BASIC_CHAIN_B_KICKB_FORWARD:
		if (pFloor) {
			if(pPlayerAttack->Animation == KICKDOWN || (pPlayerAttack->Animation == BASIC_CHAIN_B_KICKB_FORWARD && Player->GetModel()->GetCurrentFrame()>3837))
				Jump(4.25f);
			SetAnimation(EN_SENDTOAIR_AIRIDLE, fEnemyAnimations[EN_SENDTOAIR_AIRIDLE]);
			nCancellingGravityFrames = 40;
		}else if (fGravityForce >= 0) {
			Position = Player->GetHitboxPos(PLAYER_HB_ATTACK);
			Position.y -= 50;
			nCancellingGravityFrames = 0;
		}
		
		pCamera->SetZooming(-120, 15, 2, 4);
		break;

	case BASIC_CHAIN_C: case AIR_PUNCHC: case HEADBUTT: case BACKDROP_KICK: case BASIC_CHAIN_B_KICKB_PUNCH: case BASIC_CHAIN_B_KICKC: case KICK_CHAIN_C: case BUNBUN_FALL_ATK:
		if (pPlayerAttack->Animation == BASIC_CHAIN_B_KICKC && Player->GetModel()->GetCurrentFrame() < 3740)
			break;
		Position.x += sinf(XM_PI + Model->GetRotation().y) * pPlayerAttack->ahsHitboxSize.x*2;
		Position.z += cosf(XM_PI + Model->GetRotation().y) * pPlayerAttack->ahsHitboxSize.z*2;
		nState = EN_STATE_SENDOFF;
		nSendoffAttack = pPlayerAttack->Animation;
		pCamera->SetZooming(-150, 40, 2, 4);
		break;
	case RED_HOT_KICK:
		nState = EN_STATE_REDHOTKICKED;
		SetAnimation(EN_PUNCHED_A, 0);
		Player->RedHotKicked();
		return;
	default:
		if(!bAlternatePunchAnim)
			SetAnimation(EN_PUNCHED_A, fEnemyAnimations[EN_PUNCHED_A]);
		else
			SetAnimation(EN_PUNCHED_B, fEnemyAnimations[EN_PUNCHED_B]);
		if (!pFloor && IsInCollision3D(Player->GetHitboxPlayer(PLAYER_HB_ATTACK), GetHitboxEnemy(ENEMY_HB_BODY))) {
			Player->Translate({ -sinf(XM_PI + Player->GetModel()->GetRotation().y) * 10, 0, -cosf(XM_PI + Player->GetModel()->GetRotation().y) * 10 });
			Translate({ -sinf(XM_PI + Player->GetModel()->GetRotation().y) * 10, 0, -cosf(XM_PI + Player->GetModel()->GetRotation().y) * 10 });
			nCancellingGravityFrames = 40;
		}
		if (pFloor) {
			Position.x += sinf(XM_PI + Model->GetRotation().y) * pPlayerAttack->ahsHitboxSize.speed *0.75f;
			Position.z += cosf(XM_PI + Model->GetRotation().y) * pPlayerAttack->ahsHitboxSize.speed *0.75f;
		}else{
			Position.y = Player->GetHitboxPos(PLAYER_HB_ATTACK).y;
			Position.y -= 40;
		}
		pCamera->SetZooming(60, 25, 2, 4);
		if (pPlayerAttack->nAttackID == 599) {
			pCamera->SetZooming(-150, 40, 2, 4);
			nState = EN_STATE_SENDOFF;
			nSendoffAttack = HEADBUTT;
		}
		break;
	}
}

void Enemy::CameraRumbleControl(int nAttackAnim)
{
	Camera3D* pCamera = (Camera3D*)(GetMainCamera()->GetFocalPoint());
	PLAYER_ATTACK_MOVE* pAttack = ((Player3D*)pPlayer)->GetCurrentAttack();
	switch (nAttackAnim)
	{
	case SLIDE: case UPPERCUT:
		pCamera->SetShaking(8.0f, 7, 2);
		break;
	case BASIC_CHAIN_C: case AIR_PUNCHC: case HEADBUTT: case BACKDROP_KICK: case BASIC_CHAIN_B_KICKB_PUNCH: case BASIC_CHAIN_B_KICKC: case KICK_CHAIN_C:
		pCamera->SetShaking(10.0f, 7, 2);
		break;
	default:
		pCamera->SetShaking(6.0f, 7, 2);
		if (pAttack && pAttack->nAttackID == 599)
			pCamera->SetShaking(10.0f, 14, 2);
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
		
		if (--nCancellingGravityFrames > 0)
			fGravityForce += GRAVITY_FORCE*0.005f;
		else {
			fGravityForce += GRAVITY_FORCE;
			nCancellingGravityFrames = 0;
		}
		Position.y -= fGravityForce;
		
		pFloor = ((S_InGame3D*)pGame)->GetList(GO_FLOOR)->CheckCollision(GetHitboxEnemy(ENEMY_HB_FEET));
		if (pFloor)
		{
			while (IsInCollision3D(pFloor->GetHitbox(), GetHitboxEnemy(ENEMY_HB_FEET)))
				Position.y += 0.1f;
			Position.y -= 0.1f;
			Player3D* Player = (Player3D*)pPlayer;
			PLAYER_ATTACK_MOVE* pAtk = Player->GetCurrentAttack();
			if (IsInCollision3D(Player->GetHitboxPlayer(PLAYER_HB_ATTACK), GetHitboxEnemy(ENEMY_HB_BODY)) && pAtk && pAtk->Animation == KICKDOWN)
			{
				Position.x += sinf(XM_PI + Model->GetRotation().y) * pAtk->ahsHitboxSize.x * 5;
				Position.z += cosf(XM_PI + Model->GetRotation().y) * pAtk->ahsHitboxSize.z * 5;
				nState = EN_STATE_SENDOFF;
				nSendoffAttack = pAtk->Animation;
				Camera3D* pCamera = (Camera3D*)(GetMainCamera()->GetFocalPoint());
				pCamera->SetZooming(-150, 40, 2, 4);
			}
		}
	}
}

void Enemy::Draw()
{
	SetCullMode(CULLMODE_NONE);
	for (int i = 0; i < MAX_HIT_EFFECTS; i++)
	{
		if (pHit[i])
			pHit[i]->Draw();
	}
	SetCullMode(CULLMODE_CW);
	Actor::Draw();
	SetCullMode(CULLMODE_NONE);
	DrawHearts();
	SetCullMode(CULLMODE_CW);
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

void Enemy::DrawHearts()
{
	if (fHeartPosLockOn == 0)
		return;
	XMFLOAT3 RotateAround = Model->GetRotation();
	//RotateAround = ((Camera3D*)(GetMainCamera()->GetFocalPoint()))->GetRotation();
	for (int i = 0; i < MAX_ENEMY_HEART; i++)
	{
		float theta = ((float)i*(360.0f / (float)MAX_ENEMY_HEART)) * 3.142f / 180;
		float c1 = 1.0f*cos(theta) - 1.0*sin(theta);
		float c2 = 1.0f*cos(theta) + 1.0*sin(theta);

		XMFLOAT3 PositionA = SumVector(Position, { -sinf(XM_PI + RotateAround.y) * 50,0,-cosf(XM_PI + RotateAround.y) * 50 });
		XMFLOAT3 Pos =
			SumVector(PositionA,
				{ -sinf(XM_PI + RotateAround.y + XM_PI * 0.5f) *(c1*(fHeartPosLockOn - fHeartPosHealth[i]) - (c1*0.175f)),(c2*(fHeartPosLockOn - fHeartPosHealth[i])) + 60,-cosf(XM_PI + RotateAround.y + XM_PI * 0.5f) * (c1*(fHeartPosLockOn - fHeartPosHealth[i]) - (c1*0.175f)) });

		pHearts->SetPosition(Pos);
		pHearts->Draw();
	}
}

void Enemy::End()
{
	for (int i = 0; i < MAX_HIT_EFFECTS; i++)
		SAFE_DELETE(pHit[i]);
	SAFE_DELETE(pHearts);

}

Box Enemy::GetHitboxEnemy(int hb)
{
	return { Hitboxes[hb].PositionX + Position.x, Hitboxes[hb].PositionY + Position.y,Hitboxes[hb].PositionZ + Position.z, Hitboxes[hb].SizeX,Hitboxes[hb].SizeY,Hitboxes[hb].SizeZ };
}