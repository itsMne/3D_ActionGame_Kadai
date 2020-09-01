//*****************************************************************************
// Sound.h
//*****************************************************************************
#ifndef _SOUND_H_
#define _SOUND_H_
#include "DXWindow3D.h"
#include <xaudio2.h>
// Windows7�̏ꍇ�͏�L���폜���Ĉȉ��ɒu������.
//#include <C:\Program Files (x86)\Microsoft DirectX SDK (June 2010)\Include\xaudio2.h>

//*****************************************************************************
// �T�E���h�t�@�C��
//*****************************************************************************
typedef enum
{
	SOUND_LABEL_TITLE = 0,		// BGM0
	SOUND_LABEL_MENU,		// BGM0
	SOUND_LABEL_SE_PUNCH,			// ����SE
	SOUND_LABEL_SE_SELECTION_A,			// ����SE
	SOUND_LABEL_SE_SELECTION_B,			// ����SE
	SOUND_LABEL_SE_SELECTION_C,			// ����SE
	SOUND_LABEL_SE_TRANSITION,			// ����SE
	SOUND_LABEL_SE_MEDIUMPUNCH,			// ����SE
	SOUND_LABEL_SE_BIGPUNCH,			// ����SE
	SOUND_LABEL_SE_INEFFECTIVEHIT,			// ����SE
	SOUND_LABEL_SE_SETDIZZY,			// ����SE
	SOUND_LABEL_SE_ENEMYDEAD,			// ����SE
	SOUND_LABEL_SE_DIZZY,			// ����SE
	SOUND_LABEL_SE_DANGER,			// ����SE
	SOUND_LABEL_SE_ENATTACK,			// ����SE
	SOUND_LABEL_MAX,				// �o�^���̎����ݒ�
} SOUND_LABEL;

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT InitSound(HWND hWnd);
void	UninitSound(void);
HRESULT PlaySoundGame(SOUND_LABEL label);
void	StopSound(SOUND_LABEL label);
void	StopSound(void);
void	SetVolume(float fVol);

#endif
