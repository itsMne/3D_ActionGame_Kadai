#include "cUI.h"
#include "Texture.h"
#include "S_InGame3D.h"
#include "Player3D.h"
#include "InputManager.h"
#include "S_TitleScreen3D.h"
#include "RankManager.h"
#include "stdio.h"

#define MAX_HIT_EFF 5
#define MAX_UI_TITLE_OBJS MAX_MENU_UI-MAX_INGAME_UI
enum UI_TEXTURES
{
	UI_PAUSE_TEX,
	UI_FLOWER_TEX,
	UI_LOCKON_TEX,
	UI_ZOOM_TEX,
	UI_NOHIT_TEX,
	UI_AURA_PLAYERDAMAGE_TEX,
	UI_TITLE_TEX,
	UI_TITLE_BG_TEX,
	UI_MENU_OPTION_LEFTRIGHT,
	UI_MENU_OPTION_UPDOWN,
	UI_GAMESTART_OPTION,
	UI_EXIT_OPTION,
	UI_SELECTOR_OPTION,
	UI_BACKTOGAME_OPTION,
	UI_MOVESET_OPTION,
	UI_BACKTOTITLE_OPTION,
	UI_INS_OPTION,
	UI_RETURN_OPTION,
	UI_STATIC_TEX,
	UI_GAMEOVER_MESSAGE,
	UI_INGAMESCORE_TEX,
	UI_STYLE_RANK_TEX,
	UI_CLEAR_TEX,
	UI_TRANSITION_TEX,
	UI_TEX_MAX
};
ID3D11ShaderResourceView * pTextures[UI_TEX_MAX] = { nullptr };
cUI* IneffectiveHitEffect[MAX_HIT_EFF] = {nullptr};
cUI* DamageEffect[MAX_HIT_EFF] = {nullptr};
int nIneffectiveHitWaitOffset = 0;
cUI::cUI(int Type):Polygon2D()
{
	nType = Type;
	Init();
}

cUI::cUI(int Type, cUI * Parent):Polygon2D()
{
	nType = Type;
	pParent = Parent;
	Init();
}

cUI::~cUI()
{
	End();
}

void cUI::Init()
{
	for (int i = 0; i < UI_HEALTH_FLOWER; pHealthFlower[i] = nullptr, i++);
	for (int i = 0; i < MAX_INGAME_UI; pUI_INGAMEs[i] = nullptr, i++);
	for (int i = 0; i < MAX_UI_TITLE_OBJS; pUI_MENUs[i] = nullptr, i++);
	pMenuOption = nullptr;
	nNum = 0;
	switch (nType)
	{
	case UI_TRANSITION:
		if (!pTextures[UI_TRANSITION_TEX])
			CreateTextureFromFile(GetDevice(), "data/texture/UI/TransitionEffect.tga", &pTextures[UI_TRANSITION_TEX]);
		SetTexture(pTextures[UI_TRANSITION_TEX]);
		SetUVSize(4.0f, 5.0f);
		SetSpeedAnimationFrameChange(2);
		SetSize(1280, 720);
		SetAlpha(1.0f);
		fPauseSizeOffset = 35;
		break;
	case UI_PAUSE:
		if(!pTextures[UI_PAUSE_TEX])
			CreateTextureFromFile(GetDevice(), "data/texture/PauseScreen.tga", &pTextures[UI_PAUSE_TEX]);
		SetTexture(pTextures[UI_PAUSE_TEX]);
		SetUVSize(5.0f, 4.0f);
		SetSpeedAnimationFrameChange(2);
		SetSize(1280 * 35, 720 * 35);
		SetAlpha(0.75f);
		fPauseSizeOffset = 35;
		break;
	case UI_CLEAR_BG:
		SetTexture(nullptr);
		SetUVSize(1.0f, 1.0f);
		SetColor(0,0,0);
		SetAlpha(0.75f);
		SetSize(SCREEN_WIDTH, SCREEN_HEIGHT);
		SetRotation(0, 90, 0);
		break;
	case UI_CLEAR_SCREEN:
		if (!pTextures[UI_CLEAR_TEX])
			CreateTextureFromFile(GetDevice(), "data/texture/UI/ClearScreen.tga", &pTextures[UI_CLEAR_TEX]);
		SetTexture(pTextures[UI_CLEAR_TEX]);
		SetUVSize(1.0f, 3.0f);
		SetSpeedAnimationFrameChange(7);
		SetSize(1213, 1794/3);
		SetAlpha(1.0f);
		SetRotation(0, 90, 0);
		break;
	case UI_STATIC:
		if (!pTextures[UI_STATIC_TEX])
			CreateTextureFromFile(GetDevice(), "data/texture/NoiseTexture.tga", &pTextures[UI_STATIC_TEX]);
		SetTexture(pTextures[UI_STATIC_TEX]);
		SetUVSize(2.0f, 7.0f);
		SetSpeedAnimationFrameChange(2);
		SetSize(1280, 720);
		SetAlpha(0.0f);
		break;

	case UI_GAMEOVER:
		if (!pTextures[UI_GAMEOVER_MESSAGE])
			CreateTextureFromFile(GetDevice(), "data/texture/GameOverTitle.tga", &pTextures[UI_GAMEOVER_MESSAGE]);
		SetTexture(pTextures[UI_GAMEOVER_MESSAGE]);
		SetUVSize(1.0f, 3.0f);
		SetSpeedAnimationFrameChange(5);
		SetSize(960, 294);
		SetAlpha(0.0f);
		break;

	case UI_HEALTH_FLOWER:
		for (int i = 0; i < UI_HEALTH_FLOWER; pHealthFlower[i] = new cUI(i, this), i++);
		Position.x = (-SCREEN_WIDTH / 2)+ 107;
		Position.y = (SCREEN_HEIGHT / 2)- 107;
		break;
	case UI_FLOWER_BASE: case UI_FLOWER_EYES: case UI_FLOWER_PETAL_A: case UI_FLOWER_PETAL_B: case UI_FLOWER_PETAL_C: case UI_FLOWER_PETAL_D: case UI_FLOWER_PETAL_E:
		if (!pTextures[UI_FLOWER_TEX])
			CreateTextureFromFile(GetDevice(), "data/texture/Flower.tga", &pTextures[UI_FLOWER_TEX]);
		SetTexture(pTextures[UI_FLOWER_TEX]);
		SetUVSize(3.0f, 7.0f);
		SetSpeedAnimationFrameChange(2);
		SetSize(1280/6, 1280/6);
		SetAlpha(1);
		fPauseSizeOffset = 35;
		SetMoveableOnUV(false);
		nAnimeFrameChange = 10;
		x2UVFrame.y = nType;
		break;
	case UI_ZOOM:
		if (!pTextures[UI_ZOOM_TEX])
			CreateTextureFromFile(GetDevice(), "data/texture/ZoomAtk.tga", &pTextures[UI_ZOOM_TEX]);
		SetTexture(pTextures[UI_ZOOM_TEX]);
		SetSize(1280, 1280);
		SetAlpha(0.0f);
		break;
	case UI_LOCKON:
		if (!pTextures[UI_LOCKON_TEX])
			CreateTextureFromFile(GetDevice(), "data/texture/Zoom.tga", &pTextures[UI_LOCKON_TEX]);
		SetTexture(pTextures[UI_LOCKON_TEX]);
		SetSize(1280, 50);
		SetAlpha(0.75f);
		break;
	case UI_GAME_MANAGER:
		for (int i = UI_HEALTH_FLOWER; i < UI_GAME_MANAGER; pUI_INGAMEs[i] = new cUI(i), i++);
		if (--nIneffectiveHitWaitOffset < 0)
			nIneffectiveHitWaitOffset = 0;
		break;
	case UI_INEFFECTIVE_HIT:
		if (!pTextures[UI_NOHIT_TEX])
			CreateTextureFromFile(GetDevice(), "data/texture/HitUneffective.tga", &pTextures[UI_NOHIT_TEX]);
		SetTexture(pTextures[UI_NOHIT_TEX]);
		SetSize(0, 0);
		SetAlpha(1.0f);
		break;
	case UI_DAMAGE_AURA:
		if (!pTextures[UI_AURA_PLAYERDAMAGE_TEX])
			CreateTextureFromFile(GetDevice(), "data/texture/AURA_DAMAGED.tga", &pTextures[UI_AURA_PLAYERDAMAGE_TEX]);
		SetTexture(pTextures[UI_AURA_PLAYERDAMAGE_TEX]);
		SetSize(0, 0);
		SetAlpha(1.0f);
		break;
	case UI_TITLESCREEN:
		if (!pTextures[UI_TITLE_TEX])
			CreateTextureFromFile(GetDevice(), "data/texture/TitleT.tga", &pTextures[UI_TITLE_TEX]);
		SetTexture(pTextures[UI_TITLE_TEX]);
		SetSize(1280, 720);
		SetAlpha(1.0f);
		SetUVSize(2.0f, 19.0f);
		SetSpeedAnimationFrameChange(3);
		break;
	case UI_TITLE_BG:
		if (!pTextures[UI_TITLE_BG_TEX])
			CreateTextureFromFile(GetDevice(), "data/texture/0.tga", &pTextures[UI_TITLE_BG_TEX]);
		SetTexture(pTextures[UI_TITLE_BG_TEX]);
		SetSize(0, 5);
		SetColor(0.55f,0.45f,0.95f);
		break;
	case UI_MENU_OPTION_RIGHT: case UI_MENU_OPTION_LEFT:
		if (!pTextures[UI_MENU_OPTION_LEFTRIGHT])
			CreateTextureFromFile(GetDevice(), "data/texture/LeftRightOption.tga", &pTextures[UI_MENU_OPTION_LEFTRIGHT]);
		SetTexture(pTextures[UI_MENU_OPTION_LEFTRIGHT]);
		SetSize(640, 720);
		SetPosition(640 * 2, 0);
		if (nType == UI_MENU_OPTION_LEFT)
		{
			RotateAroundZ(180);
			SetPosition(-640 * 2, 0);
		}
		break;
	case UI_MENU_OPTION_UP: case UI_MENU_OPTION_DOWN:
		if (!pTextures[UI_MENU_OPTION_UPDOWN])
			CreateTextureFromFile(GetDevice(), "data/texture/UpDownOption.tga", &pTextures[UI_MENU_OPTION_UPDOWN]);
		SetTexture(pTextures[UI_MENU_OPTION_UPDOWN]);
		SetSize(1279, 362);
		SetPosition(0, -362 * 2);
		pMenuOption = new cUI(UI_MENU_OPTION, this);
		if (nType == UI_MENU_OPTION_UP)
		{
			RotateAroundZ(180);
			SetPosition(0, 362 * 2);
			if (!pTextures[UI_GAMESTART_OPTION])
				CreateTextureFromFile(GetDevice(), "data/texture/GameStartMenu.tga", &pTextures[UI_GAMESTART_OPTION]);
			pMenuOption->SetTexture(pTextures[UI_GAMESTART_OPTION]);
			pMenuOption->SetPosition(0, 50);
			pMenuOption->SetSize(530, 173);
		}
		else {
			if (!pTextures[UI_EXIT_OPTION])
				CreateTextureFromFile(GetDevice(), "data/texture/ExitMenu.tga", &pTextures[UI_EXIT_OPTION]);
			pMenuOption->SetTexture(pTextures[UI_EXIT_OPTION]);
			pMenuOption->SetPosition(0, -50);
			pMenuOption->SetSize(252, 181);
		}
		pMenuOption->SetInitialScale({ pMenuOption->GetSize().x, pMenuOption->GetSize().y,0 });
		break;
	case UI_MENU_MANAGER:
		for (int i = UI_TITLE_BG; i < UI_MENU_MANAGER; pUI_MENUs[i - UI_TITLE_BG] = new cUI(i), i++);
		break;
	case UI_MENU_OPTION:
		SetPosition(0, 0);
		break;
	case UI_MENU_SELECTOR:
		SetPosition(0, 0);
		if (!pTextures[UI_SELECTOR_OPTION])
			CreateTextureFromFile(GetDevice(), "data/texture/SelectorMenu.tga", &pTextures[UI_SELECTOR_OPTION]);
		SetTexture(pTextures[UI_SELECTOR_OPTION]);
		SetSize(70, 100);
		SetPosition(0, 0);
		SetAlpha(0);
		break;
	case UI_PAUSE_OPTION_BACKTOGAME:
		if (!pTextures[UI_BACKTOGAME_OPTION])
			CreateTextureFromFile(GetDevice(), "data/texture/PauseObjects/ContinueOption.tga", &pTextures[UI_BACKTOGAME_OPTION]);
		SetTexture(pTextures[UI_BACKTOGAME_OPTION]);
		SetSize(113, 65);
		SetPosition(0, SCREEN_HEIGHT/2-80);
		SetRotation(0, 90, 0);
		break;	
	case UI_PAUSE_OPTION_MOVESET:
		if (!pTextures[UI_MOVESET_OPTION])
			CreateTextureFromFile(GetDevice(), "data/texture/PauseObjects/MovesOption.tga", &pTextures[UI_MOVESET_OPTION]);
		SetTexture(pTextures[UI_MOVESET_OPTION]);
		SetSize(133, 73);
		SetPosition(SCREEN_WIDTH / 2 - 200, SCREEN_HEIGHT / 2 - 80);
		SetRotation(0, 90, 0);
		break;

	case UI_PAUSE_OPTION_BACKTOTITLE:
		if (!pTextures[UI_BACKTOTITLE_OPTION])
			CreateTextureFromFile(GetDevice(), "data/texture/PauseObjects/BackToTitleOption.tga", &pTextures[UI_BACKTOTITLE_OPTION]);
		SetTexture(pTextures[UI_BACKTOTITLE_OPTION]);
		SetSize(206, 68);
		SetPosition(-(SCREEN_WIDTH / 2 - 200), SCREEN_HEIGHT / 2 - 80);
		SetRotation(0, 90, 0);
		break;
	case UI_PAUSE_INSTRUCTIONS:
		if (!pTextures[UI_INS_OPTION])
			CreateTextureFromFile(GetDevice(), "data/texture/PauseObjects/Instructions.tga", &pTextures[UI_INS_OPTION]);
		SetTexture(pTextures[UI_INS_OPTION]);
		SetSize(1257, 573);
		SetPosition(0, -50);
		SetRotation( 0,90,0 );
		break;
	case UI_PAUSE_RETURNTOMAINPAUSE:
		if (!pTextures[UI_RETURN_OPTION])
			CreateTextureFromFile(GetDevice(), "data/texture/PauseObjects/ReturnOption.tga", &pTextures[UI_RETURN_OPTION]);
		SetTexture(pTextures[UI_RETURN_OPTION]);
		SetSize(172, 89);
		SetPosition(-(SCREEN_WIDTH / 2 - 200), SCREEN_HEIGHT / 2 - 80);
		SetRotation(0, 90, 0);
		break;
	case UI_SCORE_INGAME:
		if (!pTextures[UI_INGAMESCORE_TEX])
			CreateTextureFromFile(GetDevice(), "data/texture/UI/NumbersUI.tga", &pTextures[UI_INGAMESCORE_TEX]);
		SetTexture(pTextures[UI_INGAMESCORE_TEX]);
		SetUVSize(10.0f, 3.0f);
		Position.x = SCREEN_WIDTH / 2 - (25 * 8);
		Position.y = SCREEN_HEIGHT / 2 - 25;
		SetSpeedAnimationFrameChange(3);
		SetSize(18, 25);
		SetAlpha(1.0f);
		break;

	case UI_STYLE_RANK:
		if (!pTextures[UI_STYLE_RANK_TEX])
			CreateTextureFromFile(GetDevice(), "data/texture/UI/RankUI.tga", &pTextures[UI_STYLE_RANK_TEX]);
		SetTexture(pTextures[UI_STYLE_RANK_TEX]);
		SetUVSize(4.0f, 3.0f);
		Position.x = SCREEN_WIDTH / 2 - (25 * 8) + 20;
		Position.y = SCREEN_HEIGHT / 2 - 95;
		SetSize(100, 75);
		SetAlpha(1.0f);
		break;
	}
	fAcceleration = 0;
	InitialScale = Scale;
}

void cUI::Update()
{
	switch (nType)
	{
	case UI_PAUSE:
		fAcceleration += 0.0375f;
		if (!IsGamePaused())
		{
			if (fPauseSizeOffset < 35) {
				if (fAcceleration < 1.5f)
					fAcceleration = 1.5f;
				fPauseSizeOffset += fAcceleration;
				if (fPauseSizeOffset > 35)
					fPauseSizeOffset = 35;
			}
			if (fPauseSizeOffset == 35)
				fAcceleration = 0;
		}
		else
		{
			if (fPauseSizeOffset > 1) {
				if (fAcceleration < 1.5f)
					fAcceleration = 1.5f;
				fPauseSizeOffset -= fAcceleration;
				if (fPauseSizeOffset < 1) 
					fPauseSizeOffset = 1;
			}
			if (fPauseSizeOffset == 1)
				fAcceleration = 0;
		}
		SetSize(1285 * fPauseSizeOffset, 725 * fPauseSizeOffset);
		Polygon2D::UpdatePolygon();
		break;
	case UI_HEALTH_FLOWER:
		FlowerHealthUIControl();
		break;
	case UI_LOCKON:
		if (GetPlayer() && GetPlayer()->GetLockedEnemy()) {
			x2UVFrame.x += 0.005f;
			SetPolygonUV(x2UVFrame.x / 1.0f, x2UVFrame.y / 1.0f);
		}
		break;
	case UI_ZOOM:
		AtkZoomControl();
		break;
	case UI_GAME_MANAGER:
		InGameUIManagerControl();
		break;
	case UI_INEFFECTIVE_HIT:case UI_DAMAGE_AURA:
		if (fAcceleration < 10)
			fAcceleration = 10;
		fAcceleration+=5;
		ScaleUp(fAcceleration);
		return;
	case UI_TITLESCREEN:
		Polygon2D::UpdatePolygon();
		if (x2UVFrame.x == 1 && x2UVFrame.y == 18) {
			x2UVFrame.x = 0;
			x2UVFrame.y = 14;
		}
		if (GetInput(INPUT_JUMP) || GetInput(INPUT_LOCKON) || GetInput(INPUT_PAUSE))
		{
			if (x2UVFrame.y < 14)
			{
				x2UVFrame.x = 0;
				x2UVFrame.y = 14;
			}
		}
		break;
	case UI_MENU_MANAGER:
		MenuManagerControl();
		break;
	case UI_TITLE_BG:
		if (Scale.x < 1280)
		{
			fAcceleration += 2.0f;
			Scale.x += fAcceleration;
		}
		else if(Scale.y<720){
			fAcceleration += 0.5f;
			Scale.y += fAcceleration;
		}
		if (GetInput(INPUT_JUMP) || GetInput(INPUT_LOCKON) || GetInput(INPUT_PAUSE))
			SetSize(1280, 720);
		break;
	case UI_STATIC: case UI_GAMEOVER:
		Polygon2D::UpdatePolygon();
		if(IsPlayerDead() && Color.w<1)
			RaiseAlpha(0.005f);
		break;
	case UI_SCORE_INGAME:
		Polygon2D::UpdatePolygon();
		nNum = GetScore();
		if (++fAcceleration > 8) {
			x2UVFrame.y++;
			if (x2UVFrame.y == x2Frame.y)
				x2UVFrame.y = 0;
			fAcceleration = 0;
		}
		break;
	case UI_STYLE_RANK:
		x2UVFrame.x = GetRank()-1;
		if (++fAcceleration > 8) {
			x2UVFrame.y++;
			fAcceleration = 0;
			if (x2UVFrame.y == x2Frame.y)
				x2UVFrame.y = 0;
		}
		if (x2UVFrame.x == 0)
			SetAlpha(0);
		else
			SetAlpha(1);
		SetPolygonFrameSize(1.0 / x2UVMaxFrameSize.x, 1.0 / x2UVMaxFrameSize.y);
		SetPolygonUV(x2UVFrame.x / x2UVMaxFrameSize.x, x2UVFrame.y / x2UVMaxFrameSize.y);
		break;
	default:
		Polygon2D::UpdatePolygon();
		break;
	}
}

void cUI::InGameUIManagerControl()
{
	for (int i = UI_HEALTH_FLOWER; i < UI_GAME_MANAGER; pUI_INGAMEs[i]->Update(), i++);
	cUI* pContinueOption = GetSubObject(UI_PAUSE_OPTION_BACKTOGAME);
	cUI* pMoveSetOption = GetSubObject(UI_PAUSE_OPTION_MOVESET);
	cUI* pReturnToTitleOption = GetSubObject(UI_PAUSE_OPTION_BACKTOTITLE);
	cUI* pMoveset = GetSubObject(UI_PAUSE_INSTRUCTIONS);
	cUI* pPause = GetSubObject(UI_PAUSE);
	cUI* pReturnToMain = GetSubObject(UI_PAUSE_RETURNTOMAINPAUSE);

	if (IsGameClear()) {
		cUI*obj = GetSubObject(UI_CLEAR_BG);
		if (obj->GetRotationY() > 0) {
			obj->RotateAroundY(-10);
			return;
		}
		obj = GetSubObject(UI_CLEAR_SCREEN);
		if (obj->GetRotationY() > 0)
			obj->RotateAroundY(-10);

		return;
	}

	if (!pPause)
		return;

	pContinueOption->SetSize(pContinueOption->GetInitialScale());
	pMoveSetOption->SetSize(pMoveSetOption->GetInitialScale());
	pReturnToTitleOption->SetSize(pReturnToTitleOption->GetInitialScale());
	pReturnToMain->SetSize(pReturnToMain->GetInitialScale());
	static int nDir = 1;
	fAcceleration += nDir;
	if (abs(fAcceleration) > 10)
		nDir *= -1;



	switch (GetCurrentPauseState())
	{
	case NOT_PAUSED_STATE:
		if (pContinueOption->GetRotationY() < 90)
			pContinueOption->RotateAroundY(10);
		if (pMoveSetOption->GetRotationY() < 90)
			pMoveSetOption->RotateAroundY(10);
		if (pReturnToTitleOption->GetRotationY() < 90)
			pReturnToTitleOption->RotateAroundY(10);
		if (pMoveset->GetRotationY() < 90)
			pMoveset->RotateAroundY(10);		
		
		if (pReturnToMain->GetRotationY() < 90)
			pReturnToMain->RotateAroundY(10);
		break;
	case INSTRUCTIONS_PAUSE:
		if (pContinueOption->GetRotationY() < 90)
			pContinueOption->RotateAroundY(10);
		if (pReturnToTitleOption->GetRotationY() < 90)
			pReturnToTitleOption->RotateAroundY(10);

		if (pMoveSetOption->GetRotationY() > 0)
			pMoveSetOption->RotateAroundY(-10);

		if (pMoveset->GetRotationY() > 0)
			pMoveset->RotateAroundY(-10);

		if (pReturnToMain->GetRotationY() > 0)
			pReturnToMain->RotateAroundY(-10);

		pReturnToMain->ScaleUp(fAcceleration);
		break;
	case MAIN_PAUSE_SCREEN:
		if (pPause->GetSize().y <= 725)
		{
			if (pContinueOption->GetRotationY() > 0)
				pContinueOption->RotateAroundY(-10); 
			if (pMoveSetOption->GetRotationY() > 0)
				pMoveSetOption->RotateAroundY(-10);
			if (pReturnToTitleOption->GetRotationY() > 0)
				pReturnToTitleOption->RotateAroundY(-10);
			if (pMoveset->GetRotationY() < 90)
				pMoveset->RotateAroundY(10);
			if (pReturnToMain->GetRotationY() < 90)
				pReturnToMain->RotateAroundY(10);
			switch (GetCurrentPauseSelection())
			{
			case SELECTION_CONTINUE:
				pContinueOption->ScaleUp(fAcceleration);
				break;
			case SELECTION_MOVESET:
				pMoveSetOption->ScaleUp(fAcceleration);
				break;
			case SELECTION_TITLE:
				pReturnToTitleOption->ScaleUp(fAcceleration);
				break;
			default:
				break;
			}
			//
		}
		break;
	default:
		break;
	}
	for (int i = 0; i < MAX_HIT_EFF; i++)
	{
		if (IneffectiveHitEffect[i]) {
			IneffectiveHitEffect[i]->Update();
			if (IneffectiveHitEffect[i]->GetSize().x > 1500)
				SAFE_DELETE(IneffectiveHitEffect[i]);
		}
		if (DamageEffect[i]) {
			DamageEffect[i]->Update();
			if (DamageEffect[i]->GetSize().x > 1500)
				SAFE_DELETE(DamageEffect[i]);
		}
	}
}

void cUI::MenuManagerControl()
{
	for (int i = UI_TITLE_BG; i < UI_MENU_MANAGER; i++) 
	{
		if (pUI_MENUs[i - UI_TITLE_BG])
			pUI_MENUs[i - UI_TITLE_BG]->Update();
	}
	if (GetTitleSceneState() == MENU_DISPLAY)
	{
		fAcceleration++;
		bool bRight, bLeft, bUp, bDown;
		bRight = bLeft = bUp = bDown = false;
		
		cUI* obj = GetSubObject(UI_MENU_OPTION_RIGHT);
		if (obj->GetPosition().x > 640 / 2) {
			obj->Translate({ -fAcceleration,0 });
			if (obj->GetPosition().x < 640 / 2)
				obj->SetPosition(640 / 2, 0);
		}
		if (obj->GetPosition().x == 640 / 2)
			bRight = true;
		
		obj = GetSubObject(UI_MENU_OPTION_LEFT);
		if (obj->GetPosition().x < -640 / 2) {
			obj->Translate({ fAcceleration,0 });
			if (obj->GetPosition().x > -640 / 2)
				obj->SetPosition(-640 / 2, 0);
		}
		if (obj->GetPosition().x == -640 / 2)
			bLeft = true;
		obj = GetSubObject(UI_MENU_OPTION_DOWN);
		if (obj->GetPosition().y < -362 / 2) {

			obj->Translate({ 0,fAcceleration });
			if (obj->GetPosition().y > -362 / 2)
				obj->SetPosition(0, -362 / 2);
		}
		if (obj->GetPosition().y == -362 / 2)
			bDown = true;
		obj = GetSubObject(UI_MENU_OPTION_UP);
		if (obj->GetPosition().y > 362 / 2) {

			obj->Translate({ 0,-fAcceleration });
			if (obj->GetPosition().y < 362 / 2)
				obj->SetPosition(0, 362 / 2);
		}
		if (obj->GetPosition().y == 362 / 2)
			bUp = true;
		if (bUp && bDown && bRight && bLeft)
		{
			SAFE_DELETE(pUI_MENUs[UI_TITLESCREEN - UI_TITLE_BG]);
			SAFE_RELEASE(pTextures[UI_TITLE_TEX]);
			obj = GetSubObject(UI_MENU_SELECTOR);
			if (obj->GetAlpha() < 1)
				obj->RaiseAlpha(0.05f);
			int nCurrentSel = GetMenuState();

			GetSubObject(UI_MENU_OPTION_UP)->GetMenuTitleObject()->SetSize(GetSubObject(UI_MENU_OPTION_UP)->GetMenuTitleObject()->GetInitialScale());
			GetSubObject(UI_MENU_OPTION_DOWN)->GetMenuTitleObject()->SetSize(GetSubObject(UI_MENU_OPTION_DOWN)->GetMenuTitleObject()->GetInitialScale());

			static float fSelectedOffset=0;
			static int nDir = 1;
			fSelectedOffset += nDir;
			if (abs(fSelectedOffset) > 10)
				nDir *= -1;
			switch (nCurrentSel)
			{
			case MENU_OPTION_GAMESTART:
				obj = GetSubObject(UI_MENU_OPTION_UP)->GetMenuTitleObject();
				obj->ScaleUp(fSelectedOffset);
				break;
			case MENU_OPTION_GAMEEND:
				obj = GetSubObject(UI_MENU_OPTION_DOWN)->GetMenuTitleObject();
				obj->ScaleUp(fSelectedOffset);
				break;
			default:
				break;
			}
		}

	}
}

void cUI::AtkZoomControl()
{
	RotateAroundZ(45.0f);
	Camera3D* pCamera = (Camera3D*)(GetMainCamera()->GetFocalPoint());
	if (!pCamera)
		return;
	float zoom = pCamera->GetTempZoomIntensity();
	if (abs(zoom) >=150)
	{
		SetAlpha(0.5f);
	}
	else {
		if (Color.w > 0)
			SetAlpha(Color.w - 0.05f);
		if (Color.w < 0)
			SetAlpha(0);
	}
}

void cUI::FlowerHealthUIControl()
{
	for (int i = 0; i < UI_HEALTH_FLOWER; pHealthFlower[i]->Update(), i++);
	Player3D* pPlayer = GetPlayer();
	if (!pPlayer)
		return;
	int nCurrentHP = pPlayer->GetCurrentHP();
	for (int i = UI_FLOWER_PETAL_A; i < UI_FLOWER_PETAL_A+MAX_HEALTH; i++)
	{
		if (i-1 > nCurrentHP)
		{
			if(pHealthFlower[i]->GetPosition().y<SCREEN_WIDTH+ pHealthFlower[i]->GetSize().y)
				pHealthFlower[i]->Translate({ 0, -5*(i*0.25f) });
			pHealthFlower[i]->RotateAroundY(10);
		}
		else {
			pHealthFlower[i]->SetPosition(0, 0);
			pHealthFlower[i]->SetRotation(0, 0, 0);
		}
	}
	
}

void cUI::Draw()
{
	switch (nType)
	{
	case UI_HEALTH_FLOWER:
		for (int i = UI_HEALTH_FLOWER - 1; i > 1; pHealthFlower[i]->Draw(), i--);
		pHealthFlower[0]->Draw();
		pHealthFlower[1]->Draw();
		break;
	case UI_GAME_MANAGER:
		pUI_INGAMEs[UI_ZOOM]->Draw();
		for (int i = 0; i < MAX_HIT_EFF; i++)
		{
			if (IneffectiveHitEffect[i])
				IneffectiveHitEffect[i]->Draw();
			if (DamageEffect[i])
				DamageEffect[i]->Draw();
		}
		if (GetPlayer() && GetPlayer()->GetLockedEnemy()) {
			pUI_INGAMEs[UI_LOCKON]->SetTexture(nullptr);
			pUI_INGAMEs[UI_LOCKON]->SetAlpha(0.5f);
			pUI_INGAMEs[UI_LOCKON]->SetPosition(0, SCREEN_HEIGHT / 2 - pUI_INGAMEs[UI_LOCKON]->GetSize().y / 2);
			pUI_INGAMEs[UI_LOCKON]->Draw();
			pUI_INGAMEs[UI_LOCKON]->SetPosition(0, -(SCREEN_HEIGHT / 2 - pUI_INGAMEs[UI_LOCKON]->GetSize().y / 2));
			pUI_INGAMEs[UI_LOCKON]->Draw();
			pUI_INGAMEs[UI_LOCKON]->SetTexture(pTextures[UI_LOCKON_TEX]);
			pUI_INGAMEs[UI_LOCKON]->SetAlpha(1.0f);
			pUI_INGAMEs[UI_LOCKON]->SetPosition(0, SCREEN_HEIGHT / 2 - pUI_INGAMEs[UI_LOCKON]->GetSize().y / 2);
			pUI_INGAMEs[UI_LOCKON]->Draw();
			pUI_INGAMEs[UI_LOCKON]->SetPosition(0, -(SCREEN_HEIGHT / 2 - pUI_INGAMEs[UI_LOCKON]->GetSize().y / 2));
			pUI_INGAMEs[UI_LOCKON]->Draw();
		}
		for (int i = UI_HEALTH_FLOWER; i < UI_GAME_MANAGER; i++){
			if (i == UI_LOCKON || i == UI_ZOOM)
				continue;
			pUI_INGAMEs[i]->Draw();
	}
		break;
	case UI_MENU_MANAGER:
		for (int i = UI_TITLE_BG; i < UI_MENU_MANAGER; i++)
		{
			if (pUI_MENUs[i - UI_TITLE_BG])
				pUI_MENUs[i - UI_TITLE_BG]->Draw();
		}
		break;
	case UI_MENU_OPTION_UP: case UI_MENU_OPTION_DOWN: case UI_MENU_OPTION_LEFT: case UI_MENU_OPTION_RIGHT:
		Polygon2D::DrawPolygon(GetDeviceContext());
		if (pMenuOption)
			pMenuOption->Draw();
		break;
	case UI_SCORE_INGAME:
		DrawNumber();
		break;
	default:
		Polygon2D::DrawPolygon(GetDeviceContext());
		break;
	}
	
}

void cUI::DrawNumber()
{
	XMFLOAT3 x2OriginalPosition;
	int nNumberToPrint;
	int nDividerByTen;
	int nDigits = 0;
	int	nMaxDigits = 7;
	//if (nType == UI_TIMER)
	//	nMaxDigits = 2;
	x2OriginalPosition = Position;
	nNumberToPrint = nNum;
	float fSepatation = 27;
	//if (nType == UI_RESULT_SCREEN_NUMBER)
	//	fSepatation = 105.0f;
	for (nDividerByTen = 1; nNumberToPrint > 0; nNumberToPrint /= 10, nDividerByTen *= 10, nDigits++);
	nDividerByTen /= 10;
	if (nNum < 10)
	{
		for (int i = 0; i < nMaxDigits; i++)
		{
			x2UVFrame.x = 0;
			SetPolygonFrameSize(1.0 / x2UVMaxFrameSize.x, 1.0 / x2UVMaxFrameSize.y);
			SetPolygonUV(x2UVFrame.x / x2UVMaxFrameSize.x, x2UVFrame.y / x2UVMaxFrameSize.y);
			DrawPolygon(GetDeviceContext());
			Position.x += fSepatation * 0.75f;
		}
		x2UVFrame.x = nNum;
		SetPolygonFrameSize(1.0 / x2UVMaxFrameSize.x, 1.0 / x2UVMaxFrameSize.y);
		SetPolygonUV(x2UVFrame.x / x2UVMaxFrameSize.x, x2UVFrame.y / x2UVMaxFrameSize.y);
		DrawPolygon(GetDeviceContext());
	}
	else {
		for (int i = 0; i < nMaxDigits - nDigits + 1; i++)
		{
			x2UVFrame.x = 0;
			SetPolygonFrameSize(1.0 / x2UVMaxFrameSize.x, 1.0 / x2UVMaxFrameSize.y);
			SetPolygonUV(x2UVFrame.x / x2UVMaxFrameSize.x, x2UVFrame.y / x2UVMaxFrameSize.y);
			DrawPolygon(GetDeviceContext());
			Position.x += fSepatation * 0.75f;
		}
		while (nDividerByTen > 0)
		{
			nNumberToPrint = nNum / nDividerByTen % 10;
			SetPolygonFrameSize(1.0 / x2UVMaxFrameSize.x, 1.0 / x2UVMaxFrameSize.y);
			SetPolygonUV(nNumberToPrint / x2UVMaxFrameSize.x, x2UVFrame.y / x2UVMaxFrameSize.y);
			DrawPolygon(GetDeviceContext());
			Position.x += fSepatation * 0.75f;
			nDividerByTen /= 10;
		}
	}
	SetPosition(x2OriginalPosition.x, x2OriginalPosition.y);
}

void cUI::End()
{
	switch (nType)
	{
	case UI_GAME_MANAGER:
		for (int i = 0; i < MAX_HIT_EFF; i++) {
			SAFE_DELETE(IneffectiveHitEffect[i]);
			SAFE_DELETE(DamageEffect[i]);
		}
		break;
	case UI_MENU_MANAGER:
		for (int i = UI_TITLE_BG; i < UI_MENU_MANAGER; i++)
		{
			SAFE_DELETE(pUI_MENUs[i - UI_TITLE_BG]);
		}
		break;
	default:
		break;
	}
	for (int i = 0; i < MAX_INGAME_UI; i++)
	{
		SAFE_DELETE(pUI_INGAMEs[i]);
	}
	UninitPolygon();
	SAFE_DELETE(pMenuOption);
}

cUI * cUI::GetSubObject(int objType)
{
	for (int i = 0; i < MAX_HIT_EFF; i++) {
		if (IneffectiveHitEffect[i] && IneffectiveHitEffect[i]->GetType() == objType)
			return IneffectiveHitEffect[i];
		if (DamageEffect[i] && DamageEffect[i]->GetType() == objType)
			return DamageEffect[i];
	}
	for (int i = 0; i < MAX_INGAME_UI; i++)
	{
		if (pUI_INGAMEs[i] && pUI_INGAMEs[i]->GetType() == objType)
			return pUI_INGAMEs[i];
	}
	for (int i = UI_TITLE_BG; i < UI_MENU_MANAGER; i++)
	{
		if (pUI_MENUs[i - UI_TITLE_BG] && pUI_MENUs[i - UI_TITLE_BG]->GetType() == objType)
			return pUI_MENUs[i - UI_TITLE_BG];
	}
	return nullptr;
}

void ActivateInefectiveHit()
{
	static bool flip = false;
	for (int i = 0; i < MAX_HIT_EFF; i++)
	{
		if (IneffectiveHitEffect[i])
			continue;
		IneffectiveHitEffect[i] = new cUI(UI_INEFFECTIVE_HIT);
		if (flip)
			IneffectiveHitEffect[i]->RotateAroundZ(100);
		flip ^= true;
		nIneffectiveHitWaitOffset = 60;
	}
}

void ActivateDamageEffect()
{
	for (int i = 0; i < MAX_HIT_EFF; i++)
	{
		if (DamageEffect[i])
			continue;
		DamageEffect[i] = new cUI(UI_DAMAGE_AURA);
	}
}
