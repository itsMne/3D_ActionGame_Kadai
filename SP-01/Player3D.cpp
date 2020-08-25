//*****************************************************************************
// Player3D.cpp
// プレイヤーを管理する
// Author : Mane
//*****************************************************************************
#include "Player3D.h"
#include "InputManager.h"
#include "debugproc.h"
#include "Texture.h"
#include "Sound.h"
#include "S_InGame3D.h"
#include "Enemy.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define PLAYER_MODEL_PATH "data/model/Hana.fbx"
#define MAX_ATTACKS 25
#define MAX_INPUT_TIMER 27
#define MAX_WAIT_TIMER 13
#define DEBUG_FOUND_ATTACKS false
#define VERIFY_ISONFLOOR true
#define DEBUG_ADD_INPUTS false
#define MAX_SPEED 6.8f
#define GRAVITY_FORCE 0.98f*2
#define SHOW_PLAYER_HITBOX false
#define SHOW_SPECIFIC_PLAYER_HITBOX PLAYER_HB_TAUNT
#define JUMP_FORCE 20*1.34f
#define DEBUG_DIRECTIONALS false
#define DEBUG_WAITFRAME false
#define KICKDOWN_SPEED 10.98
#define LOCK_ON_DISTANCE 550.0f
#define BACKWARD_INPUT_OFFSET 1.75f
#define FORWARD_INPUT_OFFSET 1.45f

//*****************************************************************************
// グローバル変数
//*****************************************************************************

Player3D* pMainPlayer3D = nullptr;
PLAYER_ATTACK_MOVE stAllMoves[MAX_ATTACKS] =
{
	{"A",	  BASIC_CHAIN_A,				false,	GROUND_MOVE, BASIC_CHAIN_B,	 1123,	{ 1122, 1133}	,{15, 20, 10, 5},		1,	4,	100},
	{"n",	  BASIC_CHAIN_B,				false,	GROUND_MOVE, BASIC_CHAIN_C,	 1204,	{ 1200, 1220}	,{15, 20, 10, 5},		1,	4,	200},//AA
	{"n",	  BASIC_CHAIN_C,				true,	GROUND_MOVE, MAX_ANIMATIONS, -1,	{ 1300, 1320}	,{15, 20, 10, 7},		10,	7,	300},//AAA
	{"BBK",	  UPPERCUT,						true,	GROUND_MOVE, MAX_ANIMATIONS, -1,	{ 1813, 1837}	,{15, 30, 30,  5},		7,	6,	400},//BFA
	{"FFA",	  HEADBUTT,						true,	GROUND_MOVE, MAX_ANIMATIONS, -1,	{ 2866, 2914}	,{15, 20, 60,  8},		7,	6,	400},//BFA
	{"BFA",	  BACKDROP_KICK,				true,	GROUND_MOVE, MAX_ANIMATIONS, -1,	{ 2683, 2693}	,{30, 30, 60, 30},		10,	6,	400},//BFK
	{"n",	  SLIDE,						true,	GROUND_MOVE, MAX_ANIMATIONS, 482,	{ 467,  512}	,{15, 30, 30,  0},		5,	5,	400},//BFK
	{"N",	  BASIC_CHAIN_B_PAUSEA,			true,	GROUND_MOVE, MAX_ANIMATIONS, -1,	{ 0, 0}			,{15, 20, 10,  0},		2,	4,	500},//AAPA
	{"K",	  KICK_CHAIN_A,					false,	GROUND_MOVE, KICK_CHAIN_B,	 2130,	{ 2140, 2162}	,{15, 20, 10,  3},		2,	4,	600},//K
	{"n",	  KICK_CHAIN_B,					false,	GROUND_MOVE, BASIC_CHAIN_C,	 2270,	{ 2276, 2292}	,{15, 20, 10,  3},		2,	4,	700},//KK
	{"n",	  KICK_CHAIN_C,					true,	GROUND_MOVE, MAX_ANIMATIONS, -1,	{ 2995, 3028}	,{15, 20, 10, 10},		10,	7,	800},//KKK
	{"n",	  SLIDE_KICKUP,					true,	GROUND_MOVE, MAX_ANIMATIONS, -1,	{ 633, 720}		,{15, 20, 15,  0},		7,	6,	900},//
	{"n",	  BASIC_CHAIN_B_KICKA,			true,	GROUND_MOVE, MAX_ANIMATIONS, 3515,	{ 3505, 3556}	,{15, 20, 10,  4},		3,	5,	1000},//AAK
	{"n",     BASIC_CHAIN_B_KICKB,			true,	GROUND_MOVE, MAX_ANIMATIONS, 3616,	{ 3606, 3652}	,{15, 20, 10,  4},		3,	5,	1100},//AAKK
	{"n",     BASIC_CHAIN_B_KICKC,			true,	GROUND_MOVE, MAX_ANIMATIONS, -1,	{ 3740, 3765}	,{15, 20, 10,  4},		8,	6,	1200},//AAKK
	{"n",     BASIC_CHAIN_B_KICKB_PUNCH,	true,	GROUND_MOVE, MAX_ANIMATIONS, -1,	{ 3934, 3973}	,{15, 20, 10,  9},		10,	7,	1300},//AAKFK
	{"n",	  BASIC_CHAIN_B_KICKB_FORWARD,	true,	GROUND_MOVE, MAX_ANIMATIONS, -1,	{ 3835, 3843}	,{15, 20, 10,  2},		10,	7,	1400},//AAKA
	{"A",	  AIR_PUNCHA,					false,	AIR_MOVE,	 AIR_PUNCHB,	 3100,	{ 3100, 3113}	,{15, 20, 10,  0},		1,	4,	1500},//A
	{"n",	  AIR_PUNCHB,					false,	AIR_MOVE,	 AIR_PUNCHC,	 3175,	{ 3167, 3191}	,{15, 20, 10,  0},		1,	4,	1600},//A
	{"n",	  AIR_PUNCHC,					true,	AIR_MOVE,	 MAX_ANIMATIONS, -1,	{ 3272, 3290}	,{15, 20, 10,  0},		10,	7,	1700},//A
	{"FFA",	  KNEEDASH,						true,	AIR_MOVE,	 MAX_ANIMATIONS, -1,	{ 3423, 3461}	,{40, 30, 20,  0},		7,	6,	1800},//A
	{"N",	  ROULETTE,						true,	AIR_MOVE,	 MAX_ANIMATIONS, -1,	{ 3337, 3395}	,{15, 20, 10,  0},		4,	5,	1900},//A
	{"K",	  KICKDOWN,						true,	AIR_MOVE,	 MAX_ANIMATIONS, -1,	{ 4157, 4200}	,{15, 20, 30,  0},		7,	6,	1900},//K
	{"n",	  RED_HOT_KICK,					true,	AIR_MOVE,	 MAX_ANIMATIONS, -1,	{ 4283, 4338}	,{15, 20, 30,  0},		13,	9,	2000},//K
	{"n",	  BUNBUN_FALL_ATK,				true,	AIR_MOVE,	 MAX_ANIMATIONS, -1,	{ 4564, 4638}	,{120, 30, 120,0},	13,	9,	2000},//K
};

float fAnimationSpeed[] =
{
	2*1,
	2*2.5f,
	2*2.5f,//ANIMATION_WALKING
	2*2,//SLIDE
	2*0.5f,//SLIDE_STANDUP_FIGHT
	2*1,//SLIDE_STANDUP_IDLE
	2*1,//SLIDE_KICKUP
	2*1,//AIRIDLE
	2*2,//JUMP_UP
	2*1.5f,//JUMP_UP_TO_FALLDOWN
	2*1,//FIGHT_STANCE
	2*1.45f,//BASIC_CHAIN_A
	2*1.45f,//BASIC_CHAIN_B
	2*1.05f,//BASIC_CHAIN_C
	2*1.75f,//RUN_TO_IDLE
	2*1.0f,//	DOWN_DODGE,
	2*1.0f,//	DOWN_DODGE_TO_SLIDE,
	2*0.65f,//DOWN_DODGE_TO_IDLE,
	2*1.2f,//	DODGE_ROLL,
	2*1.0f,//	UPPERCUT,
	2*1.0f,//	BASIC_CHAIN_B_PAUSEA,
	2*1.25f,//KICK_CHAIN_A,
	2*1.25f,//KICK_CHAIN_B,
	2*0.5f,//BACKWARD,
	2*2.5f,//BACKWARDTOFORWARD,
	2*1.0f,//BACKDROP_KICK,
	2*1.25f,//HEADBUTT,
	2*1.25f,//KICK_CHAIN_C,
	2*1.45f,//AIR_PUNCHA,
	2*1.45f,//AIR_PUNCHB,
	2*1.15f,//AIR_PUNCHC,
	2*1.0f,//ROULETTE,
	2*1.5f,//KNEEKICK,
	2*0.8f,//BASIC_CHAIN_B_KICKA,
	2*0.8f,//BASIC_CHAIN_B_KICKB,
	2*0.75f,//BASIC_CHAIN_B_KICKC,
	2*0.5f,//BASIC_CHAIN_B_KICKB_FORWARD,
	2*0.5f,//BASIC_CHAIN_B_KICKB_PUNCH,
	2*0.5f,//BUNBUN_FLOAT,
	2*1.5f,//KICKDOWN,
	2*1.0f,//RED HOT KICK,
	2*0.75f,//BUNBUN_FALL_ATK,
	2*0.8f,//TAUNT_A
	2*0.65f,//PLAYER_DAMAGE
	2*0.65f,//PLAYER_DEATH
};

Player3D* pCurrentPlayer = nullptr;
bool bIsSoftLockOn = true;
//*****************************************************************************
// コンストラクタ関数
//*****************************************************************************
Player3D::Player3D() : Actor(PLAYER_MODEL, A_PLAYER)
{
	Init();
	bKick = false;
	bPunch = false;
	bLockingOn = false;
	bRHK_Hit = false;
	bAllStaminaUsed = false;
	fGravityForce = 0;
	pCurrentPlayer = this;
	Model->SetScale({ 0.5f, 0.5f, 0.5f });
	fPlayerSpeed = 1;
	nInputTimer = 0;
	pLight = GetMainLight();
	nState = PLAYER_IDLE_STATE;
	pLockedEnemy = nullptr;
	BunBun = new Object3D(GO_BUNBUN);
	BunBun->SetParent(this);
	BunBun->SetPosition({ 0, -100, 0 });
	BunBun->SetAnimation(BUN_BUN_APPEARS, 0.5f);
	ChuSign = new Object3D(GO_CHU);
	ChuSign->SetParent(this);
	ChuSign->SetScale({0.25f,0.25f,0.25f});
	ChuSign->SetPosition({ 30, 80, 0 });
	pDodgedEnemy = nullptr;
	pCurrentAttackPlaying = nullptr;
	pPreviousAttack = nullptr;
	nFightStanceFrameCount = 0;
	eDirection = DIR_NONE;
	nCancellingGravityFrames = 0;
	bUppercutExecute = bPressedForwardMidAttack = bPressedBackwardMidAttack = false;
	pCamera = new Camera3D(CAMERA_PLAYER);
	pCamera->SetObjectToFollow(this);
	pCamera->SetZoomOut(-75);
	ResetInputs();
	nHP = MAX_HEALTH;
	pGame = nullptr;
	bFirstSetOfPunches = false;
	pBattery = new Billboard(BB_BATTERY, { 1,1 });
	pBattery->SetUnusableAfterAnimation(false);
	pBattery->SetUse(true);
	pBatteryEnergy = new Billboard(BB_BATTERY_ENERGY, { 1,1 });
	pBatteryEnergy->SetUnusableAfterAnimation(false);
	pBatteryEnergy->SetUse(true);
	pBatteryWasted = new Billboard(BB_BATTERY, { 1,1 }, { 0.5f,0.5f,0.5f,1 });
	pBatteryWasted->SetUnusableAfterAnimation(false);
	pBatteryWasted->SetUse(true);
	nStamina = MAX_PLAYER_STAMINA;
	pDodgeSkybox = new Sphere3D("data/texture/SkyboxDodge.tga", false);
	pDodgeSkybox->SetScale({ 0.0f,0.0f,0.0f });
	pDodgeSkybox->SetRotSpeed({ 0.0f ,0.1f ,0.0f });
}

Player3D::~Player3D()
{
	End();
	Actor::~Actor();
}

//*****************************************************************************
//Init関数
//初期化関数
//引数：void
//戻：void
//*****************************************************************************
void Player3D::Init()
{
	for (int i = 0; i < PLAYER_HB_MAX; i++)
	{
		pVisualHitboxes[i] = nullptr;
		Hitboxes[i] = { 0 };
	}
	Hitboxes[PLAYER_HB_FEET] = { 0, -5.0f, 0, 17.0f, 15.0f, 17.0f };
	Hitboxes[PLAYER_HB_BODY] = { 0, 25.0f, 0, 20.0f, 40.0f, 20.0f };
	Hitboxes[PLAYER_HB_DODGE] = { 0, 25.0f, 0, 20.0f*3, 40.0f, 20.0f*3 };
	Hitboxes[PLAYER_HB_ATTACK] = { 0, 25.0f, 0, 15.0f, 20.0f, 10.0f };
	Hitboxes[PLAYER_HB_OBJECT_COL] = { 0, 25.0f, 0, 15.0f, 20.0f, 10.0f };
	Hitboxes[PLAYER_HB_LOCKON] = { 0, 25.0f, 20, 60.0f, 60.0f, 60.0f };
	Hitboxes[PLAYER_HB_TAUNT] = { 0, 0.0f, 0, 0.0f, 0.0f, 0.0f };
	for (int i = 0; i < MAX_ENEMIES_FOLLOWING_PLAYER; pEnemiesFollowingPlayer[i] = nullptr, i++);
	bDodged = false;
#if SHOW_HITBOX && SHOW_PLAYER_HITBOX
	for (int i = 0; i < PLAYER_HB_MAX; i++)
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

//*****************************************************************************
//Update関数
//変更関数
//引数：void
//戻：void
//*****************************************************************************
void Player3D::Update()
{
	Actor::Update();
	pCamera->Update();
	DodgeSkyboxControl();
	if (Position.y <= GetCurrentBottom() && nState != PLAYER_TELEPORTING_STATE) {
		nState = PLAYER_TELEPORTING_STATE;
		x3LastSafePos.y += 35.0f;
		fAcceleration = 0;
	}
	if (nHP <= 0)
	{
		pLockedEnemy = nullptr;
		Model->SetLoop(false);
		GravityControl();
		SetAnimation(PLAYER_DEATH, fAnimationSpeed[PLAYER_DEATH]);
		return;
	}
	if(DamagedStateControl())
		return;
	if (bDodged && pDodgedEnemy)
	{
		XMFLOAT3 Rot = pCamera->GetRotation();
		pCamera->SetZooming(-140, 40, 2, 4);
		Rot.y = GetFaceActorRotation(pDodgedEnemy);
		pCamera->SetNewTargetRotation(Rot);
		if (((Enemy*)pDodgedEnemy)->GetState() != EN_ATTACKING) {
			bDodged = false;
			pDodgedEnemy = nullptr;
		}
		//dodge reward
	}
	bSoftLocking = false;
	if (!pGame) {
		pGame = GetCurrentGame();
		if (!pGame)
			return;
	}
	if (IsOnTheFloor() && Model->GetCurrentAnimation()!=SLIDE_KICKUP && nState != PLAYER_DODGING_STATE && nState != PLAYER_DODGING_RECOVERY_STATE
		&& nState != PLAYER_TELEPORTING_STATE)
	{
		if (GetInput(INPUT_JUMP))
			Jump();
	}
	if (pFloor)
		bRouletteExecuted = bUppercutExecute = false;
	if (nHP < 0)
		nHP = 0;
	if (nHP > MAX_HEALTH)
		nHP = MAX_HEALTH;
	CalculateDirectionalInput();
	if (nState != PLAYER_IDLE_FIGHT_STATE)
		nFightStanceFrameCount = 0;

	SetHitboxPosition(-sinf(XM_PI + Model->GetRotation().y) * 10, 25.0f, -cosf(XM_PI + Model->GetRotation().y) * 10, PLAYER_HB_OBJECT_COL);
	SetHitboxPosition(-sinf(XM_PI + Model->GetRotation().y) * 45,25.0f,-cosf(XM_PI + Model->GetRotation().y) * 45, PLAYER_HB_ATTACK);
	
	ActivateAttackHitbox(0, 0, 0, 0);

	bKick = GetInput(INPUT_ATTACK) && CheckHoldingBack();
	bPunch = GetInput(INPUT_ATTACK) && !CheckHoldingBack();
	bLockingOn = GetInput(INPUT_LOCKON)|| (pLockedEnemy && bIsSoftLockOn);
	//ロックオン確認する
	LockingControl();
	if (nState != PLAYER_ATTACKING_STATE && nState != PLAYER_IDLE_FIGHT_STATE)
		bFirstSetOfPunches = false;
	//避ける
	if (GetInput(INPUT_DODGE) && IsOnTheFloor() && (abs(GetAxis(MOVEMENT_AXIS_HORIZONTAL)) > 0.35f || abs(GetAxis(MOVEMENT_AXIS_VERTICAL)) > 0.35f) 
		&& Model->GetCurrentAnimation()!=DODGE_ROLL && !bAllStaminaUsed && nState != PLAYER_TELEPORTING_STATE)
	{
		nState = PLAYER_DODGING_STATE;
		float fHorizontalAxis = GetAxis(MOVEMENT_AXIS_HORIZONTAL);
		float fVerticalAxis = GetAxis(MOVEMENT_AXIS_VERTICAL);
		float nModelRotation = -(atan2(fVerticalAxis, fHorizontalAxis) - 1.570796f);
		Model->SetRotationY(nModelRotation + Rotation.y + pCamera->GetRotation().y);
	}
	else if(GetInput(INPUT_DODGE) && abs(GetAxis(MOVEMENT_AXIS_HORIZONTAL)) < 0.35f && abs(GetAxis(MOVEMENT_AXIS_VERTICAL)) < 0.35f && nState != PLAYER_TAUNTING_STATE
		&& nState != PLAYER_ATTACKING_STATE)
	{
		if(IsOnTheFloor())
			nState = PLAYER_TAUNTING_STATE;
	}

	ChuSign->GetModel()->SetRotation(SumVector(pCamera->GetRotation(), {0,-XM_PI*0.5f,0}));
	if(nState!= PLAYER_TAUNTING_STATE)
		ChuSign->GetModel()->SwitchAnimation(1, 0, 2.05f);
	ChuSign->SetPosition({ -sinf(-XM_PI/2 + pCamera->GetRotation().y) * 30, 80, -cosf(-XM_PI/2 + pCamera->GetRotation().y) * 30 });
	Hitboxes[PLAYER_HB_TAUNT] = { 0, 0.0f, 0, 0.0f, 0.0f, 0.0f };
	
	if (pFloor && nState != PLAYER_TELEPORTING_STATE)
	{
		Box hb = pFloor->GetHitbox();
		float hbflooroffset = 70;
		hb.SizeX -= hbflooroffset;
		hb.SizeZ -= hbflooroffset;
		if(IsInCollision3D(hb, GetHitboxPlayer(ENEMY_HB_FEET)))
			x3LastSafePos = Position;
	}
	//ステートマシン
	switch (nState)
	{
	case PLAYER_IDLE_STATE:
		GravityControl();
		IdleStateControl();
		break;
	case PLAYER_IDLE_FIGHT_STATE:
		GravityControl();
		FightingStanceStateControl();
		break;
	case PLAYER_TELEPORTING_STATE:
		SetAnimation(BUNBUN_FALL_ATK, fAnimationSpeed[BUNBUN_FALL_ATK]);
		if (Model->GetCurrentFrame() > 4620)
			Model->SetFrameOfAnimation(4580);

		fAcceleration++;
		if (MoveToPos(fAcceleration, x3LastSafePos))
		{
			if (Scale.x != 1)
			{
				SetScaling(fAcceleration*0.005f);
				if (Scale.x >= 1) {
					Scale = { 1,1,1 };
					fAcceleration = 0;
					fGravityForce = 0;
					Damage();
					nState = PLAYER_BUNBUN_FALLING;
					break;
				}
			}
		}
		else {
			if (Scale.x != 0)
			{
				SetScaling(-fAcceleration * 0.005f);
				if (Scale.x <= 0) {
					Scale = { 0,0,0 };
					fAcceleration = 0;
					ActivateDamageEffect();
				}
			}
		}

		break;
	case PLAYER_TAUNTING_STATE:
		SetAnimation(TAUNT_A, fAnimationSpeed[TAUNT_A]);
		if(Model->GetCurrentFrame()>=4768)
			ChuSign->GetModel()->SwitchAnimation(0, 0, 8.05f);
		if (GetAxis(MOVEMENT_AXIS_HORIZONTAL) || GetAxis(MOVEMENT_AXIS_VERTICAL))
			nState = PLAYER_MOVING_STATE;
		AttackInputsControl();
		if (Model->GetCurrentFrame() >= 4817)
			nState = PLAYER_IDLE_STATE;
		if (Model->GetCurrentAnimation() == TAUNT_A && Model->GetCurrentFrame() >= 4765) {
			Hitboxes[PLAYER_HB_TAUNT] = { -sinf(XM_PI + Model->GetRotation().y) * 220, 25.0f, -cosf(XM_PI + Model->GetRotation().y) * 220, 150.0f, 60.0f, 200.0f };
			if (pLockedEnemy)
				((Enemy*)pLockedEnemy)->Enrage(600);
		}
		break;
	case PLAYER_MOVING_STATE:
		GravityControl();
		MoveControl();
		AttackInputsControl();
		break;
	case PLAYER_ATTACKING_STATE:
		AttackStateControl();
		break;
	case PLAYER_JUMPING_UP_STATE:
		JumpingStateControl();
		break;
	case PLAYER_BUNBUN_FLOATING:
		if (Model->GetCurrentFrame() >= 4098)
			Model->SetFrameOfAnimation(4060);
		if (Model->GetCurrentFrame() >= 4040)
			BunBun->Update();
		nStamina--;
		MoveControl();
		GravityControl();
		AttackInputsControl();
		if (pFloor)
			nState = PLAYER_IDLE_STATE;
		if (!GetInput(INPUT_JUMP_HOLD)) 
			nState = PLAYER_IDLE_STATE;
		if(nStamina<=0)
			nState = PLAYER_BUNBUN_FALLING;
		break;
	case PLAYER_DODGING_STATE:
		pCurrentAttackPlaying = pPreviousAttack = nullptr;
		SetAnimation(DODGE_ROLL, fAnimationSpeed[DODGE_ROLL]);
		nStamina--;
		if (Model->GetCurrentFrame() >= 1730 && nStamina > 0)
		{
			nState = PLAYER_IDLE_STATE;
		}
		if (Model->GetCurrentFrame() <= 1721)
		{
			Position.x -= sinf(XM_PI + Model->GetRotation().y) * 9.5f;
			Position.z -= cosf(XM_PI + Model->GetRotation().y) * 9.5f;
		}
		if (Model->GetCurrentFrame() >= 1758)
			nState = PLAYER_DODGING_RECOVERY_STATE;
		break;
	case PLAYER_DODGING_RECOVERY_STATE:
		SetAnimation(DOWN_DODGE_TO_IDLE, fAnimationSpeed[DOWN_DODGE_TO_IDLE]);
		if (Model->GetCurrentFrame() >= 1658)
		{
			nState = PLAYER_IDLE_STATE;
		}
		break;
	case PLAYER_BUNBUN_FALLING:
		BunBun->Update();
		SetAnimation(BUNBUN_FALL_ATK, fAnimationSpeed[BUNBUN_FALL_ATK]);
		GravityControl();
		if (!pFloor)
		{
			if (Model->GetCurrentFrame() > 4620)
				Model->SetFrameOfAnimation(4580);
		}
		else {
			if (Model->GetCurrentFrame() < 4620)
				Model->SetFrameOfAnimation(4621);
		}
		pCurrentAttackPlaying = &stAllMoves[MAX_ATTACKS - 1];
		SetHitboxPosition(0, 25.0f, 0, PLAYER_HB_ATTACK);
		if (Model->GetCurrentFrame() > pCurrentAttackPlaying->fpHitBoxActivation.InitialFrame && Model->GetCurrentFrame() < pCurrentAttackPlaying->fpHitBoxActivation.EndFrame)
			ActivateAttackHitbox(abs(-sinf(XM_PI + Model->GetRotation().y)*pCurrentAttackPlaying->ahsHitboxSize.x), abs(pCurrentAttackPlaying->ahsHitboxSize.y), abs(-cosf(XM_PI + Model->GetRotation().y)*pCurrentAttackPlaying->ahsHitboxSize.z), pCurrentAttackPlaying->ahsHitboxSize.speed);
		if (Model->GetCurrentFrame() > 4717)
		{
			pPreviousAttack = &stAllMoves[MAX_ATTACKS - 1];
			pCurrentAttackPlaying = nullptr;
			nState = PLAYER_IDLE_FIGHT_STATE;
			SetAnimation(FIGHT_STANCE, fAnimationSpeed[FIGHT_STANCE]);
		}
		break;
	default:
		if (!Model->GetLoops())
			printf("%d\n", Model->GetCurrentFrame());
		break;
	}
	InputResetControl();
	TransitionToFloatingBunBun();
	StaminaControl();
	//printf("STATE: %d FRAME: %d\n", nState, Model->GetCurrentFrame());
}

void Player3D::DodgeSkyboxControl()
{
	pDodgeSkybox->Update();
	pDodgeSkybox->SetPosition(Position);
	static float fAcc = 0;
	fAcc += 0.005f;
	if (GetKeyPress(VK_B))
	{
		if (pDodgeSkybox->GetScaleX() != 2.0f)
			pDodgeSkybox->ScaleUP({ fAcc,fAcc,fAcc });
		if (pDodgeSkybox->GetScaleX() >= 2.1f) {
			pDodgeSkybox->SetScale({ 2.1f, 2.1f, 2.1f });
			fAcc = 0;
		}
	}
	else
	{
		if(pDodgeSkybox->GetScaleX()!=0)
			pDodgeSkybox->ScaleUP({ -fAcc,-fAcc,-fAcc });
		if (pDodgeSkybox->GetScaleX() <= 0) {
			fAcc = 0;
			pDodgeSkybox->SetScale({ 0, 0, 0 });
		}
	}
}

bool Player3D::DamagedStateControl()
{
	if (nState == PLAYER_DAMAGED_STATE)
	{
		SetAnimation(PLAYER_DAMAGE, fAnimationSpeed[PLAYER_DAMAGE]);
		Position.x += sinf(XM_PI + Model->GetRotation().y) * 5;
		Position.z += cosf(XM_PI + Model->GetRotation().y) * 5;
		if (!IsOnTheFloor())
		{
			GravityControl();
			if (Model->GetCurrentFrame() > 4855)
				Model->SetFrameOfAnimation(4855);
		}
		if (Model->GetCurrentFrame() >= 4878)
			nState = PLAYER_IDLE_STATE;
		else 
			return true;
	}
	return false;
}

void Player3D::StaminaControl()
{
	pBattery->Update();
	pBattery->SetPosition(SumVector({ sinf(XM_PI*1.5f + pCamera->GetRotation().y) * 30, 10.0f, cosf(XM_PI*1.5f + pCamera->GetRotation().y) * 30 }, Position));
	pBatteryEnergy->Update();
	pBatteryEnergy->SetPosition(SumVector({ sinf(XM_PI*1.5f + pCamera->GetRotation().y) * 30, 10.0f, cosf(XM_PI*1.5f + pCamera->GetRotation().y) * 30 }, Position));
	pBatteryWasted->Update();
	pBatteryWasted->SetPosition(SumVector({ sinf(XM_PI*1.5f + pCamera->GetRotation().y) * 30, 10.0f, cosf(XM_PI*1.5f + pCamera->GetRotation().y) * 30 }, Position));

	if (nStamina < 0) {
		nStamina = 0;
	}
	if(nStamina == 0)
		bAllStaminaUsed = true;
	if (nStamina > MAX_PLAYER_STAMINA) {
		nStamina = MAX_PLAYER_STAMINA;
	}
	if(nStamina == MAX_PLAYER_STAMINA)
		bAllStaminaUsed = false;
	if (nStamina < MAX_PLAYER_STAMINA && nState!= PLAYER_BUNBUN_FLOATING)
	{
		static int nFrameStaminaRecovery = 0;
		if (++nFrameStaminaRecovery > 30)
		{
			nStamina++;
			nFrameStaminaRecovery = 0;
		}
	}
	static const float fInitialWidthBattery =  pBatteryEnergy->GetScaleWidth();
	static const float fInitialHeightBattery = pBatteryEnergy->GetScaleHeight();
	pBatteryEnergy->SetScale(fInitialHeightBattery*((float)nStamina / (float)MAX_PLAYER_STAMINA), fInitialWidthBattery);
}
void Player3D::Jump()
{
	pFloor = nullptr;
	Position.y += Hitboxes[PLAYER_HB_FEET].SizeY;
	fGravityForce = -JUMP_FORCE;
	nCancellingGravityFrames = 0;
	pCurrentAttackPlaying = nullptr;
	nState = PLAYER_JUMPING_UP_STATE;
}

void Player3D::LockingControl()
{
	if ((!pLockedEnemy && bLockingOn && !bIsSoftLockOn) || (bIsSoftLockOn && !pLockedEnemy &&  GetInput(INPUT_CAMERA)))
	{
		float fDistance = 0;
		while (!pLockedEnemy && (fDistance += 0.1f) <LOCK_ON_DISTANCE)
		{
			Hitboxes[PLAYER_HB_LOCKON].PositionX = -sinf(XM_PI + Model->GetRotation().y) * fDistance;
			Hitboxes[PLAYER_HB_LOCKON].PositionY = 25.0f;
			Hitboxes[PLAYER_HB_LOCKON].PositionZ = -cosf(XM_PI + Model->GetRotation().y) * fDistance;
			pLockedEnemy = (Actor*)(((S_InGame3D*)pGame)->GetList(GO_ENEMY)->CheckCollision(GetHitboxPlayer(PLAYER_HB_LOCKON)));
		}
		if (bIsSoftLockOn && pLockedEnemy)
			return;
	}
	if ((!bLockingOn && !bIsSoftLockOn) || (bIsSoftLockOn && pLockedEnemy && GetInput(INPUT_CAMERA)))
		pLockedEnemy = nullptr;
	else {
		if (pLockedEnemy && nState != PLAYER_DODGING_STATE && nState != PLAYER_DODGING_RECOVERY_STATE)
		{
			XMFLOAT3 EnemyPos = pLockedEnemy->GetPosition();
			XMFLOAT3 PlayerPos = GetPosition();
			PlayerPos.y = 0;
			EnemyPos.y = 0;
			float dis = GetDistance(PlayerPos, EnemyPos)/400;
			if (dis<0.15f && pCurrentAttackPlaying && (pCurrentAttackPlaying->Animation == SLIDE || pCurrentAttackPlaying->Animation == KNEEDASH || pCurrentAttackPlaying->Animation == UPPERCUT))
				return;
			FaceActor(pLockedEnemy);
			if (dis > 1.85f)
				pLockedEnemy = nullptr;
		}
	}
}

bool Player3D::CheckHoldingBack()
{
	if (!bLockingOn)
		return false;
	bool IsHoldingBack = false;
	float fHorizontalAxis = GetAxis(MOVEMENT_AXIS_HORIZONTAL);
	float fVerticalAxis = GetAxis(MOVEMENT_AXIS_VERTICAL);
	if (fHorizontalAxis == 0 && fVerticalAxis == 0)
		return false;
	float nModelRotation = -(atan2(fVerticalAxis, fHorizontalAxis) - 1.570796f);
	XMFLOAT3 x3CurrentModelRot = Model->GetRotation();
	float DirInput = abs((nModelRotation + pCamera->GetRotation(false).y) - x3CurrentModelRot.y);
	if (DirInput > XM_PI) {
		DirInput -= XM_2PI;
		DirInput = abs(DirInput);
	}
	if (DirInput >= BACKWARD_INPUT_OFFSET)
		IsHoldingBack = true;
	return IsHoldingBack;
}

bool Player3D::CheckHoldingForward()
{
	if (!bLockingOn)
		return false;
	bool IsHoldingFor = false;
	float fHorizontalAxis = GetAxis(MOVEMENT_AXIS_HORIZONTAL);
	float fVerticalAxis = GetAxis(MOVEMENT_AXIS_VERTICAL);
	if (fHorizontalAxis == 0 && fVerticalAxis == 0)
		return false;
	float nModelRotation = -(atan2(fVerticalAxis, fHorizontalAxis) - 1.570796f);
	XMFLOAT3 x3CurrentModelRot = Model->GetRotation();
	float DirInput = abs((nModelRotation + pCamera->GetRotation(false).y) - x3CurrentModelRot.y);
	if (DirInput > XM_PI) {
		DirInput -= XM_2PI;
		DirInput =abs(DirInput);
	}
	if (DirInput <= FORWARD_INPUT_OFFSET)
		IsHoldingFor = true;
	return IsHoldingFor;
}

void Player3D::TransitionToFloatingBunBun()
{
	if (bAllStaminaUsed)
		return;
	if (GetInput(INPUT_JUMP_HOLD) && Model->GetCurrentAnimation() == AIR_IDLE ) {
		SetAnimation(BUNBUN_FLOAT, fAnimationSpeed[BUNBUN_FLOAT]);
		BunBun->SetAnimation(BUN_BUN_APPEARS, 0.5f);
		BunBun->GetModel()->SetFrameOfAnimation(4020);
		nState = PLAYER_BUNBUN_FLOATING;
	}
}

void Player3D::JumpingStateControl()
{
	if (fGravityForce<0 && Model->GetCurrentFrame() > 894) {
		GravityControl();
		MoveControl();
	}

	if (Model->GetCurrentFrame() < 888 && Model->GetCurrentAnimation() == JUMP_UP)
		Model->SetFrameOfAnimation(888);
	AttackInputsControl();
	if (Model->GetCurrentAnimation() != JUMP_UP_TO_FALLDOWN)
		SetAnimation(JUMP_UP, fAnimationSpeed[JUMP_UP]);
	else
	{
		MoveControl();
		if (Model->GetCurrentFrame() >= 1016)
			nState = PLAYER_IDLE_STATE;
		return;
	}
	if (Model->GetCurrentFrame() >= 975)
		Model->SetFrameOfAnimation(905);
	if (Model->GetCurrentAnimation() == JUMP_UP && fGravityForce == 0)
		SetAnimation(JUMP_UP_TO_FALLDOWN, fAnimationSpeed[JUMP_UP_TO_FALLDOWN]);
	if (fGravityForce > 0)
		fGravityForce = 0;
}

void Player3D::GravityControl()
{
	if (IsOnTheFloor()) {
		if (!IsInCollision3D(GetHitboxPlayer(PLAYER_HB_FEET), pFloor->GetHitbox()))
			pFloor = nullptr;
		return;
	}
	
	if (--nCancellingGravityFrames > 0) {
		fGravityForce += GRAVITY_FORCE * 0.25f;
		Position.y -= fGravityForce * 0.25f;
		return;
	}
	if (nState == PLAYER_BUNBUN_FLOATING) {
		fGravityForce += GRAVITY_FORCE;
		Position.y -= fGravityForce * 0.125f;
		if (fGravityForce > 5)
			fGravityForce = 5;
		return;
	}
	fGravityForce += GRAVITY_FORCE;
	Position.y -= fGravityForce;
	nCancellingGravityFrames = 0;
	if (fGravityForce > 25)
		fGravityForce = 25;

}

void Player3D::IdleStateControl()
{
	if (GetAxis(MOVEMENT_AXIS_HORIZONTAL) || GetAxis(MOVEMENT_AXIS_VERTICAL))
		nState = PLAYER_MOVING_STATE;
	switch (Model->GetCurrentAnimation())
	{
	case ANIMATION_WALKING:
		if (fPlayerSpeed == MAX_SPEED) 
			SetAnimation(RUN_TO_IDLE, fAnimationSpeed[RUN_TO_IDLE]);
		else
			SetAnimation(ANIMATION_IDLE, fAnimationSpeed[ANIMATION_IDLE]);
		break;
	case RUN_TO_IDLE:
		if (Model->GetCurrentFrame() >= 1415)
			SetAnimation(ANIMATION_IDLE, fAnimationSpeed[ANIMATION_IDLE]);
		break;
	case JUMP_UP_TO_FALLDOWN:
		SetAnimation(AIR_IDLE, fAnimationSpeed[AIR_IDLE]);
		break;
	case AIR_IDLE:
		SetAnimation(AIR_IDLE, fAnimationSpeed[AIR_IDLE]);
		if (IsOnTheFloor())
			SetAnimation(ANIMATION_IDLE, fAnimationSpeed[ANIMATION_IDLE]);
		break;
	default:
		SetAnimation(ANIMATION_IDLE, fAnimationSpeed[ANIMATION_IDLE]);
		break;
	}
	fPlayerSpeed = 1;
	AttackInputsControl();
}

void Player3D::FightingStanceStateControl()
{
	if (IsOnTheFloor()) {
		if ((!pPreviousAttack  || (pPreviousAttack && pPreviousAttack->Animation != SLIDE)) && Model->GetCurrentAnimation()!= FIGHT_STANCE) {
			SetAnimation(FIGHT_STANCE, fAnimationSpeed[FIGHT_STANCE]);
		}
		else if(pPreviousAttack && pPreviousAttack->Animation == SLIDE)
		{
			SetAnimation(SLIDE_STANDUP_FIGHT, fAnimationSpeed[SLIDE_STANDUP_FIGHT]);
			if (GetInput(INPUT_ATTACK) && CheckHoldingBack()) {
				SwitchAttack(SLIDE_KICKUP);
				return;
			}
			if (Model->GetCurrentFrame()>=548)
			{
				pPreviousAttack = nullptr;
				SetAnimation(FIGHT_STANCE, fAnimationSpeed[FIGHT_STANCE]);
				nState = PLAYER_IDLE_STATE;
				return;
			}
		}
	}
	else {
		SetAnimation(AIR_IDLE, fAnimationSpeed[AIR_IDLE]);
		if (bKick) {
			SwitchAttack(KICKDOWN);
			return;
		}
	}
	if (Model->GetLoops() > 1)
		nState = PLAYER_IDLE_STATE;
	if (GetAxis(MOVEMENT_AXIS_HORIZONTAL) || GetAxis(MOVEMENT_AXIS_VERTICAL))
		nState = PLAYER_MOVING_STATE;
	nFightStanceFrameCount++;
#if DEBUG_WAITFRAME
	if(GetInput(INPUT_PUNCH))
		printf("%d\n", nFightStanceFrameCount);
#endif
	if (nFightStanceFrameCount < MAX_WAIT_TIMER && (bPunch || bKick) && pPreviousAttack && !pPreviousAttack->ResetInputs)
	{
			switch (pPreviousAttack->Animation)
			{
			case BASIC_CHAIN_A:
				if (bPunch)
					SwitchAttack(BASIC_CHAIN_B);
				else if (bKick)
					SwitchAttack(KICK_CHAIN_B);
				break;
			case BASIC_CHAIN_B:
				if (bPunch) {
					if(bFirstSetOfPunches)
						SwitchAttack(BASIC_CHAIN_C);
					else
					{
						bFirstSetOfPunches = true;
						SwitchAttack(BASIC_CHAIN_A);
					}
				}
				else if (bKick)
					SwitchAttack(BASIC_CHAIN_B_KICKA);
				break;
			case KICK_CHAIN_A:
				if (bPunch)
					SwitchAttack(BASIC_CHAIN_A);
				else if (bKick)
					SwitchAttack(KICK_CHAIN_B);
				break;
			case KICK_CHAIN_B:
				if (bPunch)
					SwitchAttack(BASIC_CHAIN_C);
				else if (bKick)
					SwitchAttack(KICK_CHAIN_C);
				break;
			case AIR_PUNCHB:
				if (bPunch) {
					if (bFirstSetOfPunches)
						SwitchAttack(AIR_PUNCHC);
					else
					{
						bFirstSetOfPunches = true;
						SwitchAttack(AIR_PUNCHA);
					}
					
				}
				break;
			case AIR_PUNCHA:
				if (bPunch) {
					SwitchAttack(AIR_PUNCHB);
				}
				break;
			case KNEEDASH:
				bPressedForwardMidAttack = bPressedBackwardMidAttack = false;
				break;
			default:
				AttackInputsControl();
				return;
			}
	}
	else if ((nFightStanceFrameCount >= MAX_WAIT_TIMER && nFightStanceFrameCount <= MAX_WAIT_TIMER +13) && (bPunch || bKick) && pPreviousAttack && !pPreviousAttack->ResetInputs)//WAIT INPUT
	{
			switch (pPreviousAttack->Animation)
			{
			case BASIC_CHAIN_A:
				if (bPunch) {
					SwitchAttack(BASIC_CHAIN_B);
				}
				break;
			case BASIC_CHAIN_B:
				if (bPunch) {
					SwitchAttack(BASIC_CHAIN_B_PAUSEA);
				}
				break;
			case AIR_PUNCHB:
				if (bPunch && !bFirstSetOfPunches)
					SwitchAttack(ROULETTE);
				break;
			case AIR_PUNCHA:
				if (bPunch) 
					SwitchAttack(AIR_PUNCHB);
				break;
			case KICK_CHAIN_A:
				if (bPunch)
					SwitchAttack(BASIC_CHAIN_A);
				else if (bKick)
					SwitchAttack(KICK_CHAIN_B);
				break;
			case KICK_CHAIN_B:
				if (bPunch)
					SwitchAttack(BASIC_CHAIN_C);
				break;
			default:
				FindMoveByAnimation(pPreviousAttack->nNextChain);
				return;
			}
	}
	if(nFightStanceFrameCount > MAX_WAIT_TIMER + 13 || (pPreviousAttack && pPreviousAttack->ResetInputs))
	{
		AttackInputsControl();
		return;
	}
}

void Player3D::FindMoveByAnimation(int anim)
{
	if (anim == MAX_ANIMATIONS)
	{
		if (bPunch)
			SwitchAttack(BASIC_CHAIN_A);
		else if(bKick)
			SwitchAttack(KICK_CHAIN_A);
		return;
	}
	for (int i = 0; i < MAX_ATTACKS; i++)
	{
		if (stAllMoves[i].Animation == anim)
		{
			SwitchAttack(i);
			nState = PLAYER_ATTACKING_STATE;
			return;
		}
	}
}

PLAYER_ATTACK_MOVE* Player3D::GetAttack(int anim)
{
	for (int i = 0; i < MAX_ATTACKS; i++)
	{
		if (stAllMoves[i].Animation == anim)
		{
			return &stAllMoves[i];
			
		}
	}
}

void Player3D::AttackStateControl()
{
	static bool bMultiPunch = false;
	if (bKick && !IsOnTheFloor())
		SwitchAttack(KICKDOWN);
	static XMFLOAT3 x3OriginalRotation = { 0,0,0 };
	static XMFLOAT3 x3RotationChange = { 0,0,0 };
	static bool PressedPunch, PressedKick;
	if (Model->GetLoops() > 0) {
		StopAttack();
	}
	if (!pCurrentAttackPlaying) {
		x3OriginalRotation = { 0,0,0 };
		x3RotationChange = { 0,0,0 };
		fGravityForce = 0;
		nCancellingGravityFrames = 15;
		nState = PLAYER_IDLE_FIGHT_STATE;
		nFightStanceFrameCount = 0;
		PressedKick = PressedPunch = false;
		bMultiPunch = false;
		return;
	}
	if (pCurrentAttackPlaying->Animation != BASIC_CHAIN_A &&
		pCurrentAttackPlaying->Animation != BASIC_CHAIN_B &&
		pCurrentAttackPlaying->Animation != AIR_PUNCHA &&
		pCurrentAttackPlaying->Animation != AIR_PUNCHB)
		bFirstSetOfPunches = false;
	if (pPreviousAttack && 
		pPreviousAttack->Animation != BASIC_CHAIN_A &&
		pPreviousAttack->Animation != BASIC_CHAIN_B &&
		pPreviousAttack->Animation != AIR_PUNCHA &&
		pPreviousAttack->Animation != AIR_PUNCHB)
		bFirstSetOfPunches = false;

	SetAnimation(pCurrentAttackPlaying->Animation, fAnimationSpeed[pCurrentAttackPlaying->Animation]);
	XMFLOAT3 rotCamera = Model->GetRotation();
	if (Model->GetCurrentFrame() > pCurrentAttackPlaying->nMinFrameForInputDetection 
		&& pCurrentAttackPlaying->nMinFrameForInputDetection!=-1)
	{
		if(!PressedPunch)
			PressedPunch = bPunch;
		if(!PressedKick)
			PressedKick = bKick;
	}
	else {
		PressedKick = PressedPunch = false;
	}
	if (Model->GetCurrentFrame() > pCurrentAttackPlaying->fpHitBoxActivation.InitialFrame && Model->GetCurrentFrame() < pCurrentAttackPlaying->fpHitBoxActivation.EndFrame)
		ActivateAttackHitbox(abs(-sinf(XM_PI + Model->GetRotation().y)*pCurrentAttackPlaying->ahsHitboxSize.x), abs(pCurrentAttackPlaying->ahsHitboxSize.y),abs(-cosf(XM_PI + Model->GetRotation().y)*pCurrentAttackPlaying->ahsHitboxSize.z),pCurrentAttackPlaying->ahsHitboxSize.speed);
	
	if (GetInput(INPUT_ATTACK))
		bMultiPunch = true;
	switch (pCurrentAttackPlaying->Animation)
	{
	case BASIC_CHAIN_A:
		if (bKick || PressedKick) {
			SwitchAttack(KICK_CHAIN_A);

		}else if (bPunch || PressedPunch) {
			if (Model->GetCurrentFrame() > 1132 && Model->GetCurrentFrame() < 1155) {
				SwitchAttack(BASIC_CHAIN_B);
				break;
			}
		}
		if (Model->GetCurrentFrame() >= 1155) 
			StopAttack();

		break;
	case BASIC_CHAIN_B:
		if (bPunch) {
			if (Model->GetCurrentFrame() > 1225) {
				if (!bFirstSetOfPunches)
				{
					bFirstSetOfPunches = true;
					SwitchAttack(BASIC_CHAIN_A);
					return;
				}
				SwitchAttack(BASIC_CHAIN_C);
				break;
			}
		}
		if (bKick || PressedKick) {
			if (Model->GetCurrentFrame() > 1225) {
				SwitchAttack(BASIC_CHAIN_B_KICKA);
				break;
			}
		}
		if (Model->GetCurrentFrame() >= 1245) {
			StopAttack();
		}

		break;
	case BASIC_CHAIN_C:
		bFirstSetOfPunches = false;
		if (Model->GetCurrentFrame() >= 1356) 
			StopAttack();

		break;
	case UPPERCUT:
		bPressedForwardMidAttack = bPressedBackwardMidAttack = false;
		if (Model->GetCurrentFrame() < 1809) {
			fGravityForce = -JUMP_FORCE;
			if(pFloor)
				Position.y += Hitboxes[PLAYER_HB_FEET].SizeY;
			pFloor = nullptr;
			
		}
		else {
			if (!bUppercutExecute) {
				fGravityForce += GRAVITY_FORCE;
				Position.y -= fGravityForce;
			}
		}
		if (Model->GetCurrentFrame() >= 1848)
			StopAttack();
		SetHitboxPosition(-sinf(XM_PI + Model->GetRotation().y) * 35, 50, -cosf(XM_PI + Model->GetRotation().y) * 35, PLAYER_HB_ATTACK);
		break;
	case HEADBUTT:
		nStamina--;
		if (Model->GetCurrentFrame() >= 2943)
		{
			StopAttack();
		}
		break;
	case SLIDE:
		Position.x -= sinf(XM_PI + rotCamera.y) * 15;
		Position.z -= cosf(XM_PI + rotCamera.y) * 15;
		if (Model->GetCurrentFrame() >= 510 || PressedKick)
		{
			if (GetInput(INPUT_ATTACK) && CheckHoldingBack()) {
				SwitchAttack(SLIDE_KICKUP);
				break;
			}
		}
		if (Model->GetCurrentFrame() >= 527)
		{
			StopAttack();
		}
		SetHitboxPosition(-sinf(XM_PI + Model->GetRotation().y) * 45, 0.0f, -cosf(XM_PI + Model->GetRotation().y) * 25, PLAYER_HB_ATTACK);
		break;
	case BASIC_CHAIN_B_PAUSEA:
		pCurrentAttackPlaying->nAttackID = 500;
		if ((Model->GetCurrentFrame() > 1893 && Model->GetCurrentFrame() < 1913) ||
			(Model->GetCurrentFrame() > 1919 && Model->GetCurrentFrame() < 1932) ||
			(Model->GetCurrentFrame() > 1939 && Model->GetCurrentFrame() < 1952) ||
			(Model->GetCurrentFrame() > 1958 && Model->GetCurrentFrame() < 1973) ||
			(Model->GetCurrentFrame() > 1979 && Model->GetCurrentFrame() < 1993) ||
			(Model->GetCurrentFrame() > 1998 && Model->GetCurrentFrame() < 2003) ||
			(Model->GetCurrentFrame() > 2008 && Model->GetCurrentFrame() < 2013) ||
			(Model->GetCurrentFrame() > 2055 && Model->GetCurrentFrame() < 2063))
				ActivateAttackHitbox(abs(-sinf(XM_PI + Model->GetRotation().y)*pCurrentAttackPlaying->ahsHitboxSize.x), abs(pCurrentAttackPlaying->ahsHitboxSize.y), abs(-cosf(XM_PI + Model->GetRotation().y)*pCurrentAttackPlaying->ahsHitboxSize.z), pCurrentAttackPlaying->ahsHitboxSize.speed);

		if (Model->GetCurrentFrame() == 1914 ||
			Model->GetCurrentFrame() == 1954 ||
			Model->GetCurrentFrame() == 1974 ||
			Model->GetCurrentFrame() == 1996 ||
			Model->GetCurrentFrame() == 2024)
		{
			if (!bMultiPunch)
				Model->SetFrameOfAnimation(2025);
			else
				bMultiPunch = false;
		}
		if(Model->GetCurrentFrame()>=2025)
			pCurrentAttackPlaying->nAttackID = 599;
		if (Model->GetCurrentFrame() >= 2086)
		{
			StopAttack();
		}
		break;
	case KICK_CHAIN_A:
		if (bPunch || PressedPunch) {
			if (Model->GetCurrentFrame() > 2168 && Model->GetCurrentFrame() < 2205) {
				SwitchAttack(BASIC_CHAIN_B);
				break;
			}
		}
		if (bKick || PressedKick) {
			if (Model->GetCurrentFrame() > 2168 && Model->GetCurrentFrame() < 2205) {
				SwitchAttack(KICK_CHAIN_B);
				break;
			}
		}
		if (Model->GetCurrentFrame() >= 2205) {
			StopAttack();
		}
		break;
	case KICK_CHAIN_B:
		if (bPunch || PressedPunch) {
			if (Model->GetCurrentFrame() > 2301 && Model->GetCurrentFrame() < 2315) {
				SwitchAttack(BASIC_CHAIN_C);
				break;
			}
		}
		if (bKick || PressedKick) {
			if (Model->GetCurrentFrame() > 2301 && Model->GetCurrentFrame() < 2315) {
				SwitchAttack(KICK_CHAIN_C);
				break;
			}
		}
		if (Model->GetCurrentFrame() >= 2315) {
			StopAttack();
		}
		break;
	case KICK_CHAIN_C:
		if (Model->GetCurrentFrame() >= 3056) {
			StopAttack();
		}
		break;
	case BACKDROP_KICK:
		nStamina--;
		if (Model->GetCurrentFrame() >= 2772) {
			StopAttack();
		}
		break;
	case SLIDE_KICKUP:
		if (Model->GetCurrentFrame() >= 632 && Model->GetCurrentFrame() <= 690) {
			fGravityForce += GRAVITY_FORCE;
			Position.y -= fGravityForce;
			if (fGravityForce > 0)
				fGravityForce = 0;
		}
		else {
			fGravityForce = -JUMP_FORCE*1.25f;
			if (Model->GetCurrentFrame() >= 734)
			{
				fGravityForce = 0;
				StopAttack();
				nState = PLAYER_IDLE_STATE;
				pFloor = nullptr;
				SetAnimation(AIR_IDLE, fAnimationSpeed[AIR_IDLE]);
				nCancellingGravityFrames = 15;
			}
		}
		break;
	case BASIC_CHAIN_B_KICKA:
		if (Model->GetCurrentFrame() >= 3553)
		{
			if (bKick || PressedKick || bPunch || PressedPunch) {
				SwitchAttack(BASIC_CHAIN_B_KICKB);
				break;
			}
		}
		if (Model->GetCurrentFrame() >= 3577)
		{
			StopAttack();
		}
		break;
	case BASIC_CHAIN_B_KICKB:
		if (Model->GetCurrentFrame() >= 3649)
		{
			if (bKick || PressedKick || bPunch || PressedPunch) {
				if (CheckHoldingForward())
				{
					SwitchAttack(BASIC_CHAIN_B_KICKB_PUNCH);
					break;
				}
				if (CheckHoldingBack())
				{
					SwitchAttack(BASIC_CHAIN_B_KICKB_FORWARD);
					break;
				}
				SwitchAttack(BASIC_CHAIN_B_KICKC);
				break;
			}
		}
		if (Model->GetCurrentFrame() >= 3668)
			StopAttack();
		break;
	case BASIC_CHAIN_B_KICKC:
		if (Model->GetCurrentFrame() >= 3798)
			StopAttack();
		break;
	case AIR_PUNCHA:
		if (Model->GetCurrentFrame() >= 3122)
		{
			if (bPunch || PressedPunch)
			{
				SwitchAttack(AIR_PUNCHB);
				break;
			}
		}
		if (Model->GetCurrentFrame() >= 3138)
			StopAttack();
		break;
	case AIR_PUNCHB:
		if (Model->GetCurrentFrame() >= 3205)
		{
			if (bPunch || PressedPunch)
			{
				if (!bFirstSetOfPunches)
				{
					bFirstSetOfPunches = true;
					SwitchAttack(AIR_PUNCHA);
					return;
				}
				SwitchAttack(AIR_PUNCHC);
				break;
			}
		}
		if (Model->GetCurrentFrame() >= 3217)
		{
			StopAttack();
		}
		break;
	case AIR_PUNCHC:
		bFirstSetOfPunches = false;
		if (Model->GetCurrentFrame() >= 3330)
		{
			StopAttack();
		}
		break;
	case ROULETTE:
		//回ることのコントロール
		if (Model->GetCurrentFrame() <= 3337)
			x3OriginalRotation = Model->GetRotation();
		else if (Model->GetCurrentFrame() > 3337 && Model->GetCurrentFrame() < 3350)
		{
			x3RotationChange.y -= 0.1f;
			Model->RotateAroundY(x3OriginalRotation.y + x3RotationChange.y);
		}
		else if (Model->GetCurrentFrame() > 3350 && Model->GetCurrentFrame() < 3390)
		{
			if (x3RotationChange.y < XM_2PI) {
				x3RotationChange.y += 0.35f;
				if (x3RotationChange.y > XM_2PI)
					x3RotationChange.y = XM_2PI;
				Model->SetRotation({ x3OriginalRotation.x, x3OriginalRotation.y + x3RotationChange.y, x3RotationChange.z });
				if(x3RotationChange.y == XM_2PI)
					Model->SetRotation({ x3OriginalRotation.x, x3OriginalRotation.y, x3RotationChange.z });
			}
		}
		if (Model->GetCurrentFrame() < 3366 || Model->GetCurrentFrame() > 3396) {
			fGravityForce = -JUMP_FORCE;
			if (pFloor)
				Position.y += Hitboxes[PLAYER_HB_FEET].SizeY;
			pFloor = nullptr;

		}
		else {
			//printf("%f\n", fGravityForce);
			if (!bRouletteExecuted) {
				fGravityForce += GRAVITY_FORCE;
				Position.y -= fGravityForce;
			}
		}
		if (Model->GetCurrentFrame() >= 3408) {
			StopAttack();
		}
		break;
	case KNEEDASH:
		Position.x -= sinf(XM_PI + rotCamera.y) * 25;
		Position.z -= cosf(XM_PI + rotCamera.y) * 25;
		break;
	case KICKDOWN:
		if (Model->GetCurrentFrame() < pCurrentAttackPlaying->fpHitBoxActivation.InitialFrame) {
			fGravityForce = 0;
		}
		if (Model->GetCurrentFrame() > pCurrentAttackPlaying->fpHitBoxActivation.InitialFrame && Model->GetCurrentFrame() < pCurrentAttackPlaying->fpHitBoxActivation.EndFrame && !pFloor)
		{
			fGravityForce += KICKDOWN_SPEED;
			Position.y -= fGravityForce;
		}
		if (Model->GetCurrentFrame() >= 4200 && !pFloor)
		{
			Model->SetFrameOfAnimation(pCurrentAttackPlaying->fpHitBoxActivation.InitialFrame);
		}
		if (pFloor && Model->GetCurrentFrame() <= 4200) {
			Model->SetFrameOfAnimation(4201);
			pCamera->SetShaking(10.0f, 7, 2);
		}
		break;
	case RED_HOT_KICK:
		nStamina--;
		if (nStamina < 0)
			nStamina = 0;
		if(Model->GetCurrentFrame()<=4264)
			bRHK_Hit = false;
		if (!pFloor && !bRHK_Hit) {
			if (Model->GetCurrentFrame() >= 4338)
				Model->SetFrameOfAnimation(4282);
			if (Model->GetCurrentFrame() < 4338 && Model->GetCurrentFrame() > 4283)
			{
				if (pLockedEnemy)
				{
					MoveTowardPosXZ({ pLockedEnemy->GetPosition().x, pLockedEnemy->GetPosition().z }, 30);
					fGravityForce += GRAVITY_FORCE;
					Position.y -= fGravityForce * 0.75f;
				}
				else {
					Position.x -= sinf(XM_PI + rotCamera.y) * 25;
					Position.z -= cosf(XM_PI + rotCamera.y) * 25;
					fGravityForce += GRAVITY_FORCE;
					Position.y -= fGravityForce * 0.75f;
				}
			}
		}
		else if(pFloor && !bRHK_Hit){
			if (Model->GetCurrentFrame() < 4478) {
				Model->SetFrameOfAnimation(4478);
			}

			if (Model->GetCurrentFrame() > 4478 && Model->GetCurrentFrame() < 4522)
			{
				Position.x -= sinf(XM_PI + rotCamera.y) * 5;
				Position.z -= cosf(XM_PI + rotCamera.y) * 5;
			}
		}
		if (bRHK_Hit)
		{
			if (Model->GetCurrentFrame() < 4349)
				Model->SetFrameOfAnimation(4349);
			if (Model->GetCurrentFrame() > 4387) {
				Position.x += sinf(XM_PI + rotCamera.y) * 5;
				Position.z += cosf(XM_PI + rotCamera.y) * 5;
				if (Model->GetCurrentFrame() < 4404) {
					Position.y += 5;
					fGravityForce = 0;
				}
				if (Model->GetCurrentFrame() > 4418)
				{
					if (!pFloor) {
						fGravityForce += GRAVITY_FORCE;
						Position.y -= fGravityForce * 0.5f;
						AttackInputsControl();
						if (GetInput(INPUT_JUMP_HOLD))
						{
							StopAttack();
							TransitionToFloatingBunBun();
							return;
						}
						if(Model->GetCurrentFrame() > 4460)
							Model->SetFrameOfAnimation(4422);
					}
					else {
						if (Model->GetCurrentFrame() < 4461)
							Model->SetFrameOfAnimation(4462);
						if (Model->GetCurrentFrame() > 4477 && Model->GetCurrentFrame() < 4512)
						{
							Position.x += sinf(XM_PI + rotCamera.y) * 0.5f;
							Position.z += cosf(XM_PI + rotCamera.y) * 0.5f;
						}
					}
				}
			}
		}
		break;
	default:
		break;
	}
}

void Player3D::SetHitboxPosition(float x, float y, float z, int HB)
{
	Hitboxes[HB].PositionX = x;
	Hitboxes[HB].PositionY = y;
	Hitboxes[HB].PositionZ = z;
}

void Player3D::ActivateAttackHitbox(float x, float y, float z, float Speed)
{
	Hitboxes[PLAYER_HB_ATTACK].SizeX = x;
	Hitboxes[PLAYER_HB_ATTACK].SizeY = y;
	Hitboxes[PLAYER_HB_ATTACK].SizeZ = z;
	Position.x -= sinf(XM_PI + Model->GetRotation().y) * Speed;
	Position.z -= cosf(XM_PI + Model->GetRotation().y) * Speed;
}

void Player3D::StopAttack()
{
	if (pCurrentAttackPlaying) {
		switch (pCurrentAttackPlaying->Animation)
		{
		case UPPERCUT:
			bUppercutExecute = true;
			break;
		case ROULETTE:
			bRouletteExecuted = true;
			break;
		default:
			break;
		}
		if (pCurrentAttackPlaying->ResetInputs)
			ResetInputs();
		pPreviousAttack = GetAttack(pCurrentAttackPlaying->Animation);
	}

	pCurrentAttackPlaying = nullptr;
}

void Player3D::CalculateDirectionalInput()
{
	float fHorizontalAxis = GetAxis(MOVEMENT_AXIS_HORIZONTAL);
	float fVerticalAxis = GetAxis(MOVEMENT_AXIS_VERTICAL);

	static int nFrameWait = 0;
	static float PreviousHorizontal = 0;
	static float PreviousVertical = 0;

	if (fHorizontalAxis == 0 && fVerticalAxis == 0) {
		if (++nFrameWait < 2)
			return;
		eDirection = DIR_NONE;
		PreviousHorizontal = 0;
		PreviousVertical = 0;
		return;
	}

	if (abs(fHorizontalAxis - PreviousHorizontal) < 0.25f && abs(fVerticalAxis - PreviousVertical) < 0.25f)
	{
		if (++nFrameWait < 2)
			return;
		eDirection = DIR_NONE;
		return;
	}
	nFrameWait = 0;
	PreviousHorizontal = fHorizontalAxis;
	PreviousVertical = fVerticalAxis;

	float nModelRotation = -(atan2(fVerticalAxis, fHorizontalAxis) - 1.570796f);
	XMFLOAT3 x3CurrentModelRot = Model->GetRotation();
	float DirInput =abs((nModelRotation + pCamera->GetRotation(false).y) - x3CurrentModelRot.y);
	if (DirInput > XM_PI) {
		DirInput -= XM_2PI;
		DirInput = abs(DirInput);
	}
	if (DirInput >= BACKWARD_INPUT_OFFSET)
	{
		eDirection = DIR_BACKWARD;
		AddInput('B');
		nInputTimer = MAX_INPUT_TIMER * 0.25f;
#if DEBUG_DIRECTIONALS
		printf("BACKWARD\n", DirInput);
#endif
	}
	else if (DirInput<= FORWARD_INPUT_OFFSET)
	{
		AddInput('F');
		nInputTimer = MAX_INPUT_TIMER * 0.25f;
		eDirection = DIR_FORWARD;
#if DEBUG_DIRECTIONALS
		printf("FORWARD\n", DirInput);
#endif
	}
	else {
		eDirection = DIR_OTHER;
#if DEBUG_DIRECTIONALS
		printf("OTHER\n", DirInput);
#endif
	}
	
}

void Player3D::SwitchAttack(int nNextAttack)
{
	if ((nNextAttack == RED_HOT_KICK || nNextAttack == HEADBUTT || nNextAttack == BACKDROP_KICK) && bAllStaminaUsed)
		return;
	for (int i = 0; i < MAX_ATTACKS; i++)
	{
		//printf("%d\n", i);
		if (stAllMoves[i].eAirMove == GROUND_MOVE && !IsOnTheFloor()
			|| stAllMoves[i].eAirMove == AIR_MOVE && IsOnTheFloor())
			continue;
		if (stAllMoves[i].Animation == nNextAttack) {
			pPreviousAttack = pCurrentAttackPlaying;
			pCurrentAttackPlaying = &stAllMoves[i];
			nState = PLAYER_ATTACKING_STATE;
			SetAnimation(stAllMoves[i].Animation, fAnimationSpeed[stAllMoves[i].Animation]);
			if(IsOnTheFloor())
				bPressedForwardMidAttack = bPressedBackwardMidAttack = false;
			if (pCurrentAttackPlaying->Animation != BASIC_CHAIN_A && pCurrentAttackPlaying->Animation != BASIC_CHAIN_B
				&& pCurrentAttackPlaying->Animation != AIR_PUNCHA && pCurrentAttackPlaying->Animation != AIR_PUNCHB)
				bFirstSetOfPunches = false;
			return;
		}
	}
	
}

void Player3D::MoveControl()
{
	static float RotTarget = 0;
	float fSpeedSlowOffset = 1;
	//アニメーション
	if (Model->GetCurrentAnimation() == SLIDE_STANDUP_FIGHT) {
		if (Model->GetCurrentFrame() >= 548)
		{
			pPreviousAttack = nullptr;
			SetAnimation(FIGHT_STANCE, fAnimationSpeed[FIGHT_STANCE]);
			nState = PLAYER_IDLE_STATE;
			return;
		}
		return;
	}
	if (nState != PLAYER_JUMPING_UP_STATE && nState != PLAYER_BUNBUN_FLOATING) {
		if (IsOnTheFloor()) {
			if (bLockingOn && !CheckHoldingForward() && (GetAxis(MOVEMENT_AXIS_HORIZONTAL)!=0 || GetAxis(MOVEMENT_AXIS_VERTICAL) !=0)) {
				SetAnimation(BACKWARD, fAnimationSpeed[BACKWARD]);
			}
			else {
				float analogSpd = abs(GetAxis(MOVEMENT_AXIS_HORIZONTAL)) + abs(GetAxis(MOVEMENT_AXIS_VERTICAL));
				float fAnimationSpd = (fAnimationSpeed[ANIMATION_WALKING] * (fPlayerSpeed / MAX_SPEED));
				if (fAnimationSpd > fAnimationSpeed[ANIMATION_WALKING])
					fAnimationSpd = fAnimationSpeed[ANIMATION_WALKING];
				if (Model->GetCurrentAnimation() != ANIMATION_WALKING && Model->GetCurrentAnimation() != ANIMATION_WALKING_START)
					SetAnimation(ANIMATION_WALKING_START, fAnimationSpeed[ANIMATION_WALKING_START]);
				else if (Model->GetCurrentAnimation() == ANIMATION_WALKING_START && (Model->GetLoops() >= 1 || Model->GetCurrentFrame() == 257))
					SetAnimation(ANIMATION_WALKING, fAnimationSpd);
				if (!GetAxis(MOVEMENT_AXIS_HORIZONTAL) && !GetAxis(MOVEMENT_AXIS_VERTICAL))
					nState = PLAYER_IDLE_STATE;
			}
		}
		else
		{
			SetAnimation(AIR_IDLE, fAnimationSpeed[AIR_IDLE]);
		}
	}
	if (bLockingOn && !CheckHoldingForward() && (GetAxis(MOVEMENT_AXIS_HORIZONTAL) != 0 || GetAxis(MOVEMENT_AXIS_VERTICAL) != 0) && nState != PLAYER_BUNBUN_FLOATING)
		fSpeedSlowOffset = 0.35f;
	//動く
	float fHorizontalAxis = GetAxis(MOVEMENT_AXIS_HORIZONTAL);
	float fVerticalAxis = GetAxis(MOVEMENT_AXIS_VERTICAL);
	float nModelRotation = -(atan2(fVerticalAxis, fHorizontalAxis) - 1.570796f);
	fPlayerSpeed += 0.5f;
	if (fPlayerSpeed >= MAX_SPEED)
		fPlayerSpeed = MAX_SPEED;
	XMFLOAT3 rotCamera = pMainCamera->GetCameraAngle();

	float fLockMoveBack = 1;
	if (Model->GetCurrentAnimation() == BACKWARD)
		fLockMoveBack = 0.5f;
	if (fVerticalAxis != 0) {
		Position.x -= sinf(XM_PI + rotCamera.y) * fPlayerSpeed * fVerticalAxis * fLockMoveBack*fSpeedSlowOffset;
		Position.z -= cosf(XM_PI + rotCamera.y) * fPlayerSpeed * fVerticalAxis * fLockMoveBack*fSpeedSlowOffset;
	}
	if (fHorizontalAxis != 0) {
		Position.x -= sinf(rotCamera.y - XM_PI * 0.50f) * fPlayerSpeed * fHorizontalAxis * fLockMoveBack*fSpeedSlowOffset;
		Position.z -= cosf(rotCamera.y - XM_PI * 0.50f) * fPlayerSpeed * fHorizontalAxis * fLockMoveBack*fSpeedSlowOffset;
	}
	
	if (GetAxis(MOVEMENT_AXIS_HORIZONTAL) || GetAxis(MOVEMENT_AXIS_VERTICAL))
		RotTarget = nModelRotation + Rotation.y + pCamera->GetRotation().y;
	if (!GetAxis(MOVEMENT_AXIS_HORIZONTAL) && !GetAxis(MOVEMENT_AXIS_VERTICAL) && Model->GetCurrentAnimation() == BACKWARD && Model->GetCurrentFrame() < 2387)
	{
		nState = PLAYER_IDLE_STATE;
		SetAnimation(ANIMATION_IDLE, fAnimationSpeed[ANIMATION_IDLE]);
		return;
	}
	if (fVerticalAxis != 0 || fHorizontalAxis != 0 || (Model->GetCurrentAnimation() == BACKWARD && abs(Model->GetRotation().y - RotTarget) > 0.15f))
	{
		if(!bLockingOn)
			Model->SetRotationY(nModelRotation + Rotation.y + pCamera->GetRotation().y);
	}
}

//*****************************************************************************
//Draw関数
//レンダリング関数
//引数：void
//戻：void
//*****************************************************************************
void Player3D::Draw()
{
	SetCullMode(CULLMODE_CCW);
	pDodgeSkybox->Draw();
	SetCullMode(CULLMODE_NONE);
#if SHOW_HITBOX && SHOW_PLAYER_HITBOX
	GetMainLight()->SetLightEnable(false);
	for (int i = 0; i < PLAYER_HB_MAX; i++)
	{
		if (!pVisualHitboxes[i])continue;
		Box pHB = GetHitboxPlayer(i);
		pVisualHitboxes[i]->SetScale({ pHB.SizeX, pHB.SizeY, pHB.SizeZ });
		pVisualHitboxes[i]->SetPosition({ pHB.PositionX, pHB.PositionY, pHB.PositionZ });
		pVisualHitboxes[i]->Draw();
	}
	GetMainLight()->SetLightEnable(true);
#endif
	Actor::Draw();
	ChuSign->Draw();
	if(nState == PLAYER_BUNBUN_FLOATING || nState == PLAYER_BUNBUN_FALLING)
		BunBun->Draw();
	SetCullMode(CULLMODE_NONE);
	if (nStamina != MAX_PLAYER_STAMINA) {
		pBatteryEnergy->Draw();
		pBattery->Draw();
		if(bAllStaminaUsed)
			pBatteryWasted->Draw();
	}
	SetCullMode(CULLMODE_CW);
}

//*****************************************************************************
//End関数
//終了関数
//引数：void
//戻：void
//*****************************************************************************
void Player3D::End()
{
	SAFE_DELETE(BunBun);
	SAFE_DELETE(pBattery);
	SAFE_DELETE(pBatteryEnergy);
	SAFE_DELETE(pBatteryWasted);
	SAFE_DELETE(ChuSign);
	SAFE_DELETE(pDodgeSkybox);
}

//*****************************************************************************
//Attack関数
//攻撃動作を行う
//引数：const char*
//戻：void
//*****************************************************************************
void Player3D::Attack(const char * atkInput)
{
	char szAtkInput[MAX_PLAYER_INPUT + 1];
	int i = 0;
	for (i = 0; i < MAX_PLAYER_INPUT && atkInput[i] != '*'; szAtkInput[i] = atkInput[i], i++);
	szAtkInput[i] = '\0';

#if DEBUG_ATTACK_INPUTS
	printf("%s\n", atkInput);
#endif
	Attack(szAtkInput, MAX_PLAYER_INPUT);
}

//*****************************************************************************
//Attack関数
//攻撃動作を行う
//引数：const char*, int
//戻：void
//*****************************************************************************
void Player3D::Attack(const char * atkInput, int recursions)
{
	if (recursions <= 0)
		return;
	char AtkInputComp[MAX_PLAYER_INPUT + 1];
	strcpy(AtkInputComp, atkInput);
	static bool bLastCheck = false;
	if (recursions == MAX_PLAYER_INPUT)
		bLastCheck = false;

	for (int i = 0; i < MAX_ATTACKS; i++)
	{
#if VERIFY_ISONFLOOR
		if (stAllMoves[i].eAirMove == GROUND_MOVE && !IsOnTheFloor()
			|| stAllMoves[i].eAirMove == AIR_MOVE && IsOnTheFloor())
			continue;
#endif
		if (!strcmp(stAllMoves[i].Input, AtkInputComp))
		{
			//fAtkAcceleration = 0;
#if DEBUG_FOUND_ATTACKS
			printf("FOUND AT: %d, ANIMATION: %s\n", i, stAllMoves[i].Input);
#endif
			SwitchAttack(stAllMoves[i].Animation);
			
			//pCurrentAttackPlaying = &stAllMoves[i];
			//nState = PLAYER_ATTACKING_STATE;
			if (stAllMoves[i].ResetInputs)
				ResetInputs();
			return;
		}
	}
	char szAtkInput[MAX_PLAYER_INPUT + 1];
	int i = 0;
	for (i = 1; i < MAX_PLAYER_INPUT && atkInput[i] != '\0'; szAtkInput[i - 1] = atkInput[i], i++);
	szAtkInput[i - 1] = '\0';
	Attack(szAtkInput, recursions - 1);
}

//*****************************************************************************
//IsOnTheFloor関数
//プレイヤーの床を確認する
//引数：void
//戻：bool
//*****************************************************************************
bool Player3D::IsOnTheFloor()
{
	return pFloor != nullptr;
}

//*****************************************************************************
//ResetInputs関数
//インプットの行列をリセットする
//引数：void
//戻：void
//*****************************************************************************
void Player3D::ResetInputs()
{
	strcpy(szInputs, "********");

}

//*****************************************************************************
//AttackInputsControl関数
//攻撃動作のインプットの管理
//引数：void
//戻：void
//*****************************************************************************
void Player3D::AttackInputsControl()
{

	if (nState == PLAYER_ATTACKING_STATE) {
		if(!(pCurrentAttackPlaying && pCurrentAttackPlaying->Animation==RED_HOT_KICK))
			return;
	}
	if (GetInput(INPUT_ATTACK))
	{
		if (strstr("FF", szInputs) && IsOnTheFloor()) {
			SwitchAttack(HEADBUTT);
			ResetInputs();
			return;
		}
		if (strstr("FF", szInputs) && !IsOnTheFloor()) {
			SwitchAttack(KNEEDASH);
			ResetInputs();
			return;
		}
		if (strstr("BF", szInputs) && IsOnTheFloor()) {
			if (IsOnTheFloor())
				SwitchAttack(BACKDROP_KICK);
			ResetInputs();
			return;
		}
		if (CheckHoldingForward() && !strstr("BF", szInputs) && !strstr("FB", szInputs) && IsOnTheFloor()) {
			SwitchAttack(SLIDE);
			return;
		}
	}
	if (!IsOnTheFloor() && CheckHoldingForward() && GetInput(INPUT_ATTACK) && !bAllStaminaUsed && !strstr("FF", szInputs) && !(pCurrentAttackPlaying && pCurrentAttackPlaying->Animation == RED_HOT_KICK))
	{
		if (pCurrentAttackPlaying && pCurrentAttackPlaying->Animation != KNEEDASH)
			return;
		SwitchAttack(RED_HOT_KICK);
		ResetInputs();
		return;
	}
	if (bPunch)
	{
		nInputTimer = 0;
		AddInput('A');
		Attack(szInputs);
	}
	if (bKick)
	{
		if (Model->GetCurrentAnimation() == SLIDE || Model->GetCurrentAnimation()==SLIDE_STANDUP_FIGHT)
		{
			SwitchAttack(SLIDE_KICKUP);
			ResetInputs();
			return;
		}
		if (!strcmp("FB", szInputs) && IsOnTheFloor()) {
			SwitchAttack(HEADBUTT);
			ResetInputs();
			return;
		}
		if (!strcmp("BF", szInputs)) {
			if(IsOnTheFloor())
				SwitchAttack(BACKDROP_KICK);
			ResetInputs();
			return;
		}
		nInputTimer = 0;
		AddInput('K');
		Attack(szInputs);
		//if (Model->GetCurrentAnimation() == BACKWARDTOFORWARD)
		//	SwitchAttack(BACKDROP_KICK);
	}
	if (pCurrentAttackPlaying)
		nState = PLAYER_ATTACKING_STATE;


	//printf("%s\n", szInputs);
}

void Player3D::InputResetControl()
{
	if (nState == PLAYER_ATTACKING_STATE)
		return;
	if (strcmp(szInputs, "********")) {
		if (++nInputTimer > MAX_INPUT_TIMER)
		{
			ResetInputs();
			nInputTimer = 0;
		}
	}
}


//*****************************************************************************
//AddInput関数
//プレイヤーのインプットキャラを行列に入れる
//引数：char
//戻：void
//*****************************************************************************
void Player3D::AddInput(char A)
{
	if (A == 'P' && szInputs[0] == '*')
		return;
	for (int i = 0; i < MAX_PLAYER_INPUT; i++)
	{
		if (szInputs[i] == '*') {
			szInputs[i] = A;
#if DEBUG_ADD_INPUTS
			printf("INPUTS: %s\n", szInputs);
#endif
			return;
		}
	}
	for (int i = 0; i < MAX_PLAYER_INPUT; i++)
	{
		if (i != MAX_PLAYER_INPUT - 1)
		{
			szInputs[i] = szInputs[i + 1];
		}
		else {
			szInputs[i] = A;
#if DEBUG_ADD_INPUTS
			printf("INPUTS: %s\n", szInputs);
#endif
		}
	}
}

//*****************************************************************************
//GetLastInputInserted関数
//最後の入れたインプットを戻す
//引数：void
//戻：char
//*****************************************************************************
char Player3D::GetLastInputInserted()
{
	for (int i = 0; i < MAX_PLAYER_INPUT; i++)
	{
		if (szInputs[i] == '*')
			return szInputs[i - 1];
	}
	return szInputs[MAX_PLAYER_INPUT - 1];
}

Player3D * GetPlayer()
{
	return pCurrentPlayer;
}

bool GetIsSoftLockOn()
{
	return bIsSoftLockOn;
}

//*****************************************************************************
//GetHitboxPlayer関数
//プレイヤーのヒットボックスを戻す
//引数：int
//戻：Box
//*****************************************************************************
Box Player3D::GetHitboxPlayer(int hb)
{
	return { Hitboxes[hb].PositionX + Position.x, Hitboxes[hb].PositionY + Position.y,Hitboxes[hb].PositionZ + Position.z, Hitboxes[hb].SizeX,Hitboxes[hb].SizeY,Hitboxes[hb].SizeZ };
}

XMFLOAT3 Player3D::GetHitboxPos(int hb)
{
	return { Hitboxes[hb].PositionX + Position.x, Hitboxes[hb].PositionY + Position.y,Hitboxes[hb].PositionZ + Position.z };
}

float Player3D::GetGravityForce()
{
	return fGravityForce;
}

void Player3D::AddStamina(int Stamina)
{
	nStamina += Stamina;
}

void Player3D::Damage()
{
	if (fGravityForce < 0 || nState == PLAYER_DAMAGED_STATE) { return; }; 
	if (nState == PLAYER_DODGING_STATE) { bDodged = true; return; } 
	nState = PLAYER_DAMAGED_STATE; 
	nHP--;
	ActivateDamageEffect();
}
void Player3D::Damage(int dam)
{
	if (fGravityForce < 0 || nState == PLAYER_DAMAGED_STATE) { return; }; 
	if (nState == PLAYER_DODGING_STATE) { bDodged = true; return; } 
	nState = PLAYER_DAMAGED_STATE; 
	nHP -= dam;
	ActivateDamageEffect();
};
