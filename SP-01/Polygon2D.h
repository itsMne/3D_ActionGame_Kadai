//*****************************************************************************
// Polygon2D.h
//*****************************************************************************
#ifndef	 POLYGON_H
#define  POLYGON_H
#include "main.h"
#include "UniversalStructures.h"

//*****************************************************************************
// �G�i��
//*****************************************************************************
enum Direction
{
	RIGHT_DIR = 1,
	LEFT_DIR = -1
};

//*****************************************************************************
// �N���X
//*****************************************************************************
class Polygon2D
{
private:
	bool bPolygonInited;
protected:
	ID3D11ShaderResourceView*	g_pTexture;				// �e�N�X�`���ւ̃|�C���^

	VERTEX_2D					g_vertexWk[NUM_VERTEX];	// ���_���i�[���[�N

	XMFLOAT3						Position;			// �|���S���̈ړ���
	XMFLOAT3						InitPosition;			// �|���S���̈ړ���
	XMFLOAT3						Rotation;			// �|���S���̉�]��
	XMFLOAT3						Scale;			// �|���S���̃T�C�Y
	XMFLOAT4						Color;			// �|���S���̒��_�J���[
	bool							g_bInvalidate;			// ���_�f�[�^�X�V�t���O

	
	XMFLOAT2						x2Frame;			// �e�N�X�`�����o�T�C�Y

	ID3D11Buffer*				g_pConstantBuffer;		// �萔�o�b�t�@
	ID3D11Buffer*				g_pVertexBuffer;		// ���_�o�b�t�@
	ID3D11SamplerState*			g_pSamplerState;		// �e�N�X�`�� �T���v��
	ID3D11VertexShader*			g_pVertexShader;		// ���_�V�F�[�_
	ID3D11InputLayout*			g_pInputLayout;			// ���_�t�H�[�}�b�g
	ID3D11PixelShader*			g_pPixelShader;			// �s�N�Z���V�F�[�_

	XMFLOAT4X4					g_mProj;				// �ˉe�ϊ��s��
	XMFLOAT4X4					g_mView;				// �r���[�ϊ��s��
	XMFLOAT4X4					g_mWorld;				// ���[���h�ϊ��s��
	XMFLOAT4X4					g_mTex;					// �e�N�X�`���ϊ��s��

	XMFLOAT2					x2UVFrame;
	bool						bAnimationHorizontal;
	bool						bUsesAnimation;
	bool						bMoveToNextV;
	int							nAnimeFrameChange;
	int							nFrameAnimCounter;
	Polygon2D*					pParent;
public:
	ID3D11ShaderResourceView *	gpTexture;
	XMFLOAT2					x2UV;			// UV���W
	XMFLOAT2					x2UVMaxFrameSize;			// UV���W

	Polygon2D();
	Polygon2D(const char * TexturePath);
	~Polygon2D();
	HRESULT InitPolygon(ID3D11Device* pDevice);

	void UninitPolygon();
	void UpdatePolygon();
	void DrawPolygon(ID3D11DeviceContext* pDeviceContext);
	void SetTexture(ID3D11ShaderResourceView* pTexture);
	void SetPosition(float fX, float fY);
	void SetRotation(float rX, float rY, float rZ) { Rotation = { rX, rY, rZ }; };
	void SetPolygonPosY(float y);
	void SetPosition(float fX, float fY, bool IsInit);
	void SetSize(float fScaleX, float fScaleY);
	void SetPolygonAngle(float fAngle);
	void SetPolygonUV(float fU, float fV);
	void SetPolygonFrameSize(float fWidth, float fHeight);
	void SetUVSize(float fWidth, float fHeight);
	void SetColor(float fRed, float fGreen, float fBlue);
	void SetAlpha(float fAlpha);
	void RaiseAlpha(float up);
	void ScaleUp(float scale);
	void RotateAroundY(float frot);
	void RotateAroundZ(float frot);
	float GetRotationZ() { return Rotation.z; };
	HRESULT MakeVertexPolygon(ID3D11Device* pDevice);
	void SetVertexPolygon(void);
	void Translate(XMFLOAT2);
	XMFLOAT2 GetSize();
	XMFLOAT2 GetPosition();
	XMFLOAT2 GetPolygonInitialPos();
	void bScaleUp(float scal);
	void SetMoveableOnUV(bool flag) { bMoveToNextV = flag; };
	void SetSpeedAnimationFrameChange(int speed) { nFrameAnimCounter = nAnimeFrameChange = speed; bUsesAnimation = true; }
	XMFLOAT2 GetUVFrames() { return x2UVFrame; };
	float GetAlpha() { return Color.w; };
};
#endif