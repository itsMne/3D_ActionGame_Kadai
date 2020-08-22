//*****************************************************************************
// Billboard2D.cpp
// ビールボードを管理する
// Author : Mane
//*****************************************************************************
#include "Billboard2D.h"
#include "Texture.h"
#include "Mesh3D.h"
#include "TechnicalCamera.h"
#include "input.h"
#include "debugproc.h"
#include "Light3D.h"

char szBBTexturePath[MAX_BB_TEX][256] = {
"data/texture/Hit.tga",
"data/texture/Heart.tga",
"data/texture/battery.tga",
"data/texture/batteryenergy.tga",
};

ID3D11ShaderResourceView* pBBTextures[MAX_BB_TEX] = { nullptr };

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define	VALUE_MOVE_TREE		(3.00f)							// 移動速度

//*****************************************************************************
//コンストラクタ関数
//*****************************************************************************
Billboard::Billboard(int nPpath, XMFLOAT2 Size): Mesh3D()
{
	Color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	pTexture = nullptr;
	nSlowness = 0;
	bUsingOutsideTexture = true;
	x2Size = Size;
	if (nPpath < MAX_BB_TEX && !pBBTextures[nPpath]) {
		ID3D11Device* pDevice = GetDevice();
		printf("PATH: %s\n", szBBTexturePath[nPpath]);
		CreateTextureFromFile(pDevice, szBBTexturePath[nPpath], &pBBTextures[nPpath]);
		if (pBBTextures[nPpath])
			printf("TEXTURE OK\n");
	}

	Init("");
	pTexture = pBBTextures[nPpath];
	bUse = true;
}

Billboard::Billboard(int nPpath, XMFLOAT2 Size, XMFLOAT4 Colors)
{
	Color = Colors;
	pTexture = nullptr;
	nSlowness = 0;
	bUsingOutsideTexture = true;
	x2Size = Size;
	if (nPpath < MAX_BB_TEX && !pBBTextures[nPpath]) {
		ID3D11Device* pDevice = GetDevice();
		printf("PATH: %s\n", szBBTexturePath[nPpath]);
		CreateTextureFromFile(pDevice, szBBTexturePath[nPpath], &pBBTextures[nPpath]);
		if (pBBTextures[nPpath])
			printf("TEXTURE OK\n");
	}

	Init("");
	pTexture = pBBTextures[nPpath];
	bUse = true;
	
}

Billboard::Billboard(ID3D11ShaderResourceView * texture): Mesh3D()
{
	pTexture = texture;
	Init("");
}


Billboard::~Billboard()
{
	End();
}

//*****************************************************************************
//Init関数
//初期化関数
//引数：void
//戻：HRESULT
//*****************************************************************************
HRESULT Billboard::Init(const char* szpath)
{
	ID3D11Device* pDevice = GetDevice();
	Position = XMFLOAT3(0.0f, 0.0f, 0.0f);
	Scale = XMFLOAT3(1.0f, 1.0f, 1.0f);
	Rotation = XMFLOAT3(0.0f, 0.0f, 0.0f);
	MakeVertex(pDevice);

	// テクスチャの読み込み
	

	//pMesh->pTexture = pTexture;
	//XMStoreFloat4x4(&pMesh->mtxTexture, XMMatrixIdentity());

	// マテリアルの設定
	g_nAlpha = 0;
	fWidth = 10;
	fHeight = 10;

	g_bAlphaTest = false;
	g_nAlpha = 0x0;
	g_bInTree = false;
	uv = { 0,0 };
	nAnimeCount = 3;
	SetBillboard(XMFLOAT3(0.0f, 0.0f, 0.0f), 60.0f*x2Size.x, 60.0f*x2Size.y, XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
	Rotation = {0,0,0};
	nFrameX = 1;
	nFrameY = 1;
	bUse = true;
	return S_OK;
}

//*****************************************************************************
//Update関数
//変更関数
//引数：void
//戻：void
//*****************************************************************************
void Billboard::Update()
{
	Mesh3D::Update();
	TechCamera* camera = GetMainCamera();
	//Rotation.y= camera->GetCameraAngle().y;
	Rotation.y = XM_PI+ camera->GetCameraAngle().y;
	//return;
	if (++nAnimeCount >= nSlowness) {
		++uv.U;
		if (uv.U >= (float)nFrameX) {
			uv.U = 0;
			uv.V++;
			if (uv.V >= (float)nFrameY)
			{
				uv.V = 0;
				if(bSetFalseAfterAnimation)
					bUse = false;
			}	
		}
		nAnimeCount = 0;
	}
}

//*****************************************************************************
//Draw関数
//レンダリング関数
//引数：void
//戻：void
//*****************************************************************************
void Billboard::Draw()
{
	if (!bUse)
		return;
	GetMainLight()->SetLightEnable(false);
	ID3D11DeviceContext* pDeviceContext = GetDeviceContext();
	XMMATRIX mtx_World, mtxScale, mtxRotate, mtxTranslate;
	if (!GetMainCamera()) {
		return;
	}
	XMFLOAT4X4& mtxView = GetMainCamera()->GetViewMatrix();

	g_bInTree = true;

	// ワールドマトリックスの初期化
	mtx_World = XMMatrixIdentity();
	XMStoreFloat4x4(&mtxWorld, mtx_World);

	// ポリゴンを正面に向ける
	this->mtxWorld._11 = mtxView._11;
	this->mtxWorld._12 = mtxView._21;
	this->mtxWorld._13 = mtxView._31;
	this->mtxWorld._21 = mtxView._12;
	this->mtxWorld._22 = mtxView._22;
	this->mtxWorld._23 = mtxView._32;
	this->mtxWorld._31 = mtxView._13;
	this->mtxWorld._32 = mtxView._23;
	this->mtxWorld._33 = mtxView._33;

	// スケールを反映
	mtxScale = XMMatrixScaling(fWidth,
		fHeight, 1.0f);
	mtx_World = XMMatrixMultiply(mtxScale, mtx_World);
	mtxRotate = XMMatrixRotationRollPitchYaw(Rotation.x, Rotation.y, Rotation.z);
	mtx_World = XMMatrixMultiply(mtx_World, mtxRotate);

	
	// 移動を反映
	mtxTranslate = XMMatrixTranslation(Position.x,
		Position.y,
		Position.z);
	mtx_World = XMMatrixMultiply(mtx_World, mtxTranslate);

	// ワールドマトリックスの設定
	XMStoreFloat4x4(&this->mtxWorld, mtx_World);

	// メッシュの描画
	XMMATRIX mtxTex = XMMatrixScaling(1.0f / (float)nFrameX, 1.0f / (float)nFrameY, 0.0f);
	mtxTex *= XMMatrixTranslation((float)uv.U / (float)nFrameX, (float)uv.V / (float)nFrameY, 0.0f);
	XMStoreFloat4x4(&mtxTexture, mtxTex);


	Mesh3D::Draw(pDeviceContext);
	g_bInTree = false;
	GetMainLight()->SetLightEnable(true);
}

//*****************************************************************************
//End関数
//終了関数
//引数：void
//戻：void
//*****************************************************************************
void Billboard::End()
{
	// メッシュの開放
	if(!bUsingOutsideTexture)
		SAFE_RELEASE(pTexture);
	ReleaseMesh();
}

//*****************************************************************************
//SetBillboard関数
//ビールボードを設定する
//引数：XMFLOAT3, float, float, XMFLOAT4
//戻：void
//*****************************************************************************
void Billboard::SetBillboard(XMFLOAT3 pos, float fWidth, float fHeight, XMFLOAT4 color)
{
	pos = pos;
	bUse = true;
	// 頂点座標の設定
	SetVertex(fWidth, fHeight);
	// 頂点カラーの設定
	SetColor(color);
}

//*****************************************************************************
//MakeVertex関数
//バーテックスを作る
//引数：ID3D11Device*
//戻：HRESULT
//*****************************************************************************
HRESULT Billboard::MakeVertex(ID3D11Device * pDevice)
{
	nNumVertex = 4;
	nNumIndex = 4;

	// オブジェクトの頂点配列を生成
	VERTEX_3D* pVertexWk = new VERTEX_3D[nNumVertex];

	// 頂点配列の中身を埋める
	VERTEX_3D* pVtx = pVertexWk;

	// 頂点座標の設定
	pVtx[0].vtx = XMFLOAT3(-1.0f / 2, 0.0f, 0.0f);
	pVtx[1].vtx = XMFLOAT3(-1.0f / 2, 1.0f, 0.0f);
	pVtx[2].vtx = XMFLOAT3(1.0f / 2, 0.0f, 0.0f);
	pVtx[3].vtx = XMFLOAT3(1.0f / 2, 1.0f, 0.0f);

	// 法線の設定
	pVtx[0].nor = XMFLOAT3(0.0f, 0.0f, -1.0f);
	pVtx[1].nor = XMFLOAT3(0.0f, 0.0f, -1.0f);
	pVtx[2].nor = XMFLOAT3(0.0f, 0.0f, -1.0f);
	pVtx[3].nor = XMFLOAT3(0.0f, 0.0f, -1.0f);

	// 反射光の設定
	pVtx[0].diffuse = Color;
	pVtx[1].diffuse = Color;
	pVtx[2].diffuse = Color;
	pVtx[3].diffuse = Color;

	// テクスチャ座標の設定
	pVtx[0].tex = XMFLOAT2(0.0f, 1.0f);
	pVtx[1].tex = XMFLOAT2(0.0f, 0.0f);
	pVtx[2].tex = XMFLOAT2(1.0f, 1.0f);
	pVtx[3].tex = XMFLOAT2(1.0f, 0.0f);

	// インデックス配列を生成
	int* pIndexWk = new int[nNumIndex];

	// インデックス配列の中身を埋める
	pIndexWk[0] = 0;
	pIndexWk[1] = 1;
	pIndexWk[2] = 2;
	pIndexWk[3] = 3;

	// 頂点バッファ生成
	HRESULT hr = MakeMeshVertex(pDevice, pVertexWk, pIndexWk);
	// 一時配列の解放
	delete[] pIndexWk;
	delete[] pVertexWk;

	return hr;
}

//*****************************************************************************
//SetVertex関数
//バーテックスを設定する
//引数：float, float
//戻：void
//*****************************************************************************
void Billboard::SetVertex(float fWidth, float fHeight)
{
	this->fWidth = fWidth;
	this->fHeight = fHeight;
}

//*****************************************************************************
//SetColor関数
//色を設定する
//引数：XMFLOAT4
//戻：void
//*****************************************************************************
void Billboard::SetColor(XMFLOAT4 col)
{
	this->Color = col;
	if (this->Color.x < 0)
		this->Color.x = 0;
	if (this->Color.y < 0)
		this->Color.y = 0;
	if (this->Color.z < 0)
		this->Color.z = 0;
	if (this->Color.w < 0)
		this->Color.w = 0;
}

//*****************************************************************************
//SetPosition関数
//拠点を設定する
//引数：XMFLOAT3
//戻：void
//*****************************************************************************
void Billboard::SetPosition(XMFLOAT3 newPosition)
{
	Position = newPosition;
}

//*****************************************************************************
//SetWidth関数
//幅を設定する
//引数：float
//戻：void
//*****************************************************************************
void Billboard::SetWidth(float newWidth)
{
	fWidth = newWidth;
}

//*****************************************************************************
//SetHeight関数
//高さを設定する
//引数：float
//戻：void
//*****************************************************************************
void Billboard::SetHeight(float newHeight)
{
	fHeight = newHeight;
}

//*****************************************************************************
//SetUVFrames関数
//UVフレームを設定する
//引数：int, int
//戻：void
//*****************************************************************************
void Billboard::SetUVFrames(int nX, int nY, int nSlow)
{
	nFrameX = nX;
	nFrameY = nY;
	nSlowness = nSlow;
}

//*****************************************************************************
//SetScale関数
//大きさを設定する
//引数：float
//戻：void
//*****************************************************************************
void Billboard::SetScale(float nScale)
{
	fHeight = nScale;
	fWidth = nScale;
}

void Billboard::SetScale(float Height, float Width)
{
	fHeight = Height;
	fWidth = Width;
}

//*****************************************************************************
//ScaleUp関数
//大きさを増やす
//引数：float
//戻：void
//*****************************************************************************
void Billboard::ScaleUp(float nS_Up)
{
	fHeight += nS_Up;
	fWidth  += nS_Up;
}

//*****************************************************************************
//GetAlpha関数
//アルファを戻す
//引数：void
//戻：float
//*****************************************************************************
float Billboard::GetAlpha()
{
	return Color.w;
}

//*****************************************************************************
//GetUse関数
//使うことを確認する
//引数：void
//戻：bool
//*****************************************************************************
bool Billboard::GetUse()
{
	return bUse;
}

//*****************************************************************************
//SetUse関数
//使うことを設定する
//引数：bool
//戻：void
//*****************************************************************************
void Billboard::SetUse(bool use)
{
	bUse = use;
}

//*****************************************************************************
//ResetUV関数
//UVをリセットする
//引数：void
//戻：void
//*****************************************************************************
void Billboard::ResetUV()
{
	uv = { 0,0 };
}

//*****************************************************************************
//SetUnusableAfterAnimation関数
//アニメーションが終わったら、オブジェクトの状態が使わないことにする
//引数：bool
//戻：void
//*****************************************************************************
void Billboard::SetUnusableAfterAnimation(bool inv)
{
	bSetFalseAfterAnimation = inv;
}

void ReleaseAllBillboardTextures()
{
	for (int i = 0; i < MAX_BB_TEX; i++)
		SAFE_RELEASE(pBBTextures[i]);
}
