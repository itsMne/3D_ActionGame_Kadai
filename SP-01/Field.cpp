//*****************************************************************************
// Field3D.cpp
// 床の管理
// Author : Mane
//*****************************************************************************
#include "Field.h"
#include "Shader.h"
#include "Texture.h"
#include "TechnicalCamera.h"
#include "Player3D.h"
#include "UniversalStructures.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
//#define	TEXTURE_FIELD	L"data/texture/field000.jpg"	// 読み込むテクスチャファイル名
#define M_DIFFUSE		XMFLOAT4(1.0f,1.0f,1.0f,1.0f)
#define M_SPECULAR		XMFLOAT4(0.0f,0.0f,0.0f,0.0f)
#define M_AMBIENT		XMFLOAT4(0.0f,0.0f,0.0f,1.0f)
#define M_EMISSIVE		XMFLOAT4(0.0f,0.0f,0.0f,0.0f)
#define COL_CHECK_PRECISION 15.0f
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
	XMVECTOR	vEmissive;	// エミッシブ色
};

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
static HRESULT MakeVertexField(ID3D11Device* pDevice);

//*****************************************************************************
// コンストラクタ関数
//*****************************************************************************
Field3D::Field3D() : Object3D(GO_FLOOR)
{
	pSceneLight = nullptr;
	nTextureSubDivisions = 1;
}

Field3D::Field3D(const char * TexturePath) : Object3D(GO_FLOOR)
{
	pSceneLight = GetMainLight();
	nTextureSubDivisions = 1;
	Init(pSceneLight, TexturePath);
}


Field3D::~Field3D()
{
	End();
}

//*****************************************************************************
//Init関数
//初期化関数
//引数：void
//戻：void
//*****************************************************************************
HRESULT Field3D::Init(Light3D* SceneLight, const char* TexturePath)
{
	strcpy(szTexturePath, TexturePath);
	SetTextureSubdivisions(10.0f / Scale.x);
	SetFieldLight(SceneLight);
	ID3D11Device* pDevice = GetDevice();
	HRESULT hr;
	//nType = GO_FLOOR;
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
		return hr;
	}

	// 定数バッファ生成
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(SHADER_GLOBAL);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;
	hr = pDevice->CreateBuffer(&bd, nullptr, &g_pConstantBuffer[0]);
	if (FAILED(hr)) return hr;
	bd.ByteWidth = sizeof(SHADER_GLOBAL2);
	hr = pDevice->CreateBuffer(&bd, nullptr, &g_pConstantBuffer[1]);
	if (FAILED(hr)) return hr;

	// テクスチャ サンプラ生成
	D3D11_SAMPLER_DESC sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sd.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sd.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sd.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	hr = pDevice->CreateSamplerState(&sd, &g_pSamplerState);
	if (FAILED(hr)) {
		return hr;
	}

	XMStoreFloat4x4(&g_mtxTexture, XMMatrixIdentity());

	// 位置、向きの初期設定
	Position = XMFLOAT3(0.0f, 0.0f, 0.0f);
	Rotation = XMFLOAT3(0.0f, 0.0f, 0.0f);
	Scale = { 1,1,1 };
	// マテリアルの初期設定
	g_Kd = M_DIFFUSE;
	g_Ka = M_AMBIENT;
	g_Ks = M_SPECULAR;
	g_fPower = 0.0f;
	g_Ke = M_EMISSIVE;

	// テクスチャの読み込み
	hr = CreateTextureFromFile(pDevice,			// デバイスへのポインタ
		TexturePath,	// ファイルの名前
		&g_pTexture);	// 読み込むメモリー
	if (FAILED(hr)) {
		printf("FAILED TEXTURE LOAD\n");
		return hr;
	}

	// 頂点情報の作成
	hr = MakeVertexField(pDevice);
	return hr;
}

//*****************************************************************************
//End関数
//終了関数
//引数：void
//戻：void
//*****************************************************************************
void Field3D::End(void)
{
	// テクスチャ解放
	SAFE_RELEASE(g_pTexture);
	// テクスチャ サンプラの開放
	SAFE_RELEASE(g_pSamplerState);
	// 頂点バッファの解放
	SAFE_RELEASE(g_pVertexBuffer);
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
//Update関数
//変更関数
//引数：void
//戻：void
//*****************************************************************************
void Field3D::Update(void)
{
	Object3D::Update();
	Hitbox = { 0, 0 - 1.5f, 0, Scale.x, 1.5f, Scale.z };
	Player3D* pPlayer = GetPlayer();
	if (pPlayer)
	{
		if (!pPlayer->IsOnTheFloor()) {
			if (pPlayer->GetGravityForce() < 0)
				return;
			pPlayer->TranslateY(pPlayer->GetGravityForce());
			float fCurrentGravityForce = pPlayer->GetGravityForce();
			fCurrentGravityForce /= COL_CHECK_PRECISION;
		
			for (int i = 0; i < (int)COL_CHECK_PRECISION; i++)
			{
				pPlayer->TranslateY(-fCurrentGravityForce);
				if (IsInCollision3D(pPlayer->GetHitboxPlayer(PLAYER_HB_FEET), GetHitbox()))
					break;
				if(i== (int)COL_CHECK_PRECISION-1)
					if (!IsInCollision3D(pPlayer->GetHitboxPlayer(PLAYER_HB_FEET), GetHitbox())) 
						return;
			}
			pPlayer->TranslateY(pPlayer->GetGravityForce()*10);
			while (!IsInCollision3D(pPlayer->GetHitboxPlayer(PLAYER_HB_FEET), GetHitbox())) //{
				pPlayer->TranslateY(-0.01f);
			pPlayer->SetFloor(this);
		}
	}
	SetTextureSubdivisions(10.0f / Scale.x);
}

//*****************************************************************************
//Draw関数
//レンダリング関数
//引数：void
//戻：void
//*****************************************************************************
void Field3D::Draw(void)
{
	Object3D::Draw();
	GetDeviceContext()->RSSetState(GetMainWindow()->GetRasterizerState(2));
	TechCamera* pMainCamera = GetMainCamera();
	if (!pMainCamera)
	{
		printf("メインカメラがありません\n");
		return;
	}
	ID3D11DeviceContext* pDeviceContext = GetDeviceContext();
	XMMATRIX mtxWorld, mtxRot, mtxTranslate, mtxScale;

	// ワールドマトリックスの初期化
	mtxWorld = XMMatrixIdentity();

	//大きさ
	mtxScale = XMMatrixScaling(Scale.x, Scale.y, Scale.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxScale);
	// 回転を反映
	mtxRot = XMMatrixRotationRollPitchYaw(Rotation.x, Rotation.y, Rotation.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

	// 移動を反映
	mtxTranslate = XMMatrixTranslation(Position.x, Position.y, Position.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

	// ワールドマトリックスの設定
	XMStoreFloat4x4(&g_mtxWorldField, mtxWorld);

	pDeviceContext->VSSetShader(g_pVertexShader, nullptr, 0);
	pDeviceContext->PSSetShader(g_pPixelShader, nullptr, 0);
	pDeviceContext->IASetInputLayout(g_pInputLayout);

	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	pDeviceContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer, &stride, &offset);

	pDeviceContext->PSSetSamplers(0, 1, &g_pSamplerState);
	pDeviceContext->PSSetShaderResources(0, 1, &g_pTexture);

	SHADER_GLOBAL cb;

	cb.mWVP = XMMatrixTranspose(mtxWorld *
		XMLoadFloat4x4(&(pMainCamera->GetViewMatrix())) * XMLoadFloat4x4(&(pMainCamera->GetProjMatrix())));
	cb.mW = XMMatrixTranspose(mtxWorld);
	cb.mTex = XMMatrixTranspose(XMLoadFloat4x4(&g_mtxTexture));
	pDeviceContext->UpdateSubresource(g_pConstantBuffer[0], 0, nullptr, &cb, 0, 0);
	pDeviceContext->VSSetConstantBuffers(0, 1, &g_pConstantBuffer[0]);
	SHADER_GLOBAL2 cb2;
	cb2.vEye = XMLoadFloat3(&(pMainCamera->GetCameraPos()));
	CFbxLight& light = *pSceneLight->GetLight();
	if (pSceneLight)
	{
		if (bIsUnlit || NO_LIGHT_DEFAULT)
			pSceneLight->SetLightEnable(false);
		cb2.vLightDir = XMVectorSet(light.m_direction.x, light.m_direction.y, light.m_direction.z, 0.f);
		cb2.vLa = XMLoadFloat4(&light.m_ambient);
		cb2.vLd = XMLoadFloat4(&light.m_diffuse);
		cb2.vLs = XMLoadFloat4(&light.m_specular);
	}
	else
		printf("光がありません\n");

	cb2.vDiffuse = XMLoadFloat4(&g_Kd);
	cb2.vAmbient = XMVectorSet(g_Ka.x, g_Ka.y, g_Ka.z,
		(g_pTexture != nullptr) ? 1.f : 0.f);
	cb2.vSpecular = XMVectorSet(g_Ks.x, g_Ks.y, g_Ks.z, g_fPower);
	cb2.vEmissive = XMLoadFloat4(&g_Ke);
	pDeviceContext->UpdateSubresource(g_pConstantBuffer[1], 0, nullptr, &cb2, 0, 0);
	pDeviceContext->PSSetConstantBuffers(1, 1, &g_pConstantBuffer[1]);

	// プリミティブ形状をセット
	pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	// ポリゴンの描画
	//pDeviceContext->DrawIndexed(0, 0, 4);
	pSceneLight->SetLightEnable(true);
	pDeviceContext->Draw(NUM_VERTEX, 0);
}

//*****************************************************************************
//SetFieldLight関数
//床の光を設定する
//引数：Light3D*
//戻：void
//*****************************************************************************
void Field3D::SetFieldLight(Light3D * SceneLight)
{
	pSceneLight = SceneLight;
}

//*****************************************************************************
//MakeVertexField関数
//頂点の作成
//引数：ID3D11Device*
//戻：HRESULT
//*****************************************************************************
HRESULT Field3D::MakeVertexField(ID3D11Device* pDevice)
{
	// 頂点座標の設定
	g_vertexWk[0].vtx = XMFLOAT3(-1.0f, 0.0f, 1.0f);
	g_vertexWk[1].vtx = XMFLOAT3(1.0f, 0.0f, 1.0f);
	g_vertexWk[2].vtx = XMFLOAT3(-1.0f, 0.0f, -1.0f);
	g_vertexWk[3].vtx = XMFLOAT3(1.0f, 0.0f, -1.0f);

	// 法線ベクトルの設定
	g_vertexWk[0].nor = XMFLOAT3(0.0f, 1.0f, 0.0f);
	g_vertexWk[1].nor = XMFLOAT3(0.0f, 1.0f, 0.0f);
	g_vertexWk[2].nor = XMFLOAT3(0.0f, 1.0f, 0.0f);
	g_vertexWk[3].nor = XMFLOAT3(0.0f, 1.0f, 0.0f);

	// 拡散反射光の設定
	g_vertexWk[0].diffuse = XMFLOAT4(0.9f, 0.9f, 0.9f, 1.0f);
	g_vertexWk[1].diffuse = XMFLOAT4(0.9f, 0.9f, 0.9f, 1.0f);
	g_vertexWk[2].diffuse = XMFLOAT4(0.9f, 0.9f, 0.9f, 1.0f);
	g_vertexWk[3].diffuse = XMFLOAT4(0.9f, 0.9f, 0.9f, 1.0f);

	// テクスチャ座標の設定
	g_vertexWk[0].tex = XMFLOAT2(0.0f*nTextureSubDivisions, 0.0f*nTextureSubDivisions);
	g_vertexWk[1].tex = XMFLOAT2(1.0f*nTextureSubDivisions, 0.0f*nTextureSubDivisions);
	g_vertexWk[2].tex = XMFLOAT2(0.0f*nTextureSubDivisions, 1.0f*nTextureSubDivisions);
	g_vertexWk[3].tex = XMFLOAT2(1.0f*nTextureSubDivisions, 1.0f*nTextureSubDivisions);

	D3D11_BUFFER_DESC vbd;
	ZeroMemory(&vbd, sizeof(vbd));
	vbd.Usage = D3D11_USAGE_DYNAMIC;
	vbd.ByteWidth = sizeof(g_vertexWk);
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vbd.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA initData;
	ZeroMemory(&initData, sizeof(initData));
	initData.pSysMem = &g_vertexWk[0];

	HRESULT hr = pDevice->CreateBuffer(&vbd, &initData, &g_pVertexBuffer);

	return hr;
}

//*****************************************************************************
//SetPosition関数
//拠点を設定する
//引数：XMFLOAT3
//戻：void
//*****************************************************************************
void Field3D::SetPosition(XMFLOAT3 newPos)
{
	Position = newPos;
}

//*****************************************************************************
//SetRotation関数
//回転を設定する
//引数：XMFLOAT3
//戻：void
//*****************************************************************************
void Field3D::SetRotation(XMFLOAT3 newRot)
{
	Rotation = newRot;
}

//*****************************************************************************
//SetScale関数
//大きさを設定する
//引数：float
//戻：void
//*****************************************************************************
void Field3D::SetScale(float newScale)
{
	Scale = { newScale ,newScale ,newScale };
}

//*****************************************************************************
//SetScale関数
//大きさを設定する
//引数：XMFLOAT3
//戻：void
//*****************************************************************************
void Field3D::SetScale(XMFLOAT3 newScale)
{
	Scale = newScale;
}

//*****************************************************************************
//SetTextureSubdivisions関数
//テクスチャを設定する
//引数：int
//戻：void
//*****************************************************************************
void Field3D::SetTextureSubdivisions(int newSubs)
{
	nTextureSubDivisions = newSubs;
}

//*****************************************************************************
//GetTexturePath関数
//テクスチャのパスを戻す
//引数：void
//戻：char*
//*****************************************************************************
char * Field3D::GetTexturePath()
{
	return szTexturePath;
}


