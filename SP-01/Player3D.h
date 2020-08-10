//*****************************************************************************
// Player.h
//*****************************************************************************
#ifndef	PLAYER3D_H
#define PLAYER3D_H
#include "Actor.h"
#include "Cube3D.h"
#include "Billboard2D.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define MAX_BULLETS 40
#define MAX_PLAYER_INPUT 8
#define	MAX_FLOWERS	20

//*****************************************************************************
// エナム
//*****************************************************************************
enum PLAYER_ANIMATIONS
{
	ANIMATION_IDLE = 0,//アイドル
	ANIMATION_WALKING_START,//動く開始 
	ANIMATION_WALKING,//動く 
	SLIDE,
	SLIDE_STANDUP_FIGHT,
	SLIDE_STANDUP,
	SLIDE_KICKUP,
	AIR_IDLE,
	JUMP_UP,
	JUMP_UP_TO_FALLDOWN,
	FIGHT_STANCE,
	BASIC_CHAIN_A,
	BASIC_CHAIN_B,
	BASIC_CHAIN_C,
	RUN_TO_IDLE,
	DOWN_DODGE,
	DOWN_DODGE_TO_SLIDE,
	DOWN_DODGE_TO_IDLE,
	DODGE_ROLL,
	UPPERCUT,
	BASIC_CHAIN_B_PAUSEA,
	KICK_CHAIN_A,
	KICK_CHAIN_B,
	BACKWARD,
	BACKWARDTOFORWARD,
	BACKDROP_KICK,
	HEADBUTT,
	KICK_CHAIN_C,
	AIR_PUNCHA,
	AIR_PUNCHB,
	AIR_PUNCHC,
	ROULETTE,
	KNEEKICK,
	BASIC_CHAIN_B_KICKA,
	BASIC_CHAIN_B_KICKB,
	BASIC_CHAIN_B_KICKC,
	BASIC_CHAIN_B_KICKB_FORWARD,
	BASIC_CHAIN_B_KICKB_PUNCH,
	BUNBUN_FLOAT,
	KICKDOWN,
	MAX_ANIMATIONS
};
enum EPLAYER_STATE
{
	PLAYER_IDLE_STATE=0,
	PLAYER_IDLE_FIGHT_STATE,
	PLAYER_MOVING_STATE,
	PLAYER_ATTACKING_STATE,
	PLAYER_JUMPING_UP_STATE,
	PLAYER_BUNBUN_FLOATING,
	PLAYER_STATE_MAX
};
enum EPLAYER_HITBOXES
{
	PLAYER_HB_FEET=0,
	PLAYER_HB_ATTACK,
	PLAYER_HB_MAX
};
enum AirMove
{
	BOTH_MOVES = 0,
	GROUND_MOVE = 1,
	AIR_MOVE = -1
};

enum PLAYER_DIRECTION
{
	DIR_NONE,
	DIR_FORWARD,
	DIR_BACKWARD,
	DIR_OTHER,
};
typedef struct HITBOX_ACTIVATION_FRAME
{
	float InitialFrame;
	float EndFrame;
};
typedef struct ATTACK_HITBOX_SIZE
{
	float x,y,z;
	float speed;
};
//*****************************************************************************
// 構造体定義
//*****************************************************************************
typedef struct PLAYER_ATTACK_MOVE
{
	char					Input[MAX_PLAYER_INPUT];
	int						Animation;
	bool					ResetInputs;
	AirMove					eAirMove;
	int						nNextChain;
	int						nMinFrameForInputDetection;
	HITBOX_ACTIVATION_FRAME fpHitBoxActivation;
	ATTACK_HITBOX_SIZE		ahsHitboxSize;
	int						nAttackID;
};

//*****************************************************************************
// クラス
//*****************************************************************************
class Player3D :
	public Actor
{
private:
	float fPlayerSpeed;
	int nState;
	Object3D* pFloor;
	char szInputs[MAX_PLAYER_INPUT + 1];
	PLAYER_ATTACK_MOVE* pCurrentAttackPlaying;
	PLAYER_ATTACK_MOVE* pPreviousAttack;
	//float fAnalogLockInput;
	float fGravityForce;
	int nInputTimer;
	int nCancellingGravityFrames;
	int nFightStanceFrameCount;
	//ヒットボックス
	Box Hitboxes[PLAYER_HB_MAX];
	Cube3D* pVisualHitboxes[PLAYER_HB_MAX];
	PLAYER_DIRECTION eDirection;
	bool bPressedForwardMidAttack, bPressedBackwardMidAttack;
	bool bUppercutExecute, bRouletteExecuted;
	Object3D* BunBun;
public:
	Player3D();
	Player3D(Light3D* Light);
	~Player3D();

	void Init();
	void Update();
	void TransitionToFloatingBunBun();
	void JumpingStateControl();
	void GravityControl();
	void IdleStateControl();
	void FightingStanceStateControl();
	void FindMoveByAnimation(int anim);
	PLAYER_ATTACK_MOVE* GetAttack(int anim);
	void AttackStateControl();
	void SetAttackHitboxPosition(float x, float y, float z);
	void ActivateAttackHitbox(float x, float y, float z, float Speed);
	void StopAttack();
	void CalculateDirectionalInput();
	void SwitchAttack(int nNextAttack);
	void MoveControl();
	void Draw();
	void End();
	void Attack(const char * atkInput);
	void Attack(const char * atkInput, int recursions);
	bool IsOnTheFloor();
	void ResetInputs();
	void AttackInputsControl();
	void InputResetControl();
	void AddInput(char A);
	char GetLastInputInserted();
	Box GetHitboxPlayer(int hb);
	void SetFloor(Object3D* Floor) { pFloor = Floor; fGravityForce = 0; };
};

Player3D* GetPlayer();
#endif