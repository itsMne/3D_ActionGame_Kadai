//*****************************************************************************
// Mesh3D.cpp
// メッシュを管理する
// Author : Mane
//*****************************************************************************
#include "Mesh3D.h"
#include "TechnicalCamera.h"
#include "Light3D.h"
#include "Shader.h"
#include "Texture.h"
#include "Billboard2D.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define M_DIFFUSE		XMFLOAT4(1.0f,1.0f,1.0f,1.0f)
#define M_SPECULAR		XMFLOAT4(0.0f,0.0f,0.0f,0.0f)
#define M_AMBIENT		XMFLOAT4(0.0f,0.0f,0.0f,1.0f)
#define M_EMISSIVE		XMFLOAT4(0.0f,0.0f,0.0f,0.0f)

//*****************************************************************************
// 構造体定義
//*****************************************************************************
// シェーダに渡す値
struct SHADER_GLOBAL {
	XMMATRIX	mWVP;		// ワールド×ビュー×射影行列(転置行列)
	XMMATRIX	mW;			// ワールド行列(転置行列)
	XMMATRIX	mTex;		// テクスチャ行列(転置行列)
};
struct SHADER_GLOBAL2 {
	XMVECTOR	vEye;		// 視点座標
	// 光源
	XMVECTOR	vLightDir;	// 光源方向
	XMVECTOR	vLa;		// 光源色(アンビエント)
	XMVECTOR	vLd;		// 光源色(ディフューズ)
	XMVECTOR	vLs;		// 光源色(スペキュラ)
	// マテリアル
	XMVECTOR	vAmbient;	// アンビエント色(+テクスチャ有無)
	XMVECTOR	vDiffuse;	// ディフューズ色
	XMVECTOR	vSpecular;	// スペキュラ色(+スペキュラ強度)
	XMVECTOR	vEmissive;	// エミッシブ色(+アルファテスト閾値)
};

//*****************************************************************************
// コンストラクタ関数
//*****************************************************************************
Mesh3D::Mesh3D() //: Object3D()
{
	Init();
}


Mesh3D::~Mesh3D()
{
}

//*****************************************************************************
//Init関数
//初期化関数
//引数：void
//戻：void
//*****************************************************************************
void Mesh3D::Init()
{
	ID3D11Device* pDevice = GetDevice();
	HRESULT hr;
	bUsingOutsideTexture = false;
	// シェーダ初期化
	static const D3D11_INPUT_ELEMENT_DESC layout[] = {
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0, 0,                            D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,       0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
	};
	hr = LoadShader("Vertex", "Pixel",
		&g_pVertexShader, &g_pInputLayout, &g_pPixelShader, layout, _countof(layout));
	if (FAILED(hr)) {
		printf("failed\n");
		return;
	}
	// 定数バッファ生成
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(SHADER_GLOBAL);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;
	hr = pDevice->CreateBuffer(&bd, nullptr, &g_pConstantBuffer[0]);
	if (FAILED(hr)) return;
	bd.ByteWidth = sizeof(SHADER_GLOBAL2);
	hr = pDevice->CreateBuffer(&bd, nullptr, &g_pConstantBuffer[1]);
	if (FAILED(hr)) return;

	// テクスチャ サンプラ生成
	D3D11_SAMPLER_DESC sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sd.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sd.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sd.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	hr = pDevice->CreateSamplerState(&sd, &g_pSamplerState);
	if (FAILED(hr)) {
		printf("failed\n");
		return;
	}

	// マテリアルの初期設定
	g_material.Diffuse = M_DIFFUSE;
	g_material.Ambient = M_AMBIENT;
	g_material.Specular = M_SPECULAR;
	g_material.Power = 0.0f;
	g_material.Emissive = M_EMISSIVE;
	
	//return hr;
}

//*****************************************************************************
//Update関数
//変更関数
//引数：void
//戻：void
//*****************************************************************************
void Mesh3D::Update()
{
	XMMATRIX xWorld, mtxRot, mtxTranslate, mtxScale;


	// ワールドマトリックスの初期化
	xWorld = XMMatrixIdentity();

	// 移動を反映
	mtxScale = XMMatrixScaling(Scale.x, Scale.y, Scale.z);
	xWorld = XMMatrixMultiply(xWorld, mtxScale);

	// 回転を反映
	mtxRot = XMMatrixRotationRollPitchYaw(Rotation.x, Rotation.y, Rotation.z);
	xWorld = XMMatrixMultiply(xWorld, mtxRot);

	// 移動を反映
	mtxTranslate = XMMatrixTranslation(Position.x, Position.y, Position.z);
	xWorld = XMMatrixMultiply(xWorld, mtxTranslate);


	// ワールドマトリックスの設定
	XMStoreFloat4x4(&mtxWorld, xWorld);
}

//*****************************************************************************
//Draw関数
//レンダリング関数
//引数：void
//戻：void
//*****************************************************************************
void Mesh3D::Draw(ID3D11DeviceContext* pDeviceContext)
{
	if (bisUnlit || NO_LIGHT_DEFAULT)
		GetMainLight()->SetLightEnable(false);
	if (bNoCull)
		SetCullMode(CULLMODE_NONE);
	// シェーダ設定
	pDeviceContext->VSSetShader(g_pVertexShader, nullptr, 0);
	pDeviceContext->PSSetShader(g_pPixelShader, nullptr, 0);
	pDeviceContext->IASetInputLayout(g_pInputLayout);

	// 頂点バッファをセット
	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	pDeviceContext->IASetVertexBuffers(0, 1, &pVertexBuffer, &stride, &offset);
	// インデックスバッファをセット
	pDeviceContext->IASetIndexBuffer(pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

	pDeviceContext->PSSetSamplers(0, 1, &g_pSamplerState);

	pDeviceContext->PSSetShaderResources(0, 1, &pTexture);

	SHADER_GLOBAL cb;
	XMMATRIX mtx_World = XMLoadFloat4x4(&mtxWorld);
	TechCamera* camera = GetMainCamera();
	cb.mWVP = XMMatrixTranspose(mtx_World *
		XMLoadFloat4x4(&camera->GetViewMatrix()) * XMLoadFloat4x4(&camera->GetProjMatrix()));
	cb.mW = XMMatrixTranspose(mtx_World);
	cb.mTex = XMMatrixTranspose(XMLoadFloat4x4(&mtxTexture));
	pDeviceContext->UpdateSubresource(g_pConstantBuffer[0], 0, nullptr, &cb, 0, 0);
	pDeviceContext->VSSetConstantBuffers(0, 1, &g_pConstantBuffer[0]);
	SHADER_GLOBAL2 cb2;
	cb2.vEye = XMLoadFloat3(&camera->GetCameraPos());
	CFbxLight& light = *GetMainLight()->GetLight();
	cb2.vLightDir = XMVectorSet(light.m_direction.x, light.m_direction.y, light.m_direction.z, 0.f);
	cb2.vLa = XMLoadFloat4(&light.m_ambient);
	cb2.vLd = XMLoadFloat4(&light.m_diffuse);
	cb2.vLs = XMLoadFloat4(&light.m_specular);
	//MATERIAL* pMaterial = pMaterial;
	if (!pMaterial) pMaterial = &g_material;
	cb2.vDiffuse = XMLoadFloat4(&pMaterial->Diffuse);
	cb2.vAmbient = XMVectorSet(pMaterial->Ambient.x, pMaterial->Ambient.y, pMaterial->Ambient.z,
		(pTexture != nullptr) ? 1.f : 0.f);
	cb2.vSpecular = XMVectorSet(pMaterial->Specular.x, pMaterial->Specular.y, pMaterial->Specular.z, pMaterial->Power);
	cb2.vEmissive = XMVectorSet(pMaterial->Emissive.x, pMaterial->Emissive.y, pMaterial->Emissive.z, GetAlphaTestValue());
	pDeviceContext->UpdateSubresource(g_pConstantBuffer[1], 0, nullptr, &cb2, 0, 0);
	pDeviceContext->PSSetConstantBuffers(1, 1, &g_pConstantBuffer[1]);

	// プリミティブ形状をセット
	static const D3D11_PRIMITIVE_TOPOLOGY pt[] = {
		D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP,	// 0なら三角形ストリップ
		D3D11_PRIMITIVE_TOPOLOGY_POINTLIST,
		D3D11_PRIMITIVE_TOPOLOGY_LINELIST,
		D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP,
		D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
		D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP,
	};
	primitiveType = PT_TRIANGLESTRIP;
	pDeviceContext->IASetPrimitiveTopology(pt[primitiveType]);

	// ポリゴンの描画
	pDeviceContext->DrawIndexed(nNumIndex, 0, 0);
	if (bisUnlit)
		GetMainLight()->SetLightEnable(true);
	if (bNoCull)
		SetCullMode(CULLMODE_CCW);
}

//*****************************************************************************
//End関数
//終了関数
//引数：void
//戻：void
//*****************************************************************************
void Mesh3D::End()
{
	// テクスチャ サンプラの開放
	SAFE_RELEASE(g_pSamplerState);
	// 定数バッファの解放
	for (int i = 0; i < _countof(g_pConstantBuffer); ++i) {
		SAFE_RELEASE(g_pConstantBuffer[i]);
	}
	// ピクセルシェーダ解放
	SAFE_RELEASE(g_pPixelShader);
	// 頂点フォーマット解放
	SAFE_RELEASE(g_pInputLayout);
	// 頂点シェーダ解放
	SAFE_RELEASE(g_pVertexShader);

}


//*****************************************************************************
//GetAlphaTestValue関数
//アルファを戻す
//引数：void
//戻：float
//*****************************************************************************
float Mesh3D::GetAlphaTestValue(void)
{
	//return 0.5f;
	return g_nAlpha / 255.0f;
}

//*****************************************************************************
//MakeMeshVertex関数
//バーテックスを設定する
//引数：ID3D11Device*, MESH*, VERTEX_3D, int
//戻：HRESULT
//*****************************************************************************
HRESULT Mesh3D::MakeMeshVertex(ID3D11Device * pDevice, VERTEX_3D vertexWk[], int indexWk[])
{
	D3D11_BUFFER_DESC vbd;
	ZeroMemory(&vbd, sizeof(vbd));
	vbd.Usage = D3D11_USAGE_DYNAMIC;
	vbd.ByteWidth = sizeof(VERTEX_3D) * nNumVertex;
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vbd.MiscFlags = 0;
	D3D11_SUBRESOURCE_DATA initData;
	ZeroMemory(&initData, sizeof(initData));
	initData.pSysMem = vertexWk;
	HRESULT hr = pDevice->CreateBuffer(&vbd, &initData, &pVertexBuffer);
	if (FAILED(hr)) {
		return hr;
	}

	CD3D11_BUFFER_DESC ibd(nNumIndex * sizeof(int), D3D11_BIND_INDEX_BUFFER);
	ZeroMemory(&initData, sizeof(initData));
	initData.pSysMem = indexWk;
	hr = pDevice->CreateBuffer(&ibd, &initData, &pIndexBuffer);

	return hr;
}

//*****************************************************************************
//ReleaseMesh関数
//メッシュを消す
//引数：void
//戻：void
//*****************************************************************************
void Mesh3D::ReleaseMesh()
{
	// テクスチャ解放
	if(!bUsingOutsideTexture)
		SAFE_RELEASE(pTexture);
	// 頂点バッファ解放
	SAFE_RELEASE(pVertexBuffer);
	// インデックス バッファ解放
	SAFE_RELEASE(pIndexBuffer);
}

//*****************************************************************************
//SetNoCull関数
//カリングを設定する
//引数：bool
//戻：void
//*****************************************************************************
void Mesh3D::SetNoCull(bool flag)
{
	bNoCull = flag;
}

//*****************************************************************************
//SetUnlit関数
//光を使わないことを設定する
//引数：bool
//戻：void
//*****************************************************************************
void Mesh3D::SetUnlit(bool flag)
{
	bisUnlit = flag;
}


