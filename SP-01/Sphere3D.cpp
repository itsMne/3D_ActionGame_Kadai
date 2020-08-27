//*****************************************************************************
// Sphere3D.cpp
// スフィアのオブジェクト
// Author: Mane
//*****************************************************************************
#include "Sphere3D.h"
#include "main.h"
#include "Texture.h"
#include "Player3D.h"
#include "TechnicalCamera.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define USE_16_SLASHES true
#define SKYBOX_SIZE 900

//*****************************************************************************
// コンストラクタ関数
//*****************************************************************************
Sphere3D::Sphere3D(XMFLOAT3 pos, XMFLOAT3 rot, int nSlice, int nStack, float fRadius, const char* szPath)
{
	Init(pos, rot, nSlice, nStack, fRadius, szPath);
	fRotSpeed = { 0,0,0 };
	bIsSkybox = false;
}

Sphere3D::Sphere3D(const char * szPath, bool skybox)
{
#if USE_16_SLASHES
	Init({ 0,50,100 }, { 0,0,0 }, 16, 8, SKYBOX_SIZE, szPath);
#else
	Init({ 0,50,100 }, { 0,0,0 }, 32, 16, SKYBOX_SIZE, szPath);
#endif
	bisUnlit = true;
	bNoCull = true;
	fRotSpeed = { 0,0.0001f,0 };
	bIsSkybox = skybox;
}

Sphere3D::~Sphere3D()
{
}

//*****************************************************************************
//Init関数
//初期化関数
//引数：void
//戻：void
//*****************************************************************************
HRESULT Sphere3D::Init(XMFLOAT3 pos, XMFLOAT3 rot, int nSlice, int nStack, float fRadius, const char* szPath)
{
	Mesh3D::Init();
	ID3D11Device* pDevice = GetDevice();
	HRESULT hr;

	// テクスチャ マトリックス初期化
	XMStoreFloat4x4(&mtxTexture, XMMatrixIdentity());

	// 位置、向きの初期設定
	Position = pos;
	Rotation = rot;

	// テクスチャの読み込み
	hr = CreateTextureFromFile(pDevice,					// デバイスへのポインタ
		szPath,		// ファイルの名前
		&pTexture);	// 読み込むメモリー
	if (FAILED(hr))
		return hr;

	// 頂点数の設定
	nNumVertex = (nSlice + 1) * (nSlice + 1);

	// インデックス数の設定
	nNumIndex = (nSlice + 1) * 2 * nStack + (nStack - 1) * 2;


	// オブジェクトの頂点バッファを生成
	VERTEX_3D* pVertexWk = new VERTEX_3D[nNumVertex];

	// オブジェクトのインデックスバッファを生成
	int* pIndexWk = new int[nNumIndex];

	// 頂点バッファの中身を埋める
	VERTEX_3D* pVtx = pVertexWk;
	const float texSizeX = 1.0f / nSlice;
	const float texSizeZ = 1.0f / nStack;


	for (int nCntVtxY = 0; nCntVtxY < nStack + 1; ++nCntVtxY) {

		for (int nCntVtxX = 0; nCntVtxX < nSlice + 1; ++nCntVtxX) {
			// 頂点座標の設定
			pVtx[nCntVtxY * (nSlice + 1) + nCntVtxX].vtx.x = cosf(nCntVtxX * XM_PI / (nSlice / 2)) * fRadius * sinf(nCntVtxY * XM_PI / nStack);
			pVtx[nCntVtxY * (nSlice + 1) + nCntVtxX].vtx.y = sinf(((nStack / 2) - nCntVtxY) * XM_PI / nStack)*fRadius;
			pVtx[nCntVtxY * (nSlice + 1) + nCntVtxX].vtx.z = sinf(nCntVtxX * XM_PI / (nSlice / 2)) * fRadius * sinf(nCntVtxY * XM_PI / nStack);

			// 法線の設定
			pVtx[nCntVtxY * (nSlice + 1) + nCntVtxX].nor = XMFLOAT3(0.0f, 1.0, 0.0f);

			// 反射光の設定
			pVtx[nCntVtxY * (nSlice + 1) + nCntVtxX].diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

			// テクスチャ座標の設定
			pVtx[nCntVtxY * (nSlice + 1) + nCntVtxX].tex.x = texSizeX * nCntVtxX;
			pVtx[nCntVtxY * (nSlice + 1) + nCntVtxX].tex.y = texSizeZ * nCntVtxY;
		}
	}

	// インデックスバッファの中身を埋める
	int* pIdx = pIndexWk;

	int nCntIdx = 0;
	for (int nCntVtxY = 0; nCntVtxY < nStack; ++nCntVtxY) {
		if (nCntVtxY > 0) {
			// 縮退ポリゴンのためのダブりの設定
			pIdx[nCntIdx] = (nCntVtxY + 1) * (nSlice + 1);
			++nCntIdx;
		}

		for (int nCntVtxX = 0; nCntVtxX < nSlice + 1; ++nCntVtxX) {
			pIdx[nCntIdx] = (nCntVtxY + 1) * (nSlice + 1) + nCntVtxX;
			++nCntIdx;
			pIdx[nCntIdx] = nCntVtxY * (nSlice + 1) + nCntVtxX;
			++nCntIdx;
		}

		if (nCntVtxY < nStack - 1) {
			// 縮退ポリゴンのためのダブりの設定
			pIdx[nCntIdx] = nCntVtxY * (nSlice + 1) + nSlice;
			++nCntIdx;
		}
	}

	// 頂点バッファ/インデックス バッファ生成
	hr = MakeMeshVertex(pDevice, pVertexWk, pIndexWk);

	// 一時配列解放
	delete[] pIndexWk;
	delete[] pVertexWk;

	return hr;
}

//*****************************************************************************
//Uninit関数
//終了関数
//引数：void
//戻：void
//*****************************************************************************
void Sphere3D::Uninit(void)
{
	Mesh3D::End();
}

//*****************************************************************************
//Update関数
//変更関数
//引数：void
//戻：void
//*****************************************************************************
void Sphere3D::Update(void)
{
	Mesh3D::Update();
	Rotation.x += fRotSpeed.x;
	Rotation.y += fRotSpeed.y;
	Rotation.z += fRotSpeed.z;

	//printf("a");
	
	if (bIsSkybox) {
		TechCamera* pCam = GetMainCamera();
		if (!pCam)
			return;
		Position = pCam->GetCameraPos();
		Position.y -= 150;
		Player3D* Player = GetPlayer();
		if (!Player)
			return;
		if (Player->GetDizzySkybox()->GetScaleX() >= 2.1f)
			SetScale({0,0,0});
		else
			SetScale({ 2.5f, 2.5f, 2.5f });
	}
	//Scale = { 2.5f, 2.5f, 2.5f };
}

//*****************************************************************************
//Draw関数
//レンダリング関数
//引数：void
//戻：void
//*****************************************************************************
void Sphere3D::Draw(void)
{
	if (Scale.x == 0)
		return;
	Mesh3D::Draw(GetDeviceContext());
}

//*****************************************************************************
//SetUnlit関数
//光を使うことを設定する
//引数：bool
//戻：void
//*****************************************************************************
void Sphere3D::SetUnlit(bool isUnlit)
{
	bisUnlit = isUnlit;
}
