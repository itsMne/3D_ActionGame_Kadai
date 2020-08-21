//*****************************************************************************
// S_InGame3D.cpp
// ゲームのシーンを管理する
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
bool bGamePaused;

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
	PauseScreen = new cUI(UI_GAME_MANAGER);
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
//Draw関数
//レンダリング関数
//引数：void
//戻：void
//*****************************************************************************
void S_InGame3D::Draw()
{
	// Zバッファ有効
	SetZBuffer(true);
	SetCullMode(CULLMODE_CCW);

	// 背面カリング (通常は表面のみ描画)
	pSkybox->Draw();
	SetCullMode(CULLMODE_NONE);
	// モデル描画
	Fields->Draw();
	Enemies->Draw();
	pPlayer->Draw();
	SetCullMode(CULLMODE_CCW);
	// Zバッファ無効
	SetZBuffer(false);

	PauseScreen->Draw();
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
	SAFE_DELETE(Fields);
	SAFE_DELETE(Enemies);
}

//*****************************************************************************
//GetList関数
//オブジェクトのリストのアドレスを戻す
//引数：int
//戻：Go_List*
//*****************************************************************************
Go_List * S_InGame3D::GetList(int Type)
{
	switch (Type)
	{
	case GO_ENEMY:
		return Enemies;
		break;
	case GO_FLOOR:
		return Fields;
		break;
	default:
		break;
	}
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
	return bGamePaused;
}
