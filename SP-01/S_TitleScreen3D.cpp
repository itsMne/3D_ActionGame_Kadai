#include "S_TitleScreen3D.h"
#include "InputManager.h"
#include "SceneManager.h"
#include "Sound.h"

S_TitleScreen3D* pCurrentTitleScene = nullptr;
S_TitleScreen3D::S_TitleScreen3D() :Scene3D(true)
{
	Init();
}

S_TitleScreen3D::~S_TitleScreen3D()
{
	End();
	Scene3D::~Scene3D();
}

void S_TitleScreen3D::Init()
{
	pCurrentTitleScene = this;
	UI_Title = new cUI(UI_MENU_MANAGER);
	nMenuState = MENU_TITLE_SCREEN;
	nCurrentMenuSelection = MENU_OPTION_GAMESTART;
	PlaySoundGame(SOUND_LABEL_TITLE);
}

eSceneType S_TitleScreen3D::Update()
{
	UI_Title->Update();
	cUI* pObj = nullptr;
	float fRot = 0;
	int nNewMenuSelection = nCurrentMenuSelection;
	switch (nMenuState)
	{
	case MENU_TITLE_SCREEN:
		pObj = UI_Title->GetSubObject(UI_TITLESCREEN);
		if (pObj) {
			if (pObj->GetUVFrames().y >= 14)
			{
				if (GetInput(INPUT_JUMP) || GetInput(INPUT_LOCKON) || GetInput(INPUT_PAUSE)) {
					nMenuState = MENU_DISPLAY;
					StopSound();
					PlaySoundGame(SOUND_LABEL_SE_SELECTION_A);
					PlaySoundGame(SOUND_LABEL_MENU);
				}
			}
		}
		break;
	case MENU_DISPLAY:
		pObj = UI_Title->GetSubObject(UI_MENU_SELECTOR);
		if (GetAxis(MOVEMENT_AXIS_VERTICAL) || GetAxis(MOVEMENT_AXIS_HORIZONTAL)) {
			fRot = (atan2(-GetAxis(MOVEMENT_AXIS_HORIZONTAL), GetAxis(MOVEMENT_AXIS_VERTICAL)))*(180 / XM_PI);
			pObj->SetRotation(0, 0, fRot);
			if (fRot >= 46 && fRot <= 134)
				nNewMenuSelection = MENU_OPTION_DIFFICULTY;
			printf("%f\n", fRot);
			fRot = abs(fRot);
			if (fRot >= 0 && fRot <= 45)
				nNewMenuSelection = MENU_OPTION_GAMESTART;
			if (fRot >= 135 && fRot <= 180)
				nNewMenuSelection = MENU_OPTION_GAMEEND;

		}
		if (nNewMenuSelection != nCurrentMenuSelection)
		{
			PlaySoundGame(SOUND_LABEL_SE_SELECTION_B);
			VibrateXinput(65535/2, 65535/2, 20);
			nCurrentMenuSelection = nNewMenuSelection;
		}
		if ((GetInput(INPUT_JUMP) || GetInput(INPUT_CAMERA) || GetInput(INPUT_PAUSE)) && !UI_Title->GetSubObject(UI_TITLESCREEN))
		{
			switch (nCurrentMenuSelection)
			{
			case MENU_OPTION_GAMESTART:
				VibrateXinput(65535, 65535, 35);
				StopSound();
				PlaySoundGame(SOUND_LABEL_SE_TRANSITION);
				return SCENE_IN_GAME;
				break;
			case MENU_OPTION_GAMEEND:
				//StopSound();
				EndCurrentGame();
				break;
			case MENU_OPTION_DIFFICULTY:
				PlaySoundGame(SOUND_LABEL_SE_SELECTION_C);
				VibrateXinput(65535/2, 65535/2, 30);
				ChangeDifficulty();
				break;
			default:
				break;
			}
		}

		break;
	default:
		break;
	}
	return SCENE_TITLE_SCREEN;
}

void S_TitleScreen3D::Draw()
{
	SetCullMode(CULLMODE_CCW);
	// Zバッファ無効
	SetZBuffer(false);
	UI_Title->Draw();
}

void S_TitleScreen3D::End()
{
	SAFE_DELETE(UI_Title);
	pCurrentTitleScene = nullptr;
}

int GetTitleSceneState()
{
	if (!pCurrentTitleScene)
		return MENU_MAX_STATES;
	return pCurrentTitleScene->GetState();
}

int GetMenuState()
{
	if (!pCurrentTitleScene)
		return MENU_MAXOPTIONS;
	return pCurrentTitleScene->GetMenuSelection();
}
