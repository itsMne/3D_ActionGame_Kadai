//*****************************************************************************
// S_TitleScreen.h
//*****************************************************************************
#pragma once
#include "S_Scene3D.h"

//*****************************************************************************
// S_TitleScreen.h
//*****************************************************************************
class S_TitleScreen3D :
	public Scene3D
{
private:
public:
	S_TitleScreen3D();
	~S_TitleScreen3D();
	void Init();
	eSceneType Update();
	void Draw();
	void End();
};

