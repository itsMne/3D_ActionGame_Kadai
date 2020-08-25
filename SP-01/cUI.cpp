#include "cUI.h"
#include "Texture.h"
#include "S_InGame3D.h"
#include "Player3D.h"
#include "stdio.h"

#define MAX_HIT_EFF 5

enum UI_TEXTURES
{
	UI_PAUSE_TEX,
	UI_FLOWER_TEX,
	UI_LOCKON_TEX,
	UI_ZOOM_TEX,
	UI_NOHIT_TEX,
	UI_AURA_PLAYERDAMAGE_TEX,//AURA_DAMAGED.tga
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
	for (int i = 0; i < UI_GAME_MANAGER; pUIs[i] = nullptr, i++);
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
		for (int i = UI_HEALTH_FLOWER; i < UI_GAME_MANAGER; pUIs[i] = new cUI(i), i++);
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
		for (int i = UI_HEALTH_FLOWER; i < UI_GAME_MANAGER; pUIs[i]->Update(), i++);
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
	default:
		Polygon2D::UpdatePolygon();
		break;
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
		pUIs[UI_ZOOM]->Draw();
		for (int i = 0; i < MAX_HIT_EFF; i++)
		{
			if (IneffectiveHitEffect[i])
				IneffectiveHitEffect[i]->Draw();
			if (DamageEffect[i])
				DamageEffect[i]->Draw();
		}
		if (GetPlayer() && GetPlayer()->GetLockedEnemy()) {
			pUIs[UI_LOCKON]->SetTexture(nullptr);
			pUIs[UI_LOCKON]->SetAlpha(0.5f);
			pUIs[UI_LOCKON]->SetPosition(0, SCREEN_HEIGHT / 2 - pUIs[UI_LOCKON]->GetSize().y / 2);
			pUIs[UI_LOCKON]->Draw();
			pUIs[UI_LOCKON]->SetPosition(0, -(SCREEN_HEIGHT / 2 - pUIs[UI_LOCKON]->GetSize().y / 2));
			pUIs[UI_LOCKON]->Draw();
			pUIs[UI_LOCKON]->SetTexture(pTextures[UI_LOCKON_TEX]);
			pUIs[UI_LOCKON]->SetAlpha(1.0f);
			pUIs[UI_LOCKON]->SetPosition(0, SCREEN_HEIGHT / 2 - pUIs[UI_LOCKON]->GetSize().y / 2);
			pUIs[UI_LOCKON]->Draw();
			pUIs[UI_LOCKON]->SetPosition(0, -(SCREEN_HEIGHT / 2 - pUIs[UI_LOCKON]->GetSize().y / 2));
			pUIs[UI_LOCKON]->Draw();
		}
		for (int i = UI_HEALTH_FLOWER; i < UI_GAME_MANAGER; i++){
			if (i == UI_LOCKON || i == UI_ZOOM)
				continue;
			pUIs[i]->Draw();
	}
		break;
	default:
		Polygon2D::DrawPolygon(GetDeviceContext());
		break;
	}
	
}

void cUI::End()
{
	UninitPolygon();
	if (nType == UI_GAME_MANAGER)
		for (int i = 0; i < MAX_HIT_EFF; i++) {
			SAFE_DELETE(IneffectiveHitEffect[i]);
			SAFE_DELETE(DamageEffect[i]);
		}
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
