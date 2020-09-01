//*****************************************************************************
// SceneManager.h
//*****************************************************************************
#pragma once
#include "main.h"

enum DIFFICULTY_SETTINGS
{
	DIF_EASY=0,
	DIF_NORMAL=1,
	DIF_HARD,
	DIF_MAX
};
//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitScene();
void UpdateScene();
void ChangeScene();
void DrawScene();
void EndScene();
int GetGameDifficulty();
void ChangeDifficulty();