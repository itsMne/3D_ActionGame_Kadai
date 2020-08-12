//*****************************************************************************
// S_InGame3D.cpp
// �Q�[���̃V�[�����Ǘ�����
// Author : Mane
//*****************************************************************************
#include "S_InGame3D.h"
#include "debugproc.h"
#include "InputManager.h"
#include "Billboard2D.h"
#include "Enemy.h"
#include "Field.h"
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
bool bPauseGame;
Field3D* pFieldTest = nullptr;
Enemy* pEnemyTest = nullptr;
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
	pFieldTest = new Field3D("data/texture/field001.tga");
	pFieldTest->SetPosition({ 2.578626f, 100, -138.668900f });
	pFieldTest->SetScale({ 1000,10,1000 });
	//HRESULT	hr;
	g_pDevice = GetDevice();
	pSceneLight = GetMainLight();
	InitDebugProc();
	pSceneCamera->SetFocalPointGO(pPlayer->GetCameraPlayer());
	nScoreToAdd = nScore = 0;
	//PlaySoundGame(SOUND_LABEL_TUTORIAL);
	pEnemyTest = new Enemy();
	pEnemyTest->SetPosition({ 0, 100, 0 });
	bPauseGame = false;
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
	pSceneCamera->Update();
	pPlayer->Update();
	pFieldTest->Update();
	pSkybox->Update();
	pSceneLight->SetDirection({ 0.5f,0.5f,0.5 });
	pEnemyTest->Update();
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
	pFieldTest->Draw();
	pPlayer->Draw();
	pEnemyTest->Draw();
	SetCullMode(CULLMODE_CCW);
	
	// �w�ʃJ�����O (�ʏ�͕\�ʂ̂ݕ`��)

	pSkybox->Draw();

	// Z�o�b�t�@����
	SetZBuffer(false);

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
	SAFE_DELETE(pFieldTest);
	SAFE_DELETE(pEnemyTest);
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
	return bPauseGame;
}
