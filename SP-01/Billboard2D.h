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
	MAX_BB_TEX
};

//*****************************************************************************
// �N���X
//*****************************************************************************
class Billboard: 
	public Mesh3D
{
private:
	MATERIAL						g_material;			// �}�e���A��
	bool							g_bAlphaTest;		// �A���t�@�e�X�gON/OFF
	bool							g_bInTree;			// �؏�����
	XMFLOAT4 Color;			// �F
	XMFLOAT4X4 mtxWorld;	// ���[���h�}�g���b�N�X
	XMFLOAT3 Position;			// �ʒu
	XMFLOAT3 Rotation;			// �ʒu

	UV			uv;	// �t���[��No.
	int			nAnimeCount;	// �J�E���^
	int			nFrameX, nFrameY;
	float fWidth;			// ��
	float fHeight;			// ����
	bool bSetFalseAfterAnimation;
	ID3D11ShaderResourceView* pTexture;
	XMFLOAT2 x2Size;
	int nSlowness;
public:
	Billboard(int, XMFLOAT2 Size);
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
	void ScaleUp(float nS_Up);
	float GetAlpha();
	bool GetUse();
	void SetUse(bool);
	void ResetUV();
	void SetUnusableAfterAnimation(bool inv);
};

