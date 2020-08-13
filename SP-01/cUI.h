#pragma once
#include "Polygon2D.h"
enum UI_TYPE
{
	UI_PAUSE,
	MAX_UI_TYPE
};
class cUI :
	public Polygon2D
{
private:
	int nType;
	float fAcceleration;
	float fPauseSizeOffset;
public:
	cUI(int Type);
	~cUI();
	void Init();
	void Update();
	void Draw();
	void End();
};

