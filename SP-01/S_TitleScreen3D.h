//*****************************************************************************
// S_TitleScreen.h
//*****************************************************************************
#pragma once
#include "S_Scene3D.h"
#include "cUI.h"

//*****************************************************************************
// S_TitleScreen.h
//*****************************************************************************
enum MENU_STATES
{
	MENU_TITLE_SCREEN = 0,
	MENU_DISPLAY,
	MENU_LOAD_NEXT_SCENE_STATE,
	MENU_MAX_STATES
};

enum MENU_OPTIONS
{
	MENU_OPTION_GAMESTART=0,
	MENU_OPTION_GAMEEND,
	MENU_MAXOPTIONS
};
class S_TitleScreen3D :
	public Scene3D
{
private:
	cUI* UI_Title;
	int nMenuState;
	int nCurrentMenuSelection;
public:
	S_TitleScreen3D();
	~S_TitleScreen3D();
	void Init();
	eSceneType Update();
	void Draw();
	void End();
	int GetState() { return nMenuState; };
	int GetMenuSelection() { return nCurrentMenuSelection; };
};

int GetTitleSceneState();
int GetMenuState();
