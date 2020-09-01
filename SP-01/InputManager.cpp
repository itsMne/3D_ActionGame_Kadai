//*****************************************************************************
// GameInputManager.cpp
// �C���v�b�g�̊Ǘ�
// Author : Mane
//*****************************************************************************
#include "InputManager.h"
#include "stdio.h"
#include "C_XInput.h"
#include "DXWindow3D.h"

#define USE_ANALOGS false
//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
C_XInput* Player1 = nullptr;
bool bInputs[MAX_INPUTS];
float fAxis[MAX_AXIS];
bool bHoldingXinput[MAX_INPUTS];
bool bXinputConnected;

//*****************************************************************************
//�v���g�^�C�v�錾
//*****************************************************************************
void XinputTriggerControl(bool BeforeInputs);

//*****************************************************************************
//InitInputManager�֐�
//�������̊֐�
//�����Fvoid
//�߁Fvoid
//*****************************************************************************
void InitInputManager()
{
	InitInput();
	Player1 = new C_XInput(1);
	for (int i = 0; i < MAX_INPUTS; bInputs[i] = false, i++);
	for (int i = 0; i < MAX_AXIS; fAxis[i] = 0, i++);
}

//*****************************************************************************
//UpdateInputManager�֐�
//�A�b�v�f�[�g�̊֐�
//�����Fvoid
//�߁Fvoid
//*****************************************************************************
void UpdateInputManager()
{
	UpdateInput();
	float fMouseWheel = GetMouseWheelMove();
	//printf("%f\n", fMouseWheel);
	Player1->UpdateXinput();
	XinputTriggerControl(true);
	bXinputConnected = Player1->IsConnected();

	bInputs[INPUT_FORWARD] = GetKeyPress(VK_W);
	bInputs[INPUT_BACKWARD] = GetKeyPress(VK_S);
	
	bInputs[INPUT_LEFT] = GetKeyPress(VK_A);
	bInputs[INPUT_RIGHT] = GetKeyPress(VK_D);

	bInputs[INPUT_MENU_LEFT] = GetKeyTrigger(VK_A) || (Player1->GetState().Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT && !bHoldingXinput[INPUT_MENU_LEFT]);
	bInputs[INPUT_MENU_RIGHT] = GetKeyTrigger(VK_D) || (Player1->GetState().Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT && !bHoldingXinput[INPUT_MENU_RIGHT]);

	bInputs[INPUT_JUMP] = GetKeyTrigger(VK_K) || GetMouseTrigger(MOUSEBUTTON_R) || (Player1->GetState().Gamepad.wButtons & XINPUT_GAMEPAD_A && !bHoldingXinput[INPUT_JUMP]);

	bInputs[INPUT_JUMP_HOLD] = GetKeyPress(VK_K) || GetMouseButton(MOUSEBUTTON_R) || (Player1->GetState().Gamepad.wButtons & XINPUT_GAMEPAD_A);
	
	bInputs[INPUT_PAUSE] = GetKeyTrigger(VK_SPACE) || GetKeyTrigger(VK_P) || (Player1->GetState().Gamepad.wButtons & XINPUT_GAMEPAD_START && !bHoldingXinput[INPUT_PAUSE]);

	bInputs[INPUT_ATTACK] = GetMouseTrigger(MOUSEBUTTON_L) || GetKeyTrigger(VK_I) || (Player1->GetState().Gamepad.wButtons & XINPUT_GAMEPAD_Y && !bHoldingXinput[INPUT_ATTACK]);
	bInputs[INPUT_LOCKON] = GetKeyPress(VK_L) || (Player1->GetState().Gamepad.wButtons & XINPUT_GAMEPAD_B);
	bInputs[INPUT_DODGE] = GetKeyTrigger(VK_J) || ((Player1->GetState().Gamepad.wButtons & XINPUT_GAMEPAD_X) && !bHoldingXinput[INPUT_DODGE]);
	bInputs[INPUT_CAMERA] = GetKeyTrigger(VK_L) || ((Player1->GetState().Gamepad.wButtons & XINPUT_GAMEPAD_B && !bHoldingXinput[INPUT_CAMERA]));

	bInputs[INPUT_DEBUG_AIM_ON] = GetKeyTrigger(VK_1);
	bInputs[INPUT_DEBUG_CONFIRM] = GetKeyTrigger(VK_2);
	bInputs[INPUT_SCALEUP_X] = GetKeyPress(VK_L);
	bInputs[INPUT_SCALEDOWN_X] = GetKeyPress(VK_J);
	bInputs[INPUT_SCALEUP_Y] = GetKeyPress(VK_LSHIFT) && GetKeyPress(VK_I);
	bInputs[INPUT_SCALEDOWN_Y] = GetKeyPress(VK_LSHIFT) && GetKeyPress(VK_K);
	bInputs[INPUT_SCALEUP_Z] = !GetKeyPress(VK_LSHIFT) && GetKeyPress(VK_I);
	bInputs[INPUT_SCALEDOWN_Z] = !GetKeyPress(VK_LSHIFT) && GetKeyPress(VK_K);
	bInputs[INPUT_SWITCH_DEBUGOBJ] = GetKeyTrigger(VK_O);
	bInputs[INPUT_SWITCH_DEBUGOBJALT] = GetKeyTrigger(VK_U);
	bInputs[INPUT_SAVE_LEVEL] = GetKeyPress(VK_LCONTROL) && GetKeyTrigger(VK_S);
	bInputs[INPUT_DEBUGAIM_DELETE] = GetKeyTrigger(VK_BACK);
	bInputs[INPUT_DEBUGAIM_ZOOMIN] = GetKeyPress(VK_NUMPAD8);
	bInputs[INPUT_DEBUGAIM_ZOOMOUT] = GetKeyPress(VK_NUMPAD2);

	bool bUsingKeyBoard = false;
	for (int i = 0; i < MAX_AXIS; fAxis[i] = 0, i++);
		
	if (GetKeyPress(VK_W) || (bXinputConnected && Player1->GetState().Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP))
	{
		bUsingKeyBoard = true;
		fAxis[MOVEMENT_AXIS_VERTICAL] = 1;
	}

	if (GetKeyPress(VK_S) || (bXinputConnected && Player1->GetState().Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN))
	{
		bUsingKeyBoard = true;
		fAxis[MOVEMENT_AXIS_VERTICAL] = -1;
	}

	if (GetKeyPress(VK_D) || (bXinputConnected && (Player1->GetState().Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT)))
	{
		bUsingKeyBoard = true;
		fAxis[MOVEMENT_AXIS_HORIZONTAL] = 1;
	}

	if (GetKeyPress(VK_A) || (bXinputConnected && (Player1->GetState().Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT)))
	{
		bUsingKeyBoard = true;
		fAxis[MOVEMENT_AXIS_HORIZONTAL] = -1;
	}

	if (GetKeyPress(VK_LEFT) || GetKeyPress(VK_Q))
	{
		bUsingKeyBoard = true;
		fAxis[CAMERA_AXIS_HORIZONTAL] = -1;
	}
	if (GetKeyPress(VK_RIGHT) || GetKeyPress(VK_E))
	{
		bUsingKeyBoard = true;
		fAxis[CAMERA_AXIS_HORIZONTAL] = 1;
	}
	if (GetKeyPress(VK_UP) || fMouseWheel < 0)
	{
		bUsingKeyBoard = true;
		fAxis[CAMERA_AXIS_VERTICAL] = 1;
	}
	
	if (GetKeyPress(VK_DOWN) || fMouseWheel>0)
	{
		bUsingKeyBoard = true;
		fAxis[CAMERA_AXIS_VERTICAL] = -1;
	}
	if (!bUsingKeyBoard && bXinputConnected) {
		fAxis[CAMERA_AXIS_HORIZONTAL] = (float)Player1->GetState().Gamepad.sThumbRX / 32876.7f + 0.023543f;
		fAxis[CAMERA_AXIS_VERTICAL] = (float)Player1->GetState().Gamepad.sThumbRY / 32876.7f - 0.017848f + 0.010000;
#if USE_ANALOGS
		fAxis[MOVEMENT_AXIS_HORIZONTAL] = (float)Player1->GetState().Gamepad.sThumbLX / 32767;
		fAxis[MOVEMENT_AXIS_VERTICAL] = (float)Player1->GetState().Gamepad.sThumbLY / 32767;
#endif
	}
	XinputTriggerControl(false);
}

//*****************************************************************************
//EndInputManager�֐�
//�I���̊֐�
//�����Fvoid
//�߁Fvoid
//*****************************************************************************
void EndInputManager()
{
	UninitInput();
}

//*****************************************************************************
//GetAxis�֐�
//�X�e�B�b�N�̏�Ԃ�߂�
//�����Fint
//�߁Fbool
//*****************************************************************************
float GetAxis(int nAxis)
{
	if(fAxis[nAxis] > 0.05f || fAxis[nAxis] < -0.05f)
		return fAxis[nAxis];
	return 0;
}

//*****************************************************************************
//GetInput�֐�
//�C���v�b�g�̏�Ԃ�߂�
//�����Fint
//�߁Fbool
//*****************************************************************************
bool GetInput(int nInput)
{
	return bInputs[nInput];
}

//*****************************************************************************
//VibrateXinput�֐�
//XBOX�̃R���g���[���[�ɗh�点��
//�����Ffloat, float, int
//�߁Fvoid
//*****************************************************************************
void VibrateXinput(float vivbrationL, float VibrationR)
{
	if (!bXinputConnected)
		return;
	Player1->Vibrate(vivbrationL, VibrationR, 10);
}

void VibrateXinput(float vivbrationL, float VibrationR, int nTime)
{
	if (!bXinputConnected)
		return;
	Player1->Vibrate(vivbrationL, VibrationR, nTime);
}

//*****************************************************************************
//XinputTriggerControl�֐�
//�g���K�[�̏ꍇ���Ǘ�����
//�����Fvoid
//�߁Fbool
//*****************************************************************************
void XinputTriggerControl(bool BeforeInputs)
{
	if (BeforeInputs) {
		if (bHoldingXinput[INPUT_ATTACK] && !(Player1->GetState().Gamepad.wButtons & XINPUT_GAMEPAD_Y)) bHoldingXinput[INPUT_ATTACK] = false;
		//if (bHoldingXinput[INPUT_KICK] && !(Player1->GetState().Gamepad.wButtons & XINPUT_GAMEPAD_B)) bHoldingXinput[INPUT_KICK] = false;
		if (bHoldingXinput[INPUT_JUMP] && !(Player1->GetState().Gamepad.wButtons & XINPUT_GAMEPAD_A)) bHoldingXinput[INPUT_JUMP] = false;
		if (bHoldingXinput[INPUT_PAUSE] && !(Player1->GetState().Gamepad.wButtons & XINPUT_GAMEPAD_START)) bHoldingXinput[INPUT_PAUSE] = false;
		if (bHoldingXinput[INPUT_MENU_LEFT] && !(Player1->GetState().Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT)) bHoldingXinput[INPUT_MENU_LEFT] = false;
		if (bHoldingXinput[INPUT_MENU_RIGHT] && !(Player1->GetState().Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT)) bHoldingXinput[INPUT_MENU_RIGHT] = false;
		if (bHoldingXinput[INPUT_CAMERA] && !(Player1->GetState().Gamepad.wButtons & XINPUT_GAMEPAD_B)) bHoldingXinput[INPUT_CAMERA] = false;
		if (bHoldingXinput[INPUT_DODGE] && !(Player1->GetState().Gamepad.wButtons & XINPUT_GAMEPAD_X)) bHoldingXinput[INPUT_DODGE] = false;

	}
	else {
		if (Player1->GetState().Gamepad.wButtons & XINPUT_GAMEPAD_Y) bHoldingXinput[INPUT_ATTACK] = true;
		//if (Player1->GetState().Gamepad.wButtons & XINPUT_GAMEPAD_B) bHoldingXinput[INPUT_KICK] = true;
		if (Player1->GetState().Gamepad.wButtons & XINPUT_GAMEPAD_A) bHoldingXinput[INPUT_JUMP] = true;
		if (Player1->GetState().Gamepad.wButtons & XINPUT_GAMEPAD_START) bHoldingXinput[INPUT_PAUSE] = true;
		if (Player1->GetState().Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT) bHoldingXinput[INPUT_MENU_LEFT] = true;
		if (Player1->GetState().Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT) bHoldingXinput[INPUT_MENU_RIGHT] = true;
		if (Player1->GetState().Gamepad.wButtons & XINPUT_GAMEPAD_B) bHoldingXinput[INPUT_CAMERA] = true;
		if (Player1->GetState().Gamepad.wButtons & XINPUT_GAMEPAD_X) bHoldingXinput[INPUT_DODGE] = true;
	}
}
