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
	//ポース
	UI_PAUSE_OPTION_BACKTOGAME,
	UI_PAUSE_OPTION_MOVESET,
	UI_PAUSE_OPTION_BACKTOTITLE,
	UI_PAUSE_INSTRUCTIONS,
	UI_PAUSE_RETURNTOMAINPAUSE,
	//ロックオンとズームイン
	UI_ZOOM,
	UI_LOCKON,
	//ゲームオーバー
	UI_STATIC,
	UI_GAMEOVER,
	//スコア
	UI_SCORE_INGAME,
	UI_REMAINING_ENEMIES,
	UI_REMAINING_ENEMIES_MESSAGE,
	UI_STYLE_RANK,
	//クリア
	UI_CLEAR_BG,
	UI_CLEAR_SCREEN,

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

	UI_TRANSITION,
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
	int nNum;
public:
	cUI(int Type);
	cUI(int Type, cUI* Parent);
	~cUI();
	void Init();
	void Update();
	void InGameUIManagerControl();
	void MenuManagerControl();
	void AtkZoomControl();
	void FlowerHealthUIControl();
	void Draw();
	void DrawNumber();
	void End();
	int GetType() { return nType; };
	cUI* GetSubObject(int objType);
	cUI* GetMenuTitleObject() { return pMenuOption; };
	XMFLOAT3 GetInitialScale() { return InitialScale; };
	void SetInitialScale(XMFLOAT3 scal) { InitialScale = scal; };
};

void ActivateInefectiveHit();
void ActivateDamageEffect();
