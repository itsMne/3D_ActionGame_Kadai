#include "cUI.h"
#include "Texture.h"
#include "S_InGame3D.h"
#include "stdio.h"

enum UI_TEXTURES
{
	UI_PAUSE_TEX,
	UI_TEX_MAX
};
ID3D11ShaderResourceView * pTextures[UI_TEX_MAX] = { nullptr };


cUI::cUI(int Type):Polygon2D()
{
	nType = Type;
	Init();
}

cUI::~cUI()
{
	End();
}

void cUI::Init()
{
	switch (nType)
	{
	case UI_PAUSE:
		if(!pTextures[UI_PAUSE_TEX])
			CreateTextureFromFile(GetDevice(), "data/texture/PauseScreen.tga", &pTextures[UI_PAUSE_TEX]);
		SetTexture(pTextures[UI_PAUSE_TEX]);
		SetUVSize(5.0f, 4.0f);
		SetSpeedAnimationFrameChange(2);
		SetSize(1280 * 35, 720 * 35);
		SetAlpha(0.5f);
		fPauseSizeOffset = 35;
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
				fPauseSizeOffset -= fAcceleration;
				if (fPauseSizeOffset < 1) 
					fPauseSizeOffset = 1;
			}
			if (fPauseSizeOffset == 1)
				fAcceleration = 0;
		}
		SetSize(1280 * fPauseSizeOffset, 720 * fPauseSizeOffset);
		Polygon2D::UpdatePolygon();
		break;
	default:
		Polygon2D::UpdatePolygon();
		break;
	}
}

void cUI::Draw()
{
	Polygon2D::DrawPolygon(GetDeviceContext());
}

void cUI::End()
{
	UninitPolygon();
}
