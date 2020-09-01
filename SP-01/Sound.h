//*****************************************************************************
// Sound.h
//*****************************************************************************
#ifndef _SOUND_H_
#define _SOUND_H_
#include "DXWindow3D.h"
#include <xaudio2.h>
// Windows7ÌêÍãLðíµÄÈºÉu«·¦.
//#include <C:\Program Files (x86)\Microsoft DirectX SDK (June 2010)\Include\xaudio2.h>

//*****************************************************************************
// TEht@C
//*****************************************************************************
typedef enum
{
	SOUND_LABEL_TITLE = 0,		// BGM0
	SOUND_LABEL_MENU,		// BGM0
	SOUND_LABEL_SE_PUNCH,			// £éSE
	SOUND_LABEL_SE_SELECTION_A,			// £éSE
	SOUND_LABEL_SE_SELECTION_B,			// £éSE
	SOUND_LABEL_SE_SELECTION_C,			// £éSE
	SOUND_LABEL_SE_TRANSITION,			// £éSE
	SOUND_LABEL_SE_MEDIUMPUNCH,			// £éSE
	SOUND_LABEL_SE_BIGPUNCH,			// £éSE
	SOUND_LABEL_SE_INEFFECTIVEHIT,			// £éSE
	SOUND_LABEL_SE_SETDIZZY,			// £éSE
	SOUND_LABEL_SE_ENEMYDEAD,			// £éSE
	SOUND_LABEL_SE_DIZZY,			// £éSE
	SOUND_LABEL_SE_DANGER,			// £éSE
	SOUND_LABEL_SE_ENATTACK,			// £éSE
	SOUND_LABEL_SE_DAMAGE,			// £éSE
	SOUND_LABEL_SE_SLIDE,			// £éSE
	SOUND_LABEL_SE_DEFAULTPUNCH,			// £éSE
	SOUND_LABEL_SE_JUMP,			// £éSE
	SOUND_LABEL_MAX,				// o^Ì©®Ýè
} SOUND_LABEL;

//*****************************************************************************
// vg^Cvé¾
//*****************************************************************************
HRESULT InitSound(HWND hWnd);
void	UninitSound(void);
HRESULT PlaySoundGame(SOUND_LABEL label);
void	StopSound(SOUND_LABEL label);
void	StopSound(void);
void	SetVolume(float fVol);

#endif
