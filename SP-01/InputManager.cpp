//*****************************************************************************
// GameInputManager.cpp
// インプットの管理
// Author : Mane
//*****************************************************************************
#include "InputManager.h"
#include "stdio.h"
#include "C_XInput.h"
#include "DXWindow3D.h"

//*****************************************************************************
// グローバル変数
//*****************************************************************************
C_XInput* Player1 = nullptr;
bool bInputs[MAX_INPUTS];
float fAxis[MAX_AXIS];
bool bHoldingXinput[MAX_INPUTS];
bool bXinputConnected;

//*****************************************************************************
//プロトタイプ宣言
//*****************************************************************************
void XinputTriggerControl(bool BeforeInputs);

//*****************************************************************************
//InitInputManager関数
//初期化の関数
//引数：void
//戻：void
//*****************************************************************************
void InitInputManager()
{
	InitInput();
	Player1 = new C_XInput(1);
	for (int i = 0; i < MAX_INPUTS; bInputs[i] = false, i++);
	for (int i = 0; i < MAX_AXIS; fAxis[i] = 0, i++);
}

//*****************************************************************************
//UpdateInputManager関数
//アップデートの関数
//引数：void
//戻：void
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

	bInputs[INPUT_MENU_LEFT] = GetKeyTrigger(VK_A) || Player1->GetState().Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT;
	bInputs[INPUT_MENU_RIGHT] = GetKeyTrigger(VK_D) || Player1->GetState().Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT;

	bInputs[INPUT_JUMP] = GetKeyTrigger(VK_K) || (Player1->GetState().Gamepad.wButtons & XINPUT_GAMEPAD_A && !bHoldingXinput[INPUT_JUMP]);

	bInputs[INPUT_JUMP_HOLD] = GetKeyPress(VK_K) || (Player1->GetState().Gamepad.wButtons & XINPUT_GAMEPAD_A);
	
	bInputs[INPUT_PAUSE] = GetKeyTrigger(VK_RETURN) || GetKeyTrigger(VK_P) || (Player1->GetState().Gamepad.wButtons & XINPUT_GAMEPAD_START && !bHoldingXinput[INPUT_PAUSE]);

	bInputs[INPUT_PUNCH] = GetMouseTrigger(MOUSEBUTTON_L) || GetKeyTrigger(VK_I) || (Player1->GetState().Gamepad.wButtons & XINPUT_GAMEPAD_Y && !bHoldingXinput[INPUT_PUNCH]);
	bInputs[INPUT_LOCKON] = GetKeyPress(VK_SPACE) || (Player1->GetState().Gamepad.wButtons & XINPUT_GAMEPAD_B || Player1->GetState().Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER);
	bInputs[INPUT_CAMERA] = GetKeyTrigger(VK_SPACE) || (Player1->GetState().Gamepad.wButtons & XINPUT_GAMEPAD_B || Player1->GetState().Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER
		&& !bHoldingXinput[INPUT_CAMERA]);

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
		
	if (GetKeyPress(VK_W))
	{
		bUsingKeyBoard = true;
		fAxis[MOVEMENT_AXIS_VERTICAL] = 1;
	}

	if (GetKeyPress(VK_S))
	{
		bUsingKeyBoard = true;
		fAxis[MOVEMENT_AXIS_VERTICAL] = -1;
	}

	if (GetKeyPress(VK_D))
	{
		bUsingKeyBoard = true;
		fAxis[MOVEMENT_AXIS_HORIZONTAL] = 1;
	}

	if (GetKeyPress(VK_A))
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
		fAxis[MOVEMENT_AXIS_HORIZONTAL] = (float)Player1->GetState().Gamepad.sThumbLX / 32767;
		fAxis[MOVEMENT_AXIS_VERTICAL] = (float)Player1->GetState().Gamepad.sThumbLY / 32767;
	}
	XinputTriggerControl(false);
}

//*****************************************************************************
//EndInputManager関数
//終了の関数
//引数：void
//戻：void
//*****************************************************************************
void EndInputManager()
{
	UninitInput();
}

//*****************************************************************************
//GetAxis関数
//スティックの状態を戻す
//引数：int
//戻：bool
//*****************************************************************************
float GetAxis(int nAxis)
{
	if(fAxis[nAxis] > 0.05f || fAxis[nAxis] < -0.05f)
		return fAxis[nAxis];
	return 0;
}

//*****************************************************************************
//GetInput関数
//インプットの状態を戻す
//引数：int
//戻：bool
//*****************************************************************************
bool GetInput(int nInput)
{
	return bInputs[nInput];
}

//*****************************************************************************
//VibrateXinput関数
//XBOXのコントローラーに揺らせる
//引数：float, float, int
//戻：void
//*****************************************************************************
void VibrateXinput(float vivbrationL, float VibrationR)
{
	if (!bXinputConnected)
		return;
	Player1->Vibrate(vivbrationL, VibrationR, 10);
}

//*****************************************************************************
//XinputTriggerControl関数
//トリガーの場合を管理する
//引数：void
//戻：bool
//*****************************************************************************
void XinputTriggerControl(bool BeforeInputs)
{
	if (BeforeInputs) {
		if (bHoldingXinput[INPUT_PUNCH] && !(Player1->GetState().Gamepad.wButtons & XINPUT_GAMEPAD_Y)) bHoldingXinput[INPUT_PUNCH] = false;
		//if (bHoldingXinput[INPUT_KICK] && !(Player1->GetState().Gamepad.wButtons & XINPUT_GAMEPAD_B)) bHoldingXinput[INPUT_KICK] = false;
		if (bHoldingXinput[INPUT_JUMP] && !(Player1->GetState().Gamepad.wButtons & XINPUT_GAMEPAD_A)) bHoldingXinput[INPUT_JUMP] = false;
		if (bHoldingXinput[INPUT_PAUSE] && !(Player1->GetState().Gamepad.wButtons & XINPUT_GAMEPAD_START)) bHoldingXinput[INPUT_PAUSE] = false;
		if (bHoldingXinput[INPUT_MENU_LEFT] && !(Player1->GetState().Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT)) bHoldingXinput[INPUT_MENU_LEFT] = false;
		if (bHoldingXinput[INPUT_MENU_RIGHT] && !(Player1->GetState().Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT)) bHoldingXinput[INPUT_MENU_RIGHT] = false;
		if (bHoldingXinput[INPUT_CAMERA] && !(Player1->GetState().Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER)) bHoldingXinput[INPUT_CAMERA] = false;

	}
	else {
		if (Player1->GetState().Gamepad.wButtons & XINPUT_GAMEPAD_Y) bHoldingXinput[INPUT_PUNCH] = true;
		//if (Player1->GetState().Gamepad.wButtons & XINPUT_GAMEPAD_B) bHoldingXinput[INPUT_KICK] = true;
		if (Player1->GetState().Gamepad.wButtons & XINPUT_GAMEPAD_A) bHoldingXinput[INPUT_JUMP] = true;
		if (Player1->GetState().Gamepad.wButtons & XINPUT_GAMEPAD_START) bHoldingXinput[INPUT_PAUSE] = true;
		if (Player1->GetState().Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT) bHoldingXinput[INPUT_MENU_LEFT] = true;
		if (Player1->GetState().Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT) bHoldingXinput[INPUT_MENU_RIGHT] = true;
		if (Player1->GetState().Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER) bHoldingXinput[INPUT_CAMERA] = true;
	}
}
