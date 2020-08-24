//*****************************************************************************
// Model3D.h
//*****************************************************************************
#ifndef	MODEL3D
#define MODEL3D
#include "DXWindow3D.h"
#include "TechnicalCamera.h"
#include "FbxModel.h"
#include "Light3D.h"

//*****************************************************************************
// エナム
//*****************************************************************************
enum MyEnum
{
	PLAYER_MODEL,
	BUNBUN_MODEL,
	ENEMY_MODEL,
	CHU_MODEL,
	ANGRY_MODEL,
	MAX_PRELOADED_MODELS,
};

//*****************************************************************************
// クラス
//*****************************************************************************
class Model3D
{
private:
	CFbxModel*		g_pModel;		// FBXモデルへのポインタ
	XMFLOAT3		Position;		// 現在の位置
	XMFLOAT3		Rotation;		// 現在の向き
	XMFLOAT3		Scale;		// 現在の向き
	XMFLOAT4X4	g_mtxWorld;		// ワールドマトリックス
	bool bIsMoving = false;
	TechCamera* pMainCamera;
	float fFrame;
	int nFramCount;
	int nAnimationFrameSlowness;
	int nCurrentAnimation;
	int nInitialFrame;
	int nFinalFrame;
	float fAnimationSkipFrame;
	void* GameObjectParent;
	Light3D* pLight;
	int nCountLoop;
	bool bLoop;
	bool bPreLoadedModel;
	CFbxLight* pCfbxLight;
public:
	Model3D(void* Parent, const char*ModelPath);
	Model3D(void* Parent, int ModelPath);
	~Model3D();
	HRESULT InitModel(const char*ModelPath);
	HRESULT InitModel(int ModelPath);
	void UninitModel(void);
	void UpdateModel(void);
	void SwitchAnimation(int nNewAnimNum, int FrameSlowness, float nAnimationSkipFrame);
	void DrawModel(void);
	void DrawModel(XMFLOAT3* ParPos, XMFLOAT3* ParScal, XMFLOAT3* ParRot);
	void AnimationControl();
	XMFLOAT4X4 *GetModelWorld();
	XMFLOAT3 GetRotation();
	XMFLOAT3 GetPosition();
	XMFLOAT3* GetScaleAdd();
	void SetScale(float newScale);
	void SetScale(XMFLOAT3 newScale);
	int GetNumberOfAnimations();
	void SetParent(void* newParent);
	void RotateAroundX(float x);
	void RotateAroundY(float y);
	void TranslateModel(XMFLOAT3 translation);
	void SetRotation(XMFLOAT3 rot);
	void SetPosition(XMFLOAT3 pos);
	void SetRotationX(float rotx);
	void SetRotationY(float roty);
	void SetLight(Light3D* newLight);
	int GetCurrentAnimation();
	int GetCurrentFrameOfAnimation();
	int GetEndFrameOfCurrentAnimation();
	int GetCurrentFrame();
	int GetLoops();
	void SetFrameOfAnimation(float Frame);
	void SetLoop(bool loopable);
	CFbxModel* GetFbxModel() { return g_pModel; };
};

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
void UninitPreloadedModels();
#endif
