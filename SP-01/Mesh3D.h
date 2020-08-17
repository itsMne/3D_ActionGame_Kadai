//*****************************************************************************
// Mesh3D.h
//*****************************************************************************
#pragma once
#include "DXWindow3D.h"
#include "Object3D.h"

//*****************************************************************************
// エナム
//*****************************************************************************
enum ePrimitiveType {
	PT_UNDEFINED = 0,
	PT_POINT,
	PT_LINE,
	PT_LINESTRIP,
	PT_TRIANGLE,
	PT_TRIANGLESTRIP,

	MAX_PRIMITIVETYPE
};

//*****************************************************************************
// 構造体定義
//*****************************************************************************
typedef struct {
	XMFLOAT4	Diffuse;		// Diffuse color RGBA
	XMFLOAT4	Ambient;		// Ambient color RGB
	XMFLOAT4	Specular;		// Specular 'shininess'
	XMFLOAT4	Emissive;		// Emissive color RGB
	float		Power;			// Sharpness if specular highlight
} MATERIAL;

typedef struct {
	ID3D11Buffer* pVertexBuffer;			// 頂点バッファインターフェースへのポインタ
	ID3D11Buffer* pIndexBuffer;				// インデックスバッファインターフェースへのポインタ

	XMFLOAT4X4 mtxWorld;					// ワールドマトリックス
	XMFLOAT3 Position;							// ポリゴン表示位置の中心座標
	XMFLOAT3 Rotation;							// ポリゴンの回転角	
	int nNumVertex;							// 総頂点数	
	int nNumIndex;							// 総インデックス数

	XMFLOAT4X4 mtxTexture;					// テクスチャ マトリックス
	ID3D11ShaderResourceView* pTexture;		// テクスチャ

	ePrimitiveType primitiveType;			// プリミティブ型

	MATERIAL* pMaterial = nullptr;					// マテリアル
} MESH;

//*****************************************************************************
// クラス
//*****************************************************************************
class Mesh3D: public Object3D
{
protected:

	ID3D11Buffer*				g_pConstantBuffer[2];	// 定数バッファ
	ID3D11SamplerState*			g_pSamplerState;		// テクスチャ サンプラ
	ID3D11VertexShader*			g_pVertexShader;		// 頂点シェーダ
	ID3D11InputLayout*			g_pInputLayout;			// 頂点フォーマット
	ID3D11PixelShader*			g_pPixelShader;			// ピクセルシェーダ
	MESH* pMesh;
	// マテリアル
	MATERIAL						g_material;
	int							g_nAlpha;			// アルファテストの閾値
	bool bisUnlit;
	bool bNoCull;
	bool bUsingOutsideTexture;
public:
	Mesh3D();
	~Mesh3D();
	virtual void Init();
	virtual void Update();
	void Draw(ID3D11DeviceContext* pDeviceContext);
	virtual void End();
	float GetAlphaTestValue(void);
	HRESULT MakeMeshVertex(ID3D11Device* pDevice, MESH* pMesh,
		VERTEX_3D vertexWk[], int indexWk[]);
	void ReleaseMesh();
	void SetNoCull(bool);
	void SetUnlit(bool);
};


