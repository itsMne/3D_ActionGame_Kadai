//*****************************************************************************
// S_InGame.h
//*****************************************************************************
#pragma once
#include "S_Scene3D.h"
#include "Player3D.h"
#include "Light3D.h"
#include "Sphere3D.h"
#include "Object3D.h"
#include "cUI.h"

//*****************************************************************************
// クラス
//*****************************************************************************

enum PAUSE_STATES
{
	MAIN_PAUSE_SCREEN,
	INSTRUCTIONS_PAUSE,
	NOT_PAUSED_STATE
};

enum PAUSE_SELECTION
{
	SELECTION_TITLE,
	SELECTION_CONTINUE,
	SELECTION_MOVESET,
	MAX_SELECTIONS
};
class S_InGame3D :
	public Scene3D
{
private:
	ID3D11Device* g_pDevice;
	Player3D* pPlayer;
	Sphere3D* pSkybox;
	Light3D* pSceneLight;
	Go_List* Enemies;
	Go_List* Fields;
	cUI* UI_Manager;
	int nCurrentPauseState;
	int nCurrentPauseSelection;
	bool bSceneEnded;
	int nNextScene;
public:
	S_InGame3D();
	~S_InGame3D();
	void Init();
	eSceneType Update();
	void Draw();
	void End();
	Go_List* GetList(int Type);
	int GetPauseState() { return nCurrentPauseState; };
	int GetPauseSelect() { return nCurrentPauseSelection; };
};

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
S_InGame3D* GetCurrentGame();
int GetScore();
void AddScore(int add);
bool IsGamePaused();
void SetPauseFrames(int pause, int wait);
float GetCurrentBottom();
int GetCurrentPauseState();
int GetCurrentPauseSelection();