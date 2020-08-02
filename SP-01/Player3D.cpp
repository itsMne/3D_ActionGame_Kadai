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
#define DEBUG_FOUND_ATTACKS true
#define VERIFY_ISONFLOOR true
#define DEBUG_ADD_INPUTS false
#define MAX_SPEED 6.8f
#define GRAVITY_FORCE 0.98f
#define SHOW_PLAYER_HITBOX true
#define SHOW_SPECIFIC_PLAYER_HITBOX -1
#define JUMP_FORCE 20
//*****************************************************************************
// グローバル変数
//*****************************************************************************
Player3D* pMainPlayer3D = nullptr;
PLAYER_ATTACK_MOVE stAllMoves[MAX_ATTACKS] =
{
	{"A",	  BASIC_CHAIN_A,				false,	GROUND_MOVE, BASIC_CHAIN_B,			100},
	{"N",	  BASIC_CHAIN_B,				false,	GROUND_MOVE, BASIC_CHAIN_C,			200},//AA
	{"N",	  BASIC_CHAIN_C,				true,	GROUND_MOVE, MAX_ANIMATIONS,		300},//AAA
	{"BFA",	  UPPERCUT,						true,	GROUND_MOVE, MAX_ANIMATIONS,			400},//BFA
	{"BBA",	  UPPERCUT,						true,	AIR_MOVE,	 MAX_ANIMATIONS,			400},//BFA
	{"FFA",	  HEADBUTT,						true,	GROUND_MOVE, MAX_ANIMATIONS,		400},//BFA
	{"BFK",	  BACKDROP_KICK,				true,	GROUND_MOVE, MAX_ANIMATIONS,		400},//BFK
	{"FFK",	  SLIDE,						true,	GROUND_MOVE, MAX_ANIMATIONS,		400},//BFK
	{"N",	  BASIC_CHAIN_B_PAUSEA,			true,	GROUND_MOVE, MAX_ANIMATIONS,		500},//AAPA
	{"K",	  KICK_CHAIN_A,					false,	GROUND_MOVE, KICK_CHAIN_B,			600},//K
	{"N",	  KICK_CHAIN_B,					false,	GROUND_MOVE, BASIC_CHAIN_C,			700},//KK
	{"N",	  KICK_CHAIN_C,					true,	GROUND_MOVE, MAX_ANIMATIONS,		800},//KKK
	{"N",	  SLIDE_KICKUP,					true,	GROUND_MOVE, MAX_ANIMATIONS,		900},//
	{"N",	  BASIC_CHAIN_B_KICKA,			true,	GROUND_MOVE, MAX_ANIMATIONS,		1000},//AAK
	{"N",	  BASIC_CHAIN_B_KICKB,			true,	GROUND_MOVE, MAX_ANIMATIONS,		1100},//AAKK
	{"N",	  BASIC_CHAIN_B_KICKC,			true,	GROUND_MOVE, MAX_ANIMATIONS,		1200},//AAKK
	{"N",	  BASIC_CHAIN_B_KICKB_PUNCH,	true,	GROUND_MOVE, MAX_ANIMATIONS,		1300},//AAKFK
	{"N",	  BASIC_CHAIN_B_KICKB_FORWARD,	true,	GROUND_MOVE, MAX_ANIMATIONS,		1400},//AAKA
	{"A",	  AIR_PUNCHA,					false,	AIR_MOVE,	 AIR_PUNCHB,			1500},//A
	{"N",	  AIR_PUNCHB,					false,	AIR_MOVE,	 AIR_PUNCHC,			1600},//A
	{"N",	  AIR_PUNCHC,					true,	AIR_MOVE,	 MAX_ANIMATIONS,		1700},//A
	{"BBK",	  KNEEKICK,						true,	AIR_MOVE,	 MAX_ANIMATIONS,		1800},//A
	{"N",	  ROULETTE,						true,	AIR_MOVE,	 MAX_ANIMATIONS,		1900},//A
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
	1.25f,//	BACKWARD,
	2.5f,//BACKWARDTOFORWARD,
	1.0f,//BACKDROP_KICK,
	1.25f,//HEADBUTT,
	1.25f,//KICK_CHAIN_C,

	1.0,//AIR_PUNCHA,
	1.0,//AIR_PUNCHB,
	1.0,//AIR_PUNCHC,
	1.0,//ROULETTE,
	0.5,//KNEEKICK,
	1.0,//BASIC_CHAIN_B_KICKA,
	1.0,//BASIC_CHAIN_B_KICKB,
	1.0,//BASIC_CHAIN_B_KICKC,
	1.0,//BASIC_CHAIN_B_KICKB_FORWARD,
	1.0,//BASIC_CHAIN_B_KICKB_PUNCH,
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
	pCurrentAttackPlaying = nullptr;
	pPreviousAttack = nullptr;
	nFightStanceFrameCount = 0;
	eDirection = DIR_NONE;
	nCancellingGravityFrames = 0;
	bPressedForwardMidAttack = bPressedBackwardMidAttack = false;
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
			fGravityForce -= JUMP_FORCE;
			pCurrentAttackPlaying = nullptr;
			nState = PLAYER_JUMPING_UP_STATE;
		}
	}


	CalculateDirectionalInput();

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
		if (fGravityForce<0 && Model->GetCurrentFrame() > 894) {
			GravityControl();
			MoveControl();
		}
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
		if(Model->GetCurrentAnimation()==JUMP_UP && fGravityForce==0)
			SetAnimation(JUMP_UP_TO_FALLDOWN, fAnimationSpeed[JUMP_UP_TO_FALLDOWN]);
		if (fGravityForce > 0)
				fGravityForce = 0;
		break;
	default:
		if (!Model->GetLoops())
			printf("%d\n", Model->GetCurrentFrame());
		break;
	}
	//printf("STATE: %d\n", nState);
	InputResetControl();
	//printf("INPUTS: %s\n", szInputs);
}

void Player3D::GravityControl()
{
	if (IsOnTheFloor()) {
		if (!IsInCollision3D(GetHitboxPlayer(PLAYER_HB_FEET), pFloor->GetHitbox()))
			pFloor = nullptr;
		return;
	}
	if (--nCancellingGravityFrames > 0)
		return;
	nCancellingGravityFrames = 0;
	fGravityForce += GRAVITY_FORCE;
	Position.y -= fGravityForce;
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
	else 
		SetAnimation(AIR_IDLE, fAnimationSpeed[AIR_IDLE]);
	if (Model->GetLoops() > 1)
		nState = PLAYER_IDLE_STATE;
	if (GetAxis(MOVEMENT_AXIS_HORIZONTAL) || GetAxis(MOVEMENT_AXIS_VERTICAL))
		nState = PLAYER_MOVING_STATE;
	nFightStanceFrameCount++;
	if(GetInput(INPUT_PUNCH))
		printf("%d\n", nFightStanceFrameCount);
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
				FindMoveByAnimation(pPreviousAttack->nNextChain);
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
	else
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
	if (Model->GetLoops() > 0) {
		StopAttack();
	}
	if (!pCurrentAttackPlaying) {
		fGravityForce = 0;
		nCancellingGravityFrames = 15;
		nState = PLAYER_IDLE_FIGHT_STATE;
		nFightStanceFrameCount = 0;
		return;
	}
	SetAnimation(pCurrentAttackPlaying->Animation, fAnimationSpeed[pCurrentAttackPlaying->Animation]);
	XMFLOAT3 rotCamera = Model->GetRotation();

	switch (pCurrentAttackPlaying->Animation)
	{
	case BASIC_CHAIN_A:
		if (GetInput(INPUT_KICK))
			SwitchAttack(KICK_CHAIN_A);
		if (GetInput(INPUT_PUNCH)) {
			if (Model->GetCurrentFrame() > 1132 && Model->GetCurrentFrame() < 1155) {
				SwitchAttack(BASIC_CHAIN_B);
				break;
			}
		}
		if (Model->GetCurrentFrame() >= 1155) {
			StopAttack();
		}
		break;
	case BASIC_CHAIN_B:
		if (GetInput(INPUT_PUNCH)) {
			if (Model->GetCurrentFrame() > 1220 && Model->GetCurrentFrame() < 1245) {
				SwitchAttack(BASIC_CHAIN_C);
				break;
			}
		}
		if (GetInput(INPUT_KICK)) {
			if (Model->GetCurrentFrame() > 1220 && Model->GetCurrentFrame() < 1245) {
				SwitchAttack(BASIC_CHAIN_B_KICKA);
				break;
			}
		}
		if (Model->GetCurrentFrame() >= 1245) {
			StopAttack();
		}

		break;
	case BASIC_CHAIN_C:
		if (Model->GetCurrentFrame() >= 1356) {
			StopAttack();
		}
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
			//printf("%f\n", fGravityForce);
			fGravityForce += GRAVITY_FORCE;
			Position.y -= fGravityForce;
		}
		if (Model->GetCurrentFrame() >= 1848)
		{
			StopAttack();
		}
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
		if (Model->GetCurrentFrame() >= 516)
		{
			if(GetInput(INPUT_KICK))
				SwitchAttack(SLIDE_KICKUP);
		}
		if (Model->GetCurrentFrame() >= 527)
		{
			StopAttack();
		}
		break;
	case BASIC_CHAIN_B_PAUSEA:
		if (Model->GetCurrentFrame() >= 2086)
		{
			StopAttack();
		}
		break;
	case KICK_CHAIN_A:
		if (GetInput(INPUT_PUNCH)) {
			if (Model->GetCurrentFrame() > 2168 && Model->GetCurrentFrame() < 2205) {
				SwitchAttack(BASIC_CHAIN_B);
				break;
			}
		}
		if (GetInput(INPUT_KICK)) {
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
		if (GetInput(INPUT_PUNCH)) {
			if (Model->GetCurrentFrame() > 2301 && Model->GetCurrentFrame() < 2315) {
				SwitchAttack(BASIC_CHAIN_C);
				break;
			}
		}
		if (GetInput(INPUT_KICK)) {
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
			}
		}
		break;
	case BASIC_CHAIN_B_KICKA:
		if (Model->GetCurrentFrame() >= 3553)
		{
			if (GetInput(INPUT_KICK)) {
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
			if (GetInput(INPUT_KICK)) { 
				if (bPressedForwardMidAttack)
				{
					SwitchAttack(BASIC_CHAIN_B_KICKB_FORWARD);
					break;
				}
				SwitchAttack(BASIC_CHAIN_B_KICKC);
				break;
			}
			if (GetInput(INPUT_PUNCH))
			{
				SwitchAttack(BASIC_CHAIN_B_KICKB_PUNCH);
				break;
			}
		}
		if (Model->GetCurrentFrame() >= 3668)
		{
			StopAttack();
		}
		break;
	case BASIC_CHAIN_B_KICKC:
		if (Model->GetCurrentFrame() >= 3798)
		{
			StopAttack();
		}
		break;
	case AIR_PUNCHA:
		CalculateDirectionalInput();
		if (eDirection == DIR_BACKWARD)
			bPressedBackwardMidAttack = true;
		if (bPressedBackwardMidAttack && eDirection == DIR_FORWARD)
			bPressedForwardMidAttack = true;
		if (GetInput(INPUT_KICK))
		{
			if (bPressedBackwardMidAttack && bPressedForwardMidAttack)
			{
				SwitchAttack(KNEEKICK);
				break;
			}
			
		}
		if (Model->GetCurrentFrame() >= 3122)
		{
			if (GetInput(INPUT_PUNCH))
			{
				SwitchAttack(AIR_PUNCHB);
				break;
			}
		}
		if (Model->GetCurrentFrame() >= 3138)
		{
			StopAttack();
		}
		break;
	case AIR_PUNCHB:
		CalculateDirectionalInput();
		if (eDirection == DIR_BACKWARD)
			bPressedBackwardMidAttack = true;
		if (bPressedBackwardMidAttack && eDirection == DIR_FORWARD)
			bPressedForwardMidAttack = true;
		if (GetInput(INPUT_KICK))
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
		if (Model->GetCurrentFrame() >= 3330)
		{
			StopAttack();
		}
		break;
	default:
		break;
	}
}

void Player3D::StopAttack()
{
	if(pCurrentAttackPlaying)
		pPreviousAttack = GetAttack(pCurrentAttackPlaying->Animation);
	printf("PREVIOUS ATTACK: %d\n", pPreviousAttack->Animation);
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
	if (DirInput >= 2.75f)
	{
		eDirection = DIR_BACKWARD;
		AddInput('B');
		nInputTimer = MAX_INPUT_TIMER /2;
		printf("BACKWARD\n", DirInput);
	}
	else if (DirInput<=0.5f)
	{
		AddInput('F');
		nInputTimer = MAX_INPUT_TIMER / 2;
		eDirection = DIR_FORWARD;
		printf("FORWARD\n", DirInput);
	}
	else {
		eDirection = DIR_OTHER;
		printf("OTHER\n", DirInput);
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
	if (nState != PLAYER_JUMPING_UP_STATE) {
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
						Model->RotateAroundY(-0.25f);
					else
						Model->RotateAroundY(0.25f);
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
}

//*****************************************************************************
//End関数
//終了関数
//引数：void
//戻：void
//*****************************************************************************
void Player3D::End()
{
	

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
			pCurrentAttackPlaying = &stAllMoves[i];
			nState = PLAYER_ATTACKING_STATE;
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
	if (pCurrentAttackPlaying)
	{
		if (nState == PLAYER_ATTACKING_STATE)
			return;
	}

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
	//printf("%s\n", szInputs);
}

void Player3D::InputResetControl()
{
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