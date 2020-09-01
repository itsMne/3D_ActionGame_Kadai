//*****************************************************************************
// Sound.h
//*****************************************************************************
#ifndef _SOUND_H_
#define _SOUND_H_
#include "DXWindow3D.h"
#include <xaudio2.h>
// Windows7の場合は上記を削除して以下に置き換え.
//#include <C:\Program Files (x86)\Microsoft DirectX SDK (June 2010)\Include\xaudio2.h>

//*****************************************************************************
// サウンドファイル
//*****************************************************************************
typedef enum
{
	SOUND_LABEL_TITLE = 0,		// BGM0
	SOUND_LABEL_MENU,		// BGM0
	SOUND_LABEL_SE_PUNCH,			// 殴るSE
	SOUND_LABEL_SE_SELECTION_A,			// 殴るSE
	SOUND_LABEL_SE_SELECTION_B,			// 殴るSE
	SOUND_LABEL_SE_SELECTION_C,			// 殴るSE
	SOUND_LABEL_SE_TRANSITION,			// 殴るSE
	SOUND_LABEL_SE_MEDIUMPUNCH,			// 殴るSE
	SOUND_LABEL_SE_BIGPUNCH,			// 殴るSE
	SOUND_LABEL_SE_INEFFECTIVEHIT,			// 殴るSE
	SOUND_LABEL_SE_SETDIZZY,			// 殴るSE
	SOUND_LABEL_SE_ENEMYDEAD,			// 殴るSE
	SOUND_LABEL_SE_DIZZY,			// 殴るSE
	SOUND_LABEL_SE_DANGER,			// 殴るSE
	SOUND_LABEL_SE_ENATTACK,			// 殴るSE
	SOUND_LABEL_MAX,				// 登録数の自動設定
} SOUND_LABEL;

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitSound(HWND hWnd);
void	UninitSound(void);
HRESULT PlaySoundGame(SOUND_LABEL label);
void	StopSound(SOUND_LABEL label);
void	StopSound(void);
void	SetVolume(float fVol);

#endif
