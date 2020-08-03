//*****************************************************************************
// GameObject.h
//*****************************************************************************
#pragma once
#include "UniversalStructures.h"
#include "Model3D.h"
#include "Light3D.h"

//*****************************************************************************
// �G�i��
//*****************************************************************************
enum eGameObjectTypes
{
	GO_ACTOR = 0,
	GO_TITLE_LOGO,
	GO_FLOOR,
	GO_DEBUGAIM,
	GO_CUBE,
	GO_WALL,
	GO_ENEMY,
	GO_BUNBUN,
	GO_MAX
};

enum BUNBUN_ANIM
{
	BUN_BUN_IDLE,
	BUN_BUN_APPEARS,
	BUN_BUN_ABOUTTOFALL,
	BUN_BUN_DISAPPEARS,
	BUN_BUN_MAX_ANIM
};
//*****************************************************************************
// �N���X
//*****************************************************************************
class Object3D
{
private:
	int nPauseFrames;
	//�����œ����Ă���I�u�W�F�N�g�Ȃ�
	bool bMoveable;
	bool bGoToStartPos;
	XMFLOAT3 x3MoveStartPos;
	XMFLOAT3 x3MoveEndPos;
	int nDelayFramesBetweenStops;
	float fAutoSpeed;
	float fTutorialSignSpeed;
	int nSpinningFrames;
protected:
	Model3D* Model;
	XMFLOAT3 Rotation;
	XMFLOAT3 Position;
	XMFLOAT3 InitialPosition;
	XMFLOAT3 Scale;
	Camera3D* pMainCamera;
	XMFLOAT4X4 WorldMatrix;
	int nObjType;
	bool bUse;
	bool bIsUnlit;
	//�e�Ɋւ���
	Object3D* p_goParent;
	int nUseCounterFrame;
	Box Hitbox;
	void* pVisualHitbox;
	int nCurrentRasterizer;
	Light3D* pLight;
	bool ScaleUp;
	float fAcceleration;
public:
	Object3D();
	Object3D(int nType);
	Object3D(const char* ModelPath, int nType);
	Object3D(int ModelPath, int nType);
	Object3D(Light3D* light, const char* ModelPath, int nType);
	Object3D(Light3D* light, int ModelPath, int nType);
	virtual ~Object3D();
	virtual void Init();
	virtual void Update();
	void BunBunControl();
	virtual void Draw();
	virtual void End();
	XMFLOAT3 GetPosition();
	XMFLOAT3 GetRotation();
	XMFLOAT3 GetScale();

	XMFLOAT3* GetPositionAddr();
	XMFLOAT3* GetRotationAddr();
	XMFLOAT3* GetScaleAddr();
	//void SetParent(Object3D* pParent) { Parent = pParent; };

	Model3D* GetModel();
	void SetScale(float fScale);
	void SetScale(XMFLOAT3 fScale);
	void SetRotation(XMFLOAT3 fRot);
	XMFLOAT4X4* GetModelWorld();
	void SetPosition(XMFLOAT3 newPos);
	void SetPosition(XMFLOAT3 newPos, bool bInitial);
	void SetModelRotation(XMFLOAT3 rot);
	void RotateAroundX(float x);
	void RotateAroundY(float y);
	void RotateAroundZ(float z);
	void SetUse(bool newUse);
	void InitModel(const char* szPath);
	void InitModel(int szPath);
	void SetParent(Object3D* pNewParent);
	void TranslateX(float translation);
	void TranslateY(float translation);
	void TranslateZ(float translation);
	bool IsInUse();
	int GetType();
	Box GetHitbox();
	void SetHitbox(Box);
	void SetAnimation(int anim);
	void SetAnimation(int anim, float fSpeed);
	//�����œ����Ă���I�u�W�F�N�g�Ȃ�
	//void AutomaticMovementControl();
	bool MoveToPos(float fSpeed, XMFLOAT3 Destination);
	void SetMovement(XMFLOAT3 Start, XMFLOAT3 End, float Speed, int nDelayFrames);
	bool IsMoveableObject();
	XMFLOAT3 GetMoveStartPosition();
	XMFLOAT3 GetMoveEndPosition();
	XMFLOAT3 GetForward();
	XMFLOAT3 GetModelForward();
	bool MoveTowardPos(XMFLOAT3 Destination, float Speed);
	void PauseObject(int pauseFrames);
	float GetMoveSpeed();
	int GetDelayFrames();
};

//*****************************************************************************
// �N���X-���X�g
//*****************************************************************************
typedef struct go_node {//���X�g�̊Ǘ��ׂ̈ɁA���̍\���̂��v��
	Object3D* Object;
	go_node *next;
};

typedef struct GameObjectContainer {
	XMFLOAT3 Pos;
	XMFLOAT3 Scale;
	char texpath[256];
	bool bMoveable = false;
	XMFLOAT3 MoveStartPos;
	XMFLOAT3 MoveEndPos;
	float Speed; 
	int DelayFrames;
};

class Go_List
{
private:
	int nObjectCount;
public:
	go_node * HeadNode;
	Go_List();
	~Go_List();
	int GetNumberOfObjects();
	Object3D* CheckCollision(Box hb);
	Object3D * CheckCollision(Box hb, bool bIgnoreUnused);
	void DeleteLastPosObject();
	void DeleteObject(Object3D*);
	void Update();
	void Draw();
	void End();
};
