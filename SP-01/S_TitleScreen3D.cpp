#include "S_TitleScreen3D.h"
#include "InputManager.h"

S_TitleScreen3D* pCurrentTitleScene = nullptr;
S_TitleScreen3D::S_TitleScreen3D() :Scene3D(true)
{
	Init();
}

S_TitleScreen3D::~S_TitleScreen3D()
{
	End();
}

void S_TitleScreen3D::Init()
{
	pCurrentTitleScene = this;
	UI_Title = new cUI(UI_MENU_MANAGER);
	nMenuState = MENU_TITLE_SCREEN;
	nCurrentMenuSelection = MENU_OPTION_GAMESTART;
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
				if (GetInput(INPUT_JUMP) || GetInput(INPUT_LOCKON) || GetInput(INPUT_PAUSE))
					nMenuState = MENU_DISPLAY;
			}
		}
		break;
	case MENU_DISPLAY:
		pObj = UI_Title->GetSubObject(UI_MENU_SELECTOR);
		if (GetAxis(MOVEMENT_AXIS_VERTICAL) || GetAxis(MOVEMENT_AXIS_HORIZONTAL)) {
			fRot = (atan2(-GetAxis(MOVEMENT_AXIS_HORIZONTAL), GetAxis(MOVEMENT_AXIS_VERTICAL)))*(180 / XM_PI);
			pObj->SetRotation(0, 0, fRot);
			printf("%f\n", fRot);
			fRot = abs(fRot);
			if (fRot >= 0 && fRot <= 45)
				nNewMenuSelection = MENU_OPTION_GAMESTART;
			if (fRot >= 135 && fRot <= 180)
				nNewMenuSelection = MENU_OPTION_GAMEEND;
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
