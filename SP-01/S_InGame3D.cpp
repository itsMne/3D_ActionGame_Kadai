//*****************************************************************************
// S_InGame3D.cpp
// �Q�[���̃V�[�����Ǘ�����
// Author : Mane
//*****************************************************************************
#include "S_InGame3D.h"
#include "InputManager.h"
#include "debugproc.h"
#include "InputManager.h"
#include "Billboard2D.h"
#include "Enemy.h"
#include "Field.h"
#include "Sound.h"
#include "RankManager.h"
//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define MAX_TIME 150
#define DEFAULT_BOTTOM -450.0f
//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
S_InGame3D* pCurrentGame = nullptr;
int nScore;
int nScoreToAdd;
bool bGamePaused;
int nTempPauseFrames;
int nWaitFramesForNextPause;
float fBottom;
//*****************************************************************************
// �R���X�g���N�^�֐�
//*****************************************************************************
S_InGame3D::S_InGame3D() :Scene3D(true)
{
	pCurrentGame = this;
	MainWindow = GetMainWindow();
	MainWindow->SetWindowColor255(150, 71, 89);
	pPlayer = new Player3D();
	pPlayer->SetPosition({ 2.578626f, 119.499969f, -138.668900f });
	pSkybox = new Sphere3D("data/texture/Skybox.tga", true);
	pSkybox->SetScale({ 2.5f, 2.5f, 2.5f });
	//HRESULT	hr;
	g_pDevice = GetDevice();
	pSceneLight = GetMainLight();
	InitDebugProc();
	pSceneCamera->SetFocalPointGO(pPlayer->GetCameraPlayer());
	nGameOverFrames = nScoreToAdd = nScore = 0;
	//PlaySoundGame(SOUND_LABEL_TUTORIAL);
	Enemies = new Go_List();
	Fields = new Go_List();
	Enemies->AddEnemy({ 0, 100, 0 });
	Enemies->SetEnemiesToReAdd(5);
	Enemies->AddEnemy({ 500, 100, 500 });
	//Enemies->AddEnemy({ -500, 100, -500 });
	//Enemies->AddEnemy({ -500, 100, 500 });
	//Enemies->AddEnemy({ 500, 100, -500 });
	Fields->AddField({ 2.578626f, 100, -138.668900f }, { 1000,10,1000 }, TEX_FIELD_A);
	bGamePaused = false;
	bSceneEnded = false;
	UI_Manager = new cUI(UI_GAME_MANAGER);
	nTempPauseFrames = 0;
	nWaitFramesForNextPause = 0;
	fBottom = DEFAULT_BOTTOM;
	nCurrentPauseState = NOT_PAUSED_STATE;
	nCurrentPauseSelection = SELECTION_CONTINUE;
	nNextScene = SCENE_IN_GAME;
	bGameClear = false;
	nCheckDeadEnemies = nClearFrames = 0;
	RankManager::Init();
}


S_InGame3D::~S_InGame3D()
{
	printf("ENDING INGAME SCENE\n");
	UninitPreloadedModels();
	ReleaseAllBillboardTextures();
	End();
	pCurrentGame = nullptr;
	Scene3D::~Scene3D();
}

//*****************************************************************************
//Init�֐�
//�������֐�
//�����Fvoid
//�߁Fvoid
//*****************************************************************************
void S_InGame3D::Init()
{
	//�Ȃ�
}

//*****************************************************************************
//Update�֐�
//�ύX�֐�
//�����Fvoid
//�߁FeSceneType
//*****************************************************************************
eSceneType S_InGame3D::Update()
{
	if (bSceneEnded)
		return (eSceneType)nNextScene;
	UI_Manager->Update();
	if (nGameOverFrames>=300)
	{
		if (GetInput(INPUT_JUMP) || GetInput(INPUT_CAMERA) || GetInput(INPUT_PAUSE))
			return SCENE_TITLE_SCREEN;
		return SCENE_IN_GAME;
	}
	if (Enemies->AllEnemiesDead() && ++nCheckDeadEnemies>25)
	{
		bGameClear = true;
		if (++nClearFrames > 400)
		{
			if (GetInput(INPUT_JUMP) || GetInput(INPUT_CAMERA) || GetInput(INPUT_PAUSE))
				return SCENE_TITLE_SCREEN;
		}
		return SCENE_IN_GAME;
	}
	else if(!Enemies->AllEnemiesDead())
		nCheckDeadEnemies = 0;
	if (nScoreToAdd > 0)
	{
		nScoreToAdd--;
		nScore++;
	}

	//PAUSE CONTROL
	if (bGamePaused) 
	{
		if (nCurrentPauseState == NOT_PAUSED_STATE)
			nCurrentPauseState = MAIN_PAUSE_SCREEN;

		//MENU SELECTION
		switch (nCurrentPauseState)
		{
		case MAIN_PAUSE_SCREEN:
			if (GetInput(INPUT_MENU_RIGHT))
				nCurrentPauseSelection++;
			if (GetInput(INPUT_MENU_LEFT))
				nCurrentPauseSelection--;
			if (nCurrentPauseSelection < 0)
				nCurrentPauseSelection = MAX_SELECTIONS - 1;
			if (nCurrentPauseSelection >= MAX_SELECTIONS)
				nCurrentPauseSelection = 0;
			break;
		case INSTRUCTIONS_PAUSE:
			nCurrentPauseSelection = MAX_SELECTIONS;
			break;
		default:
			break;
		}

		if (GetInput(INPUT_JUMP) || GetInput(INPUT_CAMERA) || GetInput(INPUT_PAUSE))
		{
			switch (nCurrentPauseSelection)
			{
			case SELECTION_TITLE:
				bSceneEnded = true;
				nNextScene = SCENE_TITLE_SCREEN;
				return SCENE_TITLE_SCREEN;
			case SELECTION_CONTINUE:
				bGamePaused = false;
				return SCENE_IN_GAME;
			case SELECTION_MOVESET:
				nCurrentPauseState = INSTRUCTIONS_PAUSE;
				return SCENE_IN_GAME;
			}
			if (nCurrentPauseState == INSTRUCTIONS_PAUSE)
			{
				nCurrentPauseState = MAIN_PAUSE_SCREEN;
				nCurrentPauseSelection = SELECTION_MOVESET;
			}
		}
		if (GetInput(INPUT_PAUSE)) {
			bGamePaused ^= true;
			nCurrentPauseSelection = MAX_SELECTIONS;
		}
		return SCENE_IN_GAME;
	}
	//PAUSE CONTROL END
	if (GetInput(INPUT_PAUSE))
		bGamePaused ^= true;
	if (--nTempPauseFrames > 0)
	{
		pPlayer->GetCameraPlayer()->Update();
		return SCENE_IN_GAME;
	}
	else {
		if (--nWaitFramesForNextPause < 0)
			nWaitFramesForNextPause = 0;
	}
	pSceneCamera->Update();
	pPlayer->Update();
	Fields->Update();
	pSkybox->Update();
	pSceneLight->SetDirection({ 0.5f,0.5f,0.5 });
	Enemies->Update();
	nCurrentPauseState = NOT_PAUSED_STATE;
	nCurrentPauseSelection = SELECTION_CONTINUE;
	RankManager::Update();
	if (pPlayer->IsPlayerDead())
	{
		nGameOverFrames++;
	}
	return SCENE_IN_GAME;
}

//*****************************************************************************
//Draw�֐�
//�����_�����O�֐�
//�����Fvoid
//�߁Fvoid
//*****************************************************************************
void S_InGame3D::Draw()
{
	// Z�o�b�t�@�L��
	SetZBuffer(true);
	SetCullMode(CULLMODE_CCW);

	// �w�ʃJ�����O (�ʏ�͕\�ʂ̂ݕ`��)
	pSkybox->Draw();
	pPlayer->DrawDizzySkybox();
	SetCullMode(CULLMODE_NONE);
	// ���f���`��
	Fields->Draw();
	Enemies->Draw();
	pPlayer->Draw();
	SetCullMode(CULLMODE_CCW);
	// Z�o�b�t�@����
	SetZBuffer(false);

	UI_Manager->Draw();
	// �f�o�b�O������\��
	DrawDebugProc();
}

//*****************************************************************************
//End�֐�
//�I���֐�
//�����Fvoid
//�߁Fvoid
//*****************************************************************************
void S_InGame3D::End()
{
	Scene3D::End();
	// �t�B�[���h�I������
	// ���f���\���I������
	SAFE_DELETE(pPlayer);
	SAFE_DELETE(Fields);
	SAFE_DELETE(Enemies);
}

//*****************************************************************************
//GetList�֐�
//�I�u�W�F�N�g�̃��X�g�̃A�h���X��߂�
//�����Fint
//�߁FGo_List*
//*****************************************************************************
Go_List * S_InGame3D::GetList(int Type)
{
	switch (Type)
	{
	case GO_ENEMY:
		return Enemies;
	case GO_FLOOR:
		return Fields;
	default:
		break;
	}
	return nullptr;
}

//*****************************************************************************
//GetCurrentGame�֐�
//���݂̃V�[���Q�[���̃A�h���X��߂�
//�����Fvoid
//�߁FS_InGame3D*
//*****************************************************************************
S_InGame3D * GetCurrentGame()
{
	return pCurrentGame;
}

//*****************************************************************************
//GetScore�֐�
//�X�R�A��߂�
//�����Fvoid
//�߁Fint
//*****************************************************************************
int GetScore()
{
	return nScore;
}

//*****************************************************************************
//AddScore�֐�
//�X�R�A�𑝂₷
//�����Fint
//�߁Fvoid
//*****************************************************************************
void AddScore(int add)
{
	nScoreToAdd += add*GetRank();
}

//*****************************************************************************
//IsGamePaused�֐�
//�|�[�X����Ă����Ԃ��m�F����
//�����Fvoid
//�߁Fbool
//*****************************************************************************
bool IsGamePaused()
{
	return bGamePaused;
}

void SetPauseFrames(int pause, int wait)
{
	if (nWaitFramesForNextPause > 0)
		return;
	nTempPauseFrames = pause;
	nWaitFramesForNextPause = wait;
}

float GetCurrentBottom()
{
	return fBottom;
}

int GetCurrentPauseState()
{
	if (!pCurrentGame)
		return NOT_PAUSED_STATE;
	return pCurrentGame->GetPauseState();
}

int GetCurrentPauseSelection()
{
	if (!pCurrentGame)
		return MAX_SELECTIONS;
	return pCurrentGame->GetPauseSelect();
}

bool IsGameClear()
{
	if(!pCurrentGame)
		return false;
	return pCurrentGame->IsGameClear();
}
