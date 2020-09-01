//*****************************************************************************
// SceneManager.cpp
// �V�[�����Ǘ�����
// Author : Mane
//*****************************************************************************
#include "SceneManager.h"
#include "S_InGame3D.h"
#include "S_TitleScreen3D.h"
#include "cUI.h"

//*****************************************************************************
//�O���[�o���ϐ�
//*****************************************************************************
Scene3D* pCurrentScene = nullptr;
int nCurrentScene = SCENE_TITLE_SCREEN;
int nNextScene = SCENE_TITLE_SCREEN;//CHANGE
int nDifficulty = DIF_NORMAL;
cUI* pTransition = nullptr;
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
	if (!pTransition)
		pTransition = new cUI(UI_TRANSITION);
	
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
	if (pTransition && nNextScene == nCurrentScene && (pTransition->GetUVFrames().x != 0 || pTransition->GetUVFrames().y != 0))
	{
		pTransition->SetAnimationInversed(true);
		pTransition->Update();
		if(pTransition->GetUVFrames().x == 0 && pTransition->GetUVFrames().y == 0)
			pCurrentScene->Update();
		return;
	}
	if (pCurrentScene && nNextScene == nCurrentScene)
		nNextScene = pCurrentScene->Update();

	if (nNextScene != nCurrentScene)
	{
		if (!pTransition)
			return;
		if (pTransition->GetUVFrames().x == 3 && pTransition->GetUVFrames().y == 4)
		{
			EndScene();
			InitScene();
		}
		pTransition->SetAnimationInversed(false);
		pTransition->Update();

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
	SetCullMode(CULLMODE_CCW);
	// Z�o�b�t�@����
	SetZBuffer(false);
	if (pTransition)
		pTransition->Draw();
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

int GetGameDifficulty()
{
	return nDifficulty;
}

void ChangeDifficulty()
{
	nDifficulty++;
	if (nDifficulty == DIF_MAX)
		nDifficulty = 0;
}
