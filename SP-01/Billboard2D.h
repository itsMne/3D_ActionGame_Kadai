//*****************************************************************************
// Billboard2D.h
//*****************************************************************************
#pragma once
#include "main.h"
#include "Mesh3D.h"
#include "UniversalStructures.h"

enum BILLBOARD_TEXTURES
{
	BB_HIT,
	BB_HEART,
	BB_BATTERY,
	BB_BATTERY_ENERGY,
	MAX_BB_TEX
};

//*****************************************************************************
// クラス
//*****************************************************************************
class Billboard: 
	public Mesh3D
{
private:
	MATERIAL						g_material;			// マテリアル
	bool							g_bAlphaTest;		// アルファテストON/OFF
	bool							g_bInTree;			// 木処理中
	XMFLOAT4 Color;			// 色
	XMFLOAT4X4 mtxWorld;	// ワールドマトリックス

	UV			uv;	// フレームNo.
	int			nAnimeCount;	// カウンタ
	int			nFrameX, nFrameY;
	float		fWidth;			// 幅
	float		fHeight;			// 高さ
	bool bSetFalseAfterAnimation;
	ID3D11ShaderResourceView* pTexture;
	XMFLOAT2 x2Size;
	int nSlowness;

public:
	Billboard(int, XMFLOAT2 Size);
	Billboard(int, XMFLOAT2 Size, XMFLOAT4 Colors);
	Billboard(ID3D11ShaderResourceView* texture);
	~Billboard();
	HRESULT Init(const char* szpath);
	void Update();
	void Draw();
	void End();
	void SetBillboard(XMFLOAT3 pos, float fWidth, float fHeight, XMFLOAT4 col);
	HRESULT MakeVertex(ID3D11Device* pDevice);
	void SetVertex(float fWidth, float fHeight);
	void SetColor(XMFLOAT4 col);
	void SetPosition(XMFLOAT3 newPosition);
	void SetWidth(float newWidth);
	void SetHeight(float newHeight);
	void SetUVFrames(int nX, int nY, int nSlowness);
	void SetScale(float nScale);
	void SetScale(float Height, float Width);
	float GetScaleWidth() { return fHeight; };
	float GetScaleHeight() { return fWidth; };
	void ScaleUp(float nS_Up);
	float GetAlpha();
	bool GetUse();
	void SetUse(bool);
	void ResetUV();
	XMFLOAT4 GetColor() { return Color; };
	void SetUnusableAfterAnimation(bool inv);
};

