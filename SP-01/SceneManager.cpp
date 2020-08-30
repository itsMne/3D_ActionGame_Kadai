//*****************************************************************************
// SceneManager.cpp
// �V�[�����Ǘ�����
// Author : Mane
//*****************************************************************************
#include "SceneManager.h"
#include "S_InGame3D.h"
#include "S_TitleScreen3D.h"

//*****************************************************************************
//�O���[�o���ϐ�
//*****************************************************************************
Scene3D* pCurrentScene = nullptr;
int nCurrentScene = SCENE_TITLE_SCREEN;
int nNextScene = SCENE_TITLE_SCREEN;

//*****************************************************************************
//InitScene�֐�
//�������֐�
//�����Fvoid
//�߁FHRESULT
//*****************************************************************************
HRESULT InitScene()
{
	nCurrentScene = nNextScene;
	switch (nCurrentScene)
	{
	case SCENE_IN_GAME:
		pCurrentScene = new S_InGame3D();
		break;
	case SCENE_TITLE_SCREEN:
		pCurrentScene = new S_TitleScreen3D();
		break;
	default:
		break;
	}
	//
	
	
	return S_OK;
}

//*****************************************************************************
//UpdateScene�֐�
//�ύX�֐�
//�����Fvoid
//�߁Fvoid
//*****************************************************************************
void UpdateScene()
{
	if (pCurrentScene)
		nNextScene = pCurrentScene->Update();
	if (nNextScene != nCurrentScene)
	{
		EndScene();
		InitScene();
	}
}

//*****************************************************************************
//ChangeScene�֐�
//�V�[����ς���
//�����Fvoid
//�߁Fvoid
//*****************************************************************************
void ChangeScene()
{
	nCurrentScene = nNextScene;
	InitScene();
}

//*****************************************************************************
//DrawScene�֐�
//�����_�����O�֐�
//�����Fvoid
//�߁Fvoid
//*****************************************************************************
void DrawScene()
{
	
	if (pCurrentScene)
		pCurrentScene->Draw();
}

//*****************************************************************************
//EndScene�֐�
//�I���֐�
//�����Fvoid
//�߁Fvoid
//*****************************************************************************
void EndScene()
{
	SAFE_DELETE(pCurrentScene);
}
