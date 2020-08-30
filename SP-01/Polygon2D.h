//*****************************************************************************
// Polygon2D.h
//*****************************************************************************
#ifndef	 POLYGON_H
#define  POLYGON_H
#include "main.h"
#include "UniversalStructures.h"

//*****************************************************************************
// エナム
//*****************************************************************************
enum Direction
{
	RIGHT_DIR = 1,
	LEFT_DIR = -1
};

//*****************************************************************************
// クラス
//*****************************************************************************
class Polygon2D
{
private:
	bool bPolygonInited;
protected:
	ID3D11ShaderResourceView*	g_pTexture;				// テクスチャへのポインタ

	VERTEX_2D					g_vertexWk[NUM_VERTEX];	// 頂点情報格納ワーク

	XMFLOAT3						Position;			// ポリゴンの移動量
	XMFLOAT3						InitPosition;			// ポリゴンの移動量
	XMFLOAT3						Rotation;			// ポリゴンの回転量
	XMFLOAT3						Scale;			// ポリゴンのサイズ
	XMFLOAT4						Color;			// ポリゴンの頂点カラー
	bool							g_bInvalidate;			// 頂点データ更新フラグ

	
	XMFLOAT2						x2Frame;			// テクスチャ抽出サイズ

	ID3D11Buffer*				g_pConstantBuffer;		// 定数バッファ
	ID3D11Buffer*				g_pVertexBuffer;		// 頂点バッファ
	ID3D11SamplerState*			g_pSamplerState;		// テクスチャ サンプラ
	ID3D11VertexShader*			g_pVertexShader;		// 頂点シェーダ
	ID3D11InputLayout*			g_pInputLayout;			// 頂点フォーマット
	ID3D11PixelShader*			g_pPixelShader;			// ピクセルシェーダ

	XMFLOAT4X4					g_mProj;				// 射影変換行列
	XMFLOAT4X4					g_mView;				// ビュー変換行列
	XMFLOAT4X4					g_mWorld;				// ワールド変換行列
	XMFLOAT4X4					g_mTex;					// テクスチャ変換行列

	XMFLOAT2					x2UVFrame;
	bool						bAnimationHorizontal;
	bool						bUsesAnimation;
	bool						bMoveToNextV;
	int							nAnimeFrameChange;
	int							nFrameAnimCounter;
	Polygon2D*					pParent;
public:
	ID3D11ShaderResourceView *	gpTexture;
	XMFLOAT2					x2UV;			// UV座標
	XMFLOAT2					x2UVMaxFrameSize;			// UV座標

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