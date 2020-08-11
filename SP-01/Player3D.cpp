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

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define PLAYER_MODEL_PATH "data/model/Hana.fbx"
#define MAX_ATTACKS 24
#define MAX_INPUT_TIMER 27
#define MAX_WAIT_TIMER 13
#define DEBUG_FOUND_ATTACKS false
#define VERIFY_ISONFLOOR true
#define DEBUG_ADD_INPUTS false
#define MAX_SPEED 6.8f
#define GRAVITY_FORCE 0.98f
#define SHOW_PLAYER_HITBOX true
#define SHOW_SPECIFIC_PLAYER_HITBOX -1
#define JUMP_FORCE 20
#define DEBUG_DIRECTIONALS false
#define DEBUG_WAITFRAME false
#define KICKDOWN_SPEED 10.98

//*****************************************************************************
// グローバル変数
//*****************************************************************************

Player3D* pMainPlayer3D = nullptr;
PLAYER_ATTACK_MOVE stAllMoves[MAX_ATTACKS] =
{
	{"A",	  BASIC_CHAIN_A,				false,	GROUND_MOVE, BASIC_CHAIN_B,	 1123,	{ 1122, 1133}	,{15, 20, 10, 10},	100},
	{"n",	  BASIC_CHAIN_B,				false,	GROUND_MOVE, BASIC_CHAIN_C,	 1204,	{ 1200, 1220}	,{15, 20, 10, 10},	200},//AA
	{"n",	  BASIC_CHAIN_C,				true,	GROUND_MOVE, MAX_ANIMATIONS, -1,	{ 1300, 1320}	,{15, 20, 10, 10},	300},//AAA
	{"BFA",	  UPPERCUT,						true,	GROUND_MOVE, MAX_ANIMATIONS, -1,	{ 1813, 1837}	,{15, 30, 15,  5},	400},//BFA
	{"BBA",	  UPPERCUT,						true,	AIR_MOVE,	 MAX_ANIMATIONS, -1,	{ 1813, 1837}	,{15, 30, 15,  5},	400},//BFA
	{"FFA",	  HEADBUTT,						true,	GROUND_MOVE, MAX_ANIMATIONS, -1,	{ 2866, 2914}	,{15, 20, 60,  8},	400},//BFA
	{"BFK",	  BACKDROP_KICK,				true,	GROUND_MOVE, MAX_ANIMATIONS, -1,	{ 2680, 2697}	,{15,  0, 60, 12},	400},//BFK
	{"FFK",	  SLIDE,						true,	GROUND_MOVE, MAX_ANIMATIONS, 482,	{ 467,  512}	,{15, 30, 30,  0},	400},//BFK
	{"N",	  BASIC_CHAIN_B_PAUSEA,			true,	GROUND_MOVE, MAX_ANIMATIONS, -1,	{ 0, 0}			,{15, 20, 10,  0},	500},//AAPA
	{"K",	  KICK_CHAIN_A,					false,	GROUND_MOVE, KICK_CHAIN_B,	 2130,	{ 2140, 2162}	,{15, 20, 10,  5},	600},//K
	{"n",	  KICK_CHAIN_B,					false,	GROUND_MOVE, BASIC_CHAIN_C,	 2270,	{ 2276, 2292}	,{15, 20, 10,  5},	700},//KK
	{"n",	  KICK_CHAIN_C,					true,	GROUND_MOVE, MAX_ANIMATIONS, -1,	{ 2995, 3028}	,{15, 20, 10, 10},	800},//KKK
	{"n",	  SLIDE_KICKUP,					true,	GROUND_MOVE, MAX_ANIMATIONS, -1,	{ 0, 0}			,{15, 20, 10,  0},	900},//
	{"n",	  BASIC_CHAIN_B_KICKA,			true,	GROUND_MOVE, MAX_ANIMATIONS, 3515,	{ 3505, 3556}	,{15, 20, 10,  8},	1000},//AAK
	{"n",     BASIC_CHAIN_B_KICKB,			true,	GROUND_MOVE, MAX_ANIMATIONS, 3616,	{ 3606, 3652}	,{15, 20, 10,  8},	1100},//AAKK
	{"n",     BASIC_CHAIN_B_KICKC,			true,	GROUND_MOVE, MAX_ANIMATIONS, -1,	{ 3740, 3765}	,{15, 20, 10,  8},	1200},//AAKK
	{"n",     BASIC_CHAIN_B_KICKB_PUNCH,	true,	GROUND_MOVE, MAX_ANIMATIONS, -1,	{ 3934, 3973}	,{15, 20, 10,  9},	1300},//AAKFK
	{"n",	  BASIC_CHAIN_B_KICKB_FORWARD,	true,	GROUND_MOVE, MAX_ANIMATIONS, -1,	{ 3828, 3852}	,{15, 20, 10,  2},	1400},//AAKA
	{"A",	  AIR_PUNCHA,					false,	AIR_MOVE,	 AIR_PUNCHB,	 3100,	{ 3100, 3113}	,{15, 20, 10,  0},	1500},//A
	{"n",	  AIR_PUNCHB,					false,	AIR_MOVE,	 AIR_PUNCHC,	 3175,	{ 3167, 3191}	,{15, 20, 10,  0},	1600},//A
	{"n",	  AIR_PUNCHC,					true,	AIR_MOVE,	 MAX_ANIMATIONS, -1,	{ 3272, 3290}	,{15, 20, 10,  0},	1700},//A
	{"BBK",	  KNEEKICK,						true,	AIR_MOVE,	 MAX_ANIMATIONS, -1,	{ 3423, 3461}	,{15, 20, 10,  0},	1800},//A
	{"N",	  ROULETTE,						true,	AIR_MOVE,	 MAX_ANIMATIONS, -1,	{ 3363, 3395}	,{15, 20, 10,  0},	1900},//A
	{"K",	  KICKDOWN,						true,	AIR_MOVE,	 MAX_ANIMATIONS, -1,	{ 4157, 4200}	,{15, 20, 10,  0},	1900},//K
};

float fAnimationSpeed[] =
{
	1,
	2.5f,
	2.5f,//ANIMATION_WALKING
	2,//SLIDE
	0.5f,//SLIDE_STANDUP_FIGHT
	1,//SLIDE_STANDUP_IDLE
	1,//SLIDE_KICKUP
	1,//AIRIDLE
	2,//JUMP_UP
	1.5f,//JUMP_UP_TO_FALLDOWN
	1,//FIGHT_STANCE
	1.3f,//BASIC_CHAIN_A
	1.3f,//BASIC_CHAIN_B
	0.75f,//BASIC_CHAIN_C
	1.75f,//RUN_TO_IDLE
	1.0f,//	DOWN_DODGE,
	1.0f,//	DOWN_DODGE_TO_SLIDE,
	1.0f,//	DOWN_DODGE_TO_IDLE,
	1.0f,//	DODGE_ROLL,
	1.0f,//	UPPERCUT,
	1.0f,//	BASIC_CHAIN_B_PAUSEA,
	1.25f,//KICK_CHAIN_A,
	1.25f,//KICK_CHAIN_B,
	1.05f,//BACKWARD,
	2.5f,//BACKWARDTOFORWARD,
	1.0f,//BACKDROP_KICK,
	1.25f,//HEADBUTT,
	1.25f,//KICK_CHAIN_C,
	1.0f,//AIR_PUNCHA,
	1.0f,//AIR_PUNCHB,
	1.0f,//AIR_PUNCHC,
	1.0f,//ROULETTE,
	1.5f,//KNEEKICK,
	0.8f,//BASIC_CHAIN_B_KICKA,
	0.8f,//BASIC_CHAIN_B_KICKB,
	0.75f,//BASIC_CHAIN_B_KICKC,
	0.5f,//BASIC_CHAIN_B_KICKB_FORWARD,
	0.5f,//BASIC_CHAIN_B_KICKB_PUNCH,
	0.5f,//BUNBUN_FLOAT,
	1.5f,//KICKDOWN,
};

Player3D* pCurrentPlayer = nullptr;

//*****************************************************************************
// コンストラクタ関数
//*****************************************************************************
Player3D::Player3D() : Actor(PLAYER_MODEL, A_PLAYER)
{
	Init();
	fGravityForce = 0;
	pCurrentPlayer = this;
	Model->SetScale({ 0.5f, 0.5f, 0.5f });
	fPlayerSpeed = 1;
	nInputTimer = 0;
	pLight = GetMainLight();
	nState = PLAYER_IDLE_STATE;
	pFloor = nullptr;
	BunBun = new Object3D(GO_BUNBUN);
	BunBun->SetParent(this);
	BunBun->SetPosition({ 0, -100, 50 });
	BunBun->SetAnimation(BUN_BUN_APPEARS, 0.5f);
	pCurrentAttackPlaying = nullptr;
	pPreviousAttack = nullptr;
	nFightStanceFrameCount = 0;
	eDirection = DIR_NONE;
	nCancellingGravityFrames = 0;
	bUppercutExecute = bPressedForwardMidAttack = bPressedBackwardMidAttack = false;
	ResetInputs();
}

Player3D::Player3D(Light3D * Light) :Actor(PLAYER_MODEL, A_PLAYER)
{
	Init();
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
	Hitboxes[PLAYER_HB_FEET] = { 0, -5.0f, 0, 5.0f, 15.0f, 5.0f };
	Hitboxes[PLAYER_HB_ATTACK] = { 0, 25.0f, 0, 15.0f, 20.0f, 10.0f };
#if SHOW_HITBOX && SHOW_PLAYER_HITBOX
	for (int i = 0; i < PLAYER_HB_MAX; i++)
	{
		pVisualHitboxes[i] = new Cube3D();
		pVisualHitboxes[i]->Init("data/texture/hbox.tga");
		pVisualHitboxes[i]->SetScale({ Hitboxes[i].SizeX, Hitboxes[i].SizeY, Hitboxes[i].SizeZ });
		pVisualHitboxes[i]->SetPosition({ Hitboxes[i].PositionX,Hitboxes[i].PositionY,Hitboxes[i].PositionZ });
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
	if (IsOnTheFloor())
	{
		if (GetInput(INPUT_JUMP)) {
			pFloor = nullptr;
			Position.y += Hitboxes[PLAYER_HB_FEET].SizeY;
			fGravityForce = -JUMP_FORCE;
			nCancellingGravityFrames = 0;
			pCurrentAttackPlaying = nullptr;
			nState = PLAYER_JUMPING_UP_STATE;
		}
	}
	if (pFloor) {
		bRouletteExecuted = bUppercutExecute = false;
	}

	CalculateDirectionalInput();
	if (nState != PLAYER_IDLE_FIGHT_STATE)
		nFightStanceFrameCount = 0;

	SetAttackHitboxPosition(-sinf(XM_PI + Model->GetRotation().y) * 45,25.0f,-cosf(XM_PI + Model->GetRotation().y) * 45);
	Hitboxes[PLAYER_HB_ATTACK].SizeX = Hitboxes[PLAYER_HB_ATTACK].SizeY = Hitboxes[PLAYER_HB_ATTACK].SizeZ = 0;
	
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
		MoveControl();
		GravityControl();
		AttackInputsControl();
		if (pFloor)
			nState = PLAYER_IDLE_STATE;
		if(!GetInput(INPUT_JUMP_HOLD))
			nState = PLAYER_IDLE_STATE;
		break;
	default:
		if (!Model->GetLoops())
			printf("%d\n", Model->GetCurrentFrame());
		break;
	}
	InputResetControl();
	TransitionToFloatingBunBun();
	//printf("INPUTS: %s\n", szInputs);
}

void Player3D::TransitionToFloatingBunBun()
{
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
			if (GetInput(INPUT_KICK))
				SwitchAttack(SLIDE_KICKUP);
			if (Model->GetCurrentFrame()>=548)
			{
				pPreviousAttack = nullptr;
				SetAnimation(FIGHT_STANCE, fAnimationSpeed[FIGHT_STANCE]);
			}
		}
	}
	else {
		SetAnimation(AIR_IDLE, fAnimationSpeed[AIR_IDLE]);
		if (GetInput(INPUT_KICK)) {
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
	if (nFightStanceFrameCount < MAX_WAIT_TIMER && (GetInput(INPUT_PUNCH) || GetInput(INPUT_KICK)) && pPreviousAttack && !pPreviousAttack->ResetInputs)
	{
			switch (pPreviousAttack->Animation)
			{
			case BASIC_CHAIN_A:
				if (GetInput(INPUT_PUNCH))
					SwitchAttack(BASIC_CHAIN_B);
				else if (GetInput(INPUT_KICK))
					SwitchAttack(KICK_CHAIN_B);
				break;
			case BASIC_CHAIN_B:
				if (GetInput(INPUT_PUNCH))
					SwitchAttack(BASIC_CHAIN_C);
				else if (GetInput(INPUT_KICK))
					SwitchAttack(BASIC_CHAIN_B_KICKA);
				break;
			case KICK_CHAIN_A:
				if (GetInput(INPUT_PUNCH))
					SwitchAttack(BASIC_CHAIN_A);
				else if (GetInput(INPUT_KICK))
					SwitchAttack(KICK_CHAIN_B);
				break;
			case KICK_CHAIN_B:
				if (GetInput(INPUT_PUNCH))
					SwitchAttack(BASIC_CHAIN_C);
				else if (GetInput(INPUT_KICK))
					SwitchAttack(KICK_CHAIN_C);
				break;
			case AIR_PUNCHB:
				if (GetInput(INPUT_PUNCH)) {
					SwitchAttack(AIR_PUNCHC);
				}
				if (eDirection == DIR_BACKWARD)
					bPressedBackwardMidAttack = true;
				if (bPressedBackwardMidAttack && eDirection == DIR_FORWARD)
					bPressedForwardMidAttack = true;
				if (bPressedBackwardMidAttack && bPressedForwardMidAttack) {
					if (GetInput(INPUT_KICK))
						SwitchAttack(KNEEKICK);
					else if (GetInput(INPUT_PUNCH))
						SwitchAttack(UPPERCUT);
				}
				break;
			case AIR_PUNCHA:
				if (GetInput(INPUT_PUNCH)) {
					SwitchAttack(AIR_PUNCHB);
				}
				if (eDirection == DIR_BACKWARD)
					bPressedBackwardMidAttack = true;
				if (bPressedBackwardMidAttack && eDirection == DIR_FORWARD)
					bPressedForwardMidAttack = true;
				if (bPressedBackwardMidAttack && bPressedForwardMidAttack) {
					if(GetInput(INPUT_KICK))
						SwitchAttack(KNEEKICK);
					else if(GetInput(INPUT_PUNCH))
						SwitchAttack(UPPERCUT);
				}
				break;
			case KNEEKICK:
				bPressedForwardMidAttack = bPressedBackwardMidAttack = false;
				break;
			default:
				AttackInputsControl();
				//FindMoveByAnimation(pPreviousAttack->nNextChain);
				return;
			}
	}
	else if ((nFightStanceFrameCount >= MAX_WAIT_TIMER && nFightStanceFrameCount <= MAX_WAIT_TIMER +13) && (GetInput(INPUT_PUNCH) || GetInput(INPUT_KICK)) && pPreviousAttack && !pPreviousAttack->ResetInputs)//WAIT INPUT
	{
			switch (pPreviousAttack->Animation)
			{
			case BASIC_CHAIN_A:
				if (GetInput(INPUT_PUNCH)) {
					SwitchAttack(UPPERCUT);
				}
				break;
			case BASIC_CHAIN_B:
				if (GetInput(INPUT_PUNCH)) {
					SwitchAttack(BASIC_CHAIN_B_PAUSEA);
				}
				break;
			case AIR_PUNCHB:
				if (GetInput(INPUT_PUNCH)) {
					SwitchAttack(ROULETTE);
					break;
				}
				if (eDirection == DIR_BACKWARD)
					bPressedBackwardMidAttack = true;
				if (bPressedBackwardMidAttack && eDirection == DIR_FORWARD)
					bPressedForwardMidAttack = true;
				if (bPressedBackwardMidAttack && bPressedForwardMidAttack) {
					if (GetInput(INPUT_KICK))
						SwitchAttack(KNEEKICK);
					else if (GetInput(INPUT_PUNCH))
						SwitchAttack(UPPERCUT);
				}
				break;
			case AIR_PUNCHA:
				if (GetInput(INPUT_PUNCH)) {
					SwitchAttack(AIR_PUNCHB);
				}
				if (eDirection == DIR_BACKWARD)
					bPressedBackwardMidAttack = true;
				if (bPressedBackwardMidAttack && eDirection == DIR_FORWARD)
					bPressedForwardMidAttack = true;
				if (bPressedBackwardMidAttack && bPressedForwardMidAttack) {
					if (GetInput(INPUT_KICK))
						SwitchAttack(KNEEKICK);
					else if (GetInput(INPUT_PUNCH))
						SwitchAttack(UPPERCUT);
				}
				break;
			case KICK_CHAIN_A:
				if (GetInput(INPUT_PUNCH))
					SwitchAttack(BASIC_CHAIN_A);
				else if (GetInput(INPUT_KICK))
					SwitchAttack(KICK_CHAIN_B);
				break;
			case KICK_CHAIN_B:
				if (GetInput(INPUT_PUNCH))
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
		if (GetInput(INPUT_PUNCH))
			SwitchAttack(BASIC_CHAIN_A);
		else if(GetInput(INPUT_KICK))
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
	static bool bFirstSetOfPunches = false;
	if (GetInput(INPUT_KICK) && !IsOnTheFloor()) 
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
		bFirstSetOfPunches = false;
		return;
	}
	if(pCurrentAttackPlaying->Animation != BASIC_CHAIN_A &&
		pCurrentAttackPlaying->Animation != BASIC_CHAIN_B &&
		pCurrentAttackPlaying->Animation != BASIC_CHAIN_C && 
		pCurrentAttackPlaying->Animation != AIR_PUNCHA &&
		pCurrentAttackPlaying->Animation != AIR_PUNCHB &&
		pCurrentAttackPlaying->Animation != AIR_PUNCHC)
		bFirstSetOfPunches = false;

	SetAnimation(pCurrentAttackPlaying->Animation, fAnimationSpeed[pCurrentAttackPlaying->Animation]);
	XMFLOAT3 rotCamera = Model->GetRotation();
	if (Model->GetCurrentFrame() > pCurrentAttackPlaying->nMinFrameForInputDetection 
		&& pCurrentAttackPlaying->nMinFrameForInputDetection!=-1)
	{
		if(!PressedPunch)
			PressedPunch = GetInput(INPUT_PUNCH);
		if(!PressedKick)
			PressedKick = GetInput(INPUT_KICK);
	}
	else {
		PressedKick = PressedPunch = false;
	}
	if (Model->GetCurrentFrame() > pCurrentAttackPlaying->fpHitBoxActivation.InitialFrame && Model->GetCurrentFrame() < pCurrentAttackPlaying->fpHitBoxActivation.EndFrame)
		ActivateAttackHitbox(pCurrentAttackPlaying->ahsHitboxSize.x, pCurrentAttackPlaying->ahsHitboxSize.y, pCurrentAttackPlaying->ahsHitboxSize.z, pCurrentAttackPlaying->ahsHitboxSize.speed);

	switch (pCurrentAttackPlaying->Animation)
	{
	case BASIC_CHAIN_A:
		if (GetInput(INPUT_KICK) || PressedKick) {
			SwitchAttack(KICK_CHAIN_A);

		}else if (GetInput(INPUT_PUNCH) || PressedPunch) {
			if (Model->GetCurrentFrame() > 1132 && Model->GetCurrentFrame() < 1155) {
				SwitchAttack(BASIC_CHAIN_B);
				break;
			}
		}
		if (Model->GetCurrentFrame() >= 1155) 
			StopAttack();

		break;
	case BASIC_CHAIN_B:
		if (GetInput(INPUT_PUNCH)) {
			if (Model->GetCurrentFrame() > 1225) {
				if (!bFirstSetOfPunches)
				{
					bFirstSetOfPunches = true;
					SwitchAttack(BASIC_CHAIN_A);
					break;
				}
				SwitchAttack(BASIC_CHAIN_C);
				break;
			}
		}
		if (GetInput(INPUT_KICK) || PressedKick) {
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
		SetAttackHitboxPosition(-sinf(XM_PI + Model->GetRotation().y) * 35, 50, -cosf(XM_PI + Model->GetRotation().y) * 35);
		break;
	case HEADBUTT:
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
			if (GetInput(INPUT_KICK) || PressedKick) {
				SwitchAttack(SLIDE_KICKUP);
				break;
			}
		}
		if (Model->GetCurrentFrame() >= 527)
		{
			StopAttack();
		}
		SetAttackHitboxPosition(-sinf(XM_PI + Model->GetRotation().y) * 45, 0.0f, -cosf(XM_PI + Model->GetRotation().y) * 25);
		break;
	case BASIC_CHAIN_B_PAUSEA:
		if (Model->GetCurrentFrame() >= 2086)
		{
			StopAttack();
		}
		break;
	case KICK_CHAIN_A:
		if (GetInput(INPUT_PUNCH) || PressedPunch) {
			if (Model->GetCurrentFrame() > 2168 && Model->GetCurrentFrame() < 2205) {
				SwitchAttack(BASIC_CHAIN_B);
				break;
			}
		}
		if (GetInput(INPUT_KICK) || PressedKick) {
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
		if (GetInput(INPUT_PUNCH) || PressedPunch) {
			if (Model->GetCurrentFrame() > 2301 && Model->GetCurrentFrame() < 2315) {
				SwitchAttack(BASIC_CHAIN_C);
				break;
			}
		}
		if (GetInput(INPUT_KICK) || PressedKick) {
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
		if (Model->GetCurrentFrame() >= 2772) {
			StopAttack();
		}
		break;
	case SLIDE_KICKUP:
		if (Model->GetCurrentFrame() >= 632 && Model->GetCurrentFrame() <= 690) {
			fGravityForce += GRAVITY_FORCE;
			Position.y -= fGravityForce;
		}
		else {
			fGravityForce = -JUMP_FORCE*1.25f;
			if (Model->GetCurrentFrame() >= 734)
			{
				fGravityForce = 0;
				StopAttack();
				nState = PLAYER_IDLE_STATE;
			}
		}
		break;
	case BASIC_CHAIN_B_KICKA:
		if (Model->GetCurrentFrame() >= 3553)
		{
			if (GetInput(INPUT_KICK) || PressedKick) {
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
		if (eDirection == DIR_FORWARD)
		{
			bPressedForwardMidAttack = true;
		}
		if (Model->GetCurrentFrame() >= 3649)
		{
			if (GetInput(INPUT_KICK) || PressedKick) {
				if (bPressedForwardMidAttack)
				{
					SwitchAttack(BASIC_CHAIN_B_KICKB_FORWARD);
					break;
				}
				SwitchAttack(BASIC_CHAIN_B_KICKC);
				break;
			}
			if (GetInput(INPUT_PUNCH) || PressedPunch)
			{
				SwitchAttack(BASIC_CHAIN_B_KICKB_PUNCH);
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
		CalculateDirectionalInput();
		if (eDirection == DIR_BACKWARD)
			bPressedBackwardMidAttack = true;
		if (bPressedBackwardMidAttack && eDirection == DIR_FORWARD)
			bPressedForwardMidAttack = true;
		if (GetInput(INPUT_KICK) || PressedKick)
		{
			if (bPressedBackwardMidAttack && bPressedForwardMidAttack)
			{
				SwitchAttack(KNEEKICK);
				break;
			}
			
		}
		if (Model->GetCurrentFrame() >= 3122)
		{
			if (GetInput(INPUT_PUNCH) || PressedPunch)
			{
				SwitchAttack(AIR_PUNCHB);
				break;
			}
		}
		if (Model->GetCurrentFrame() >= 3138)
			StopAttack();
		break;
	case AIR_PUNCHB:
		CalculateDirectionalInput();
		if (eDirection == DIR_BACKWARD)
			bPressedBackwardMidAttack = true;
		if (bPressedBackwardMidAttack && eDirection == DIR_FORWARD)
			bPressedForwardMidAttack = true;
		if (GetInput(INPUT_KICK) || PressedKick)
		{
			if (bPressedBackwardMidAttack && bPressedForwardMidAttack)
			{
				SwitchAttack(KNEEKICK);
				break;
			}

		}
		if (Model->GetCurrentFrame() >= 3205)
		{
			if (GetInput(INPUT_PUNCH))
			{
				if (!bFirstSetOfPunches)
				{
					bFirstSetOfPunches = true;
					SwitchAttack(AIR_PUNCHA);
					break;
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
		if (Model->GetCurrentFrame() < 3335)
		{
			x3OriginalRotation = Model->GetRotation();
		}
		else if (Model->GetCurrentFrame() > 3335 && Model->GetCurrentFrame() < 3350)
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
			}
		}
		//
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
		if (Model->GetCurrentFrame() >= 3408)
			StopAttack();
		break;
	case KNEEKICK:
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
		if(pFloor && Model->GetCurrentFrame() <= 4200)
			Model->SetFrameOfAnimation(4201);
		break;
	default:
		break;
	}
}

void Player3D::SetAttackHitboxPosition(float x, float y, float z)
{
	Hitboxes[PLAYER_HB_ATTACK].PositionX = x;
	Hitboxes[PLAYER_HB_ATTACK].PositionY = y;
	Hitboxes[PLAYER_HB_ATTACK].PositionZ = z;
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
	float DirInput =abs((nModelRotation + Rotation.y) - x3CurrentModelRot.y);
	if (DirInput >= 3.05f)
	{
		eDirection = DIR_BACKWARD;
		AddInput('B');
		nInputTimer = MAX_INPUT_TIMER /2;
#if DEBUG_DIRECTIONALS
		printf("BACKWARD\n", DirInput);
#endif
	}
	else if (DirInput<=0.5f)
	{
		AddInput('F');
		nInputTimer = MAX_INPUT_TIMER / 2;
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
			return;
		}
	}
	
}

void Player3D::MoveControl()
{
	static float RotTarget = 0;
	//アニメーション
	if (nState != PLAYER_JUMPING_UP_STATE && nState != PLAYER_BUNBUN_FLOATING) {
		if (IsOnTheFloor()) {
			if (eDirection == DIR_BACKWARD) {
				SetAnimation(BACKWARD, fAnimationSpeed[BACKWARD]);
			}
			else if(Model->GetCurrentAnimation() != BACKWARD && Model->GetCurrentAnimation() != BACKWARDTOFORWARD){
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
	if (Model->GetCurrentAnimation() == BACKWARD)
	{
		if(eDirection==DIR_FORWARD)
			SetAnimation(BACKWARDTOFORWARD, fAnimationSpeed[BACKWARDTOFORWARD]);
	}
	if ((Model->GetCurrentAnimation() == BACKWARD && Model->GetCurrentFrame() >= 2434)
		|| (Model->GetCurrentAnimation() == BACKWARDTOFORWARD && Model->GetCurrentFrame() >= 2593))
	{
		SetAnimation(ANIMATION_WALKING, fAnimationSpeed[ANIMATION_WALKING]);
		if (GetAxis(MOVEMENT_AXIS_HORIZONTAL) == 0 && GetAxis(MOVEMENT_AXIS_VERTICAL) == 0)
		{
			SetAnimation(ANIMATION_IDLE, fAnimationSpeed[ANIMATION_IDLE]);
			nState = PLAYER_IDLE_STATE;
			Model->SetRotationY(RotTarget);
			return;
		}
	}
	//動く
	float fHorizontalAxis = GetAxis(MOVEMENT_AXIS_HORIZONTAL);
	float fVerticalAxis = GetAxis(MOVEMENT_AXIS_VERTICAL);
	float nModelRotation = -(atan2(fVerticalAxis, fHorizontalAxis) - 1.570796f);
	fPlayerSpeed += 0.5f;
	if (fPlayerSpeed >= MAX_SPEED)
		fPlayerSpeed = MAX_SPEED;
	XMFLOAT3 rotCamera = pMainCamera->GetCameraAngle();
	if (fVerticalAxis != 0) {
		Position.x -= sinf(XM_PI + rotCamera.y) * fPlayerSpeed * fVerticalAxis;
		Position.z -= cosf(XM_PI + rotCamera.y) * fPlayerSpeed * fVerticalAxis;
	}
	if (fHorizontalAxis != 0) {
		Position.x -= sinf(rotCamera.y - XM_PI * 0.50f) * fPlayerSpeed * fHorizontalAxis;
		Position.z -= cosf(rotCamera.y - XM_PI * 0.50f) * fPlayerSpeed * fHorizontalAxis;
	}
	
	if (GetAxis(MOVEMENT_AXIS_HORIZONTAL) || GetAxis(MOVEMENT_AXIS_VERTICAL))
		RotTarget = nModelRotation + Rotation.y;
	if (!GetAxis(MOVEMENT_AXIS_HORIZONTAL) && !GetAxis(MOVEMENT_AXIS_VERTICAL) && Model->GetCurrentAnimation() == BACKWARD && Model->GetCurrentFrame() < 2387)
	{
		nState = PLAYER_IDLE_STATE;
		SetAnimation(ANIMATION_IDLE, fAnimationSpeed[ANIMATION_IDLE]);
		return;
	}
	if (fVerticalAxis != 0 || fHorizontalAxis != 0 || (Model->GetCurrentAnimation() == BACKWARD && abs(Model->GetRotation().y - RotTarget) > 0.15f))
	{
		//XMFLOAT3 x3CurrentModelRot = Model->GetRotation();
		//if (!GetInput(INPUT_LOCKON))
		if(Model->GetCurrentAnimation()!=BACKWARD)
			Model->SetRotationY(nModelRotation + Rotation.y);
		else 
		{
			if (Model->GetCurrentFrame() > 2387)
			{
				if (abs(Model->GetRotation().y - RotTarget) > 0.15f) {
					if(Model->GetRotation().y> RotTarget)
						Model->RotateAroundY(-0.18f);
					else
						Model->RotateAroundY(0.18f);
				}
			}
		}

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
#if SHOW_HITBOX && SHOW_PLAYER_HITBOX
	GetMainLight()->SetLightEnable(false);
#if SHOW_SPECIFIC_PLAYER_HITBOX < 0
	for (int i = 0; i < PLAYER_HB_MAX; i++)
	{
		if (!pVisualHitboxes[i])continue;
		Box pHB = GetHitboxPlayer(i);
		pVisualHitboxes[i]->SetScale({ pHB.SizeX, pHB.SizeY, pHB.SizeZ });
		pVisualHitboxes[i]->SetPosition({ pHB.PositionX, pHB.PositionY, pHB.PositionZ });
		pVisualHitboxes[i]->Draw();
	}
#else
	if (!pVisualHitboxes[SHOW_SPECIFIC_PLAYER_HITBOX]) {
		Box pHB = GetHitboxPlayer(SHOW_SPECIFIC_PLAYER_HITBOX);
		pVisualHitboxes[SHOW_SPECIFIC_PLAYER_HITBOX]->SetScale({ pHB.SizeX, pHB.SizeY, pHB.SizeZ });
		pVisualHitboxes[SHOW_SPECIFIC_PLAYER_HITBOX]->SetPosition({ pHB.PositionX, pHB.PositionY, pHB.PositionZ });
		pVisualHitboxes[SHOW_SPECIFIC_PLAYER_HITBOX]->Draw();
	}	
#endif
	GetMainLight()->SetLightEnable(true);
#endif
	Actor::Draw();
	if(nState == PLAYER_BUNBUN_FLOATING)
		BunBun->Draw();
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
			if (GetAxis(MOVEMENT_AXIS_VERTICAL) != 0 || GetAxis(MOVEMENT_AXIS_HORIZONTAL) != 0) {
				float fHorizontalAxis = GetAxis(MOVEMENT_AXIS_HORIZONTAL);
				float fVerticalAxis = GetAxis(MOVEMENT_AXIS_VERTICAL);
				float nModelRotation = -(atan2(fVerticalAxis, fHorizontalAxis) - 1.570796f);
				Model->SetRotationY(nModelRotation + Rotation.y);
			}
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
	if (nState == PLAYER_ATTACKING_STATE)
		return;

	if (GetInput(INPUT_PUNCH))
	{
		nInputTimer = 0;
		AddInput('A');
		Attack(szInputs);
		if (Model->GetCurrentAnimation() == BACKWARDTOFORWARD)
			SwitchAttack(UPPERCUT);
	}
	if (GetInput(INPUT_KICK))
	{
		nInputTimer = 0;
		AddInput('K');
		Attack(szInputs);
		if (Model->GetCurrentAnimation() == BACKWARDTOFORWARD)
			SwitchAttack(BACKDROP_KICK);
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

float Player3D::GetGravityForce()
{
	return fGravityForce;
}
