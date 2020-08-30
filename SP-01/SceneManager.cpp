//*****************************************************************************
// SceneManager.cpp
// シーンを管理する
// Author : Mane
//*****************************************************************************
#include "SceneManager.h"
#include "S_InGame3D.h"
#include "S_TitleScreen3D.h"

//*****************************************************************************
//グローバル変数
//*****************************************************************************
Scene3D* pCurrentScene = nullptr;
int nCurrentScene = SCENE_IN_GAME;
int nNextScene = SCENE_IN_GAME;

//*****************************************************************************
//InitScene関数
//初期化関数
//引数：void
//戻：HRESULT
//*****************************************************************************
HRESULT InitScene()
{
	//pCurrentScene = new S_InGame3D();
	pCurrentScene = new S_TitleScreen3D();
	
	return S_OK;
}

//*****************************************************************************
//UpdateScene関数
//変更関数
//引数：void
//戻：void
//*****************************************************************************
void UpdateScene()
{
	if (pCurrentScene)
		nNextScene = pCurrentScene->Update();
}

//*****************************************************************************
//ChangeScene関数
//シーンを変える
//引数：void
//戻：void
//*****************************************************************************
void ChangeScene()
{
	nCurrentScene = nNextScene;
	InitScene();
}

//*****************************************************************************
//DrawScene関数
//レンダリング関数
//引数：void
//戻：void
//*****************************************************************************
void DrawScene()
{
	
	if (pCurrentScene)
		pCurrentScene->Draw();
}

//*****************************************************************************
//EndScene関数
//終了関数
//引数：void
//戻：void
//*****************************************************************************
void EndScene()
{
	SAFE_DELETE(pCurrentScene);
}
