//*****************************************************************************
// S_InGame3D.cpp
// ゲームのシーンを管理する
// Author : Mane
//*****************************************************************************
#include "S_InGame3D.h"
#include "debugproc.h"
#include "InputManager.h"
#include "Billboard2D.h"
#include "Enemy.h"
#include "Field.h"
#include "Polygon2D.h"
#include "Sound.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define MAX_TIME 150

//*****************************************************************************
// グローバル変数
//*****************************************************************************
S_InGame3D* pCurrentGame = nullptr;
int nScore;
int nScoreToAdd;
bool bPauseGame;
Field3D* pFieldTest = nullptr;
Enemy* pEnemyTest = nullptr;
Polygon2D* PauseScreen;
//*****************************************************************************
// コンストラクタ関数
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
	PauseScreen = new Polygon2D("data/texture/PauseScreen.tga");
	PauseScreen->SetUVSize(5.0f, 4.0f);
	PauseScreen->SetSpeedAnimationFrameChange(2);
	PauseScreen->SetPolygonSize(1280, 720);
	PauseScreen->SetAlpha(0.5f);
}


S_InGame3D::~S_InGame3D()
{
	printf("ENDING INGAME SCENE\n");
	UninitPreloadedModels();
	End();
	pCurrentGame = nullptr;
}

//*****************************************************************************
//Init関数
//初期化関数
//引数：void
//戻：void
//*****************************************************************************
void S_InGame3D::Init()
{
	//なし
}

//*****************************************************************************
//Update関数
//変更関数
//引数：void
//戻：eSceneType
//*****************************************************************************
eSceneType S_InGame3D::Update()
{
	pSceneCamera->Update();
	pPlayer->Update();
	pFieldTest->Update();
	pSkybox->Update();
	pSceneLight->SetDirection({ 0.5f,0.5f,0.5 });
	pEnemyTest->Update();
	PauseScreen->UpdatePolygon();
	return SCENE_IN_GAME;
}

//*****************************************************************************
//Draw関数
//レンダリング関数
//引数：void
//戻：void
//*****************************************************************************
void S_InGame3D::Draw()
{
	// Zバッファ有効
	SetZBuffer(true);

	SetCullMode(CULLMODE_NONE);
	// モデル描画
	pFieldTest->Draw();
	pPlayer->Draw();
	pEnemyTest->Draw();
	SetCullMode(CULLMODE_CCW);
	
	// 背面カリング (通常は表面のみ描画)

	pSkybox->Draw();

	// Zバッファ無効
	SetZBuffer(false);

	PauseScreen->DrawPolygon(GetDeviceContext());
	// デバッグ文字列表示
	DrawDebugProc();
}

//*****************************************************************************
//End関数
//終了関数
//引数：void
//戻：void
//*****************************************************************************
void S_InGame3D::End()
{
	Scene3D::End();
	// フィールド終了処理
	// モデル表示終了処理
	SAFE_DELETE(pPlayer);
	SAFE_DELETE(pFieldTest);
	SAFE_DELETE(pEnemyTest);
}

//*****************************************************************************
//GetList関数
//オブジェクトのリストのアドレスを戻す
//引数：int
//戻：Go_List*
//*****************************************************************************
Go_List * S_InGame3D::GetList(int Type)
{
	return nullptr;
}

//*****************************************************************************
//GetCurrentGame関数
//現在のシーンゲームのアドレスを戻す
//引数：void
//戻：S_InGame3D*
//*****************************************************************************
S_InGame3D * GetCurrentGame()
{
	return pCurrentGame;
}

//*****************************************************************************
//GetScore関数
//スコアを戻す
//引数：void
//戻：int
//*****************************************************************************
int GetScore()
{
	return nScore;
}

//*****************************************************************************
//AddScore関数
//スコアを増やす
//引数：int
//戻：void
//*****************************************************************************
void AddScore(int add)
{
	nScoreToAdd += add;
}

//*****************************************************************************
//IsGamePaused関数
//ポースされている状態を確認する
//引数：void
//戻：bool
//*****************************************************************************
bool IsGamePaused()
{
	return bPauseGame;
}
