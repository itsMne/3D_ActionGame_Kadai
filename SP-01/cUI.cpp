#include "cUI.h"
#include "Texture.h"
#include "S_InGame3D.h"
#include "Player3D.h"
#include "InputManager.h"
#include "S_TitleScreen3D.h"
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
	for (int i = 0; i < UI_GAME_MANAGER; pUI_INGAMEs[i] = nullptr, i++);
	for (int i = 0; i < MAX_UI_TITLE_OBJS; pUI_MENUs[i] = nullptr, i++);
	pMenuOption = nullptr;
	switch (nType)
	{
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
	}
	fAcceleration = 0;
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
		for (int i = UI_HEALTH_FLOWER; i < UI_GAME_MANAGER; pUI_INGAMEs[i]->Update(), i++);
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
	default:
		Polygon2D::UpdatePolygon();
		break;
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
		//obj->SetAlpha(0);
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
	default:
		Polygon2D::DrawPolygon(GetDeviceContext());
		break;
	}
	
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
