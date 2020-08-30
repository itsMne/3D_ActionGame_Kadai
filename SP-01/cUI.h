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
	MAX_INGAME_UI,
	UI_TITLE_BG,
	UI_TITLESCREEN,
	UI_MENU_OPTION_RIGHT,
	UI_MENU_OPTION_LEFT,
	UI_MENU_OPTION_UP,
	UI_MENU_OPTION_DOWN,
	UI_MENU_SELECTOR,
	UI_MENU_MANAGER,
	UI_MENU_OPTION,
	MAX_MENU_UI
};
class cUI :
	public Polygon2D
{
private:
	int nType;
	float fAcceleration;
	float fPauseSizeOffset;
	cUI* pHealthFlower[UI_HEALTH_FLOWER];
	cUI* pUI_INGAMEs[MAX_INGAME_UI];
	cUI* pUI_MENUs[MAX_MENU_UI-MAX_INGAME_UI];
	cUI* pMenuOption;
	XMFLOAT3 InitialScale;
public:
	cUI(int Type);
	cUI(int Type, cUI* Parent);
	~cUI();
	void Init();
	void Update();
	void MenuManagerControl();
	void AtkZoomControl();
	void FlowerHealthUIControl();
	void Draw();
	void End();
	int GetType() { return nType; };
	cUI* GetSubObject(int objType);
	cUI* GetMenuTitleObject() { return pMenuOption; };
	XMFLOAT3 GetInitialScale() { return InitialScale; };
	void SetInitialScale(XMFLOAT3 scal) { InitialScale = scal; };
};

void ActivateInefectiveHit();
void ActivateDamageEffect();
