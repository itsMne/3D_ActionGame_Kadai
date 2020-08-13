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
#include "cUI.h"
#include "Sound.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define MAX_TIME 150

//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
S_InGame3D* pCurrentGame = nullptr;
int nScore;
int nScoreToAdd;
bool bGamePaused;
cUI* PauseScreen;
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
	pSkybox = new Sphere3D("data/texture/Skybox.tga");
	//HRESULT	hr;
	g_pDevice = GetDevice();
	pSceneLight = GetMainLight();
	InitDebugProc();
	pSceneCamera->SetFocalPointGO(pPlayer->GetCameraPlayer());
	nScoreToAdd = nScore = 0;
	//PlaySoundGame(SOUND_LABEL_TUTORIAL);
	Enemies = new Go_List();
	Fields = new Go_List();
	Enemies->AddEnemy({ 0, 100, 0 });
	Fields->AddField({ 2.578626f, 100, -138.668900f }, { 1000,10,1000 }, TEX_FIELD_A);
	bGamePaused = false;
	PauseScreen = new cUI(UI_PAUSE);

}


S_InGame3D::~S_InGame3D()
{
	printf("ENDING INGAME SCENE\n");
	UninitPreloadedModels();
	End();
	pCurrentGame = nullptr;
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
	if (GetInput(INPUT_PAUSE))
		bGamePaused ^= true;
	PauseScreen->Update();
	if (bGamePaused) 
	{
		return SCENE_IN_GAME;
	}
	pSceneCamera->Update();
	pPlayer->Update();
	Fields->Update();
	pSkybox->Update();
	pSceneLight->SetDirection({ 0.5f,0.5f,0.5 });
	Enemies->Update();

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

	SetCullMode(CULLMODE_NONE);
	// ���f���`��
	Fields->Draw();
	pPlayer->Draw();
	Enemies->Draw();
	SetCullMode(CULLMODE_CCW);
	
	// �w�ʃJ�����O (�ʏ�͕\�ʂ̂ݕ`��)

	pSkybox->Draw();

	// Z�o�b�t�@����
	SetZBuffer(false);

	PauseScreen->Draw();
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
	nScoreToAdd += add;
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
