#pragma once
#include "Polygon2D.h"
enum UI_TYPE
{
	UI_FLOWER_BASE,
	UI_FLOWER_EYES,
	UI_FLOWER_PETAL_A,
	UI_FLOWER_PETAL_B,
	UI_FLOWER_PETAL_C,
	UI_FLOWER_PETAL_D,
	UI_FLOWER_PETAL_E,
	UI_HEALTH_FLOWER,
	UI_PAUSE,
	UI_ZOOM,
	UI_LOCKON,
	UI_GAME_MANAGER,
	UI_INEFFECTIVE_HIT,
	UI_DAMAGE_AURA,
	MAX_INGAME_UI
};
class cUI :
	public Polygon2D
{
private:
	int nType;
	float fAcceleration;
	float fPauseSizeOffset;
	cUI* pHealthFlower[UI_HEALTH_FLOWER];
	cUI* pUIs[MAX_INGAME_UI];
public:
	cUI(int Type);
	cUI(int Type, cUI* Parent);
	~cUI();
	void Init();
	void Update();
	void AtkZoomControl();
	void FlowerHealthUIControl();
	void Draw();
	void End();
};

void ActivateInefectiveHit();
void ActivateDamageEffect();
