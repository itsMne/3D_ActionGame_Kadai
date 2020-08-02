// 境界ボックス デバッグ表示 [Box.cpp]
#include "Cube.h"
#include "Shader.h"
#include "Camera3D.h"
#include "Light3D.h"
#include "FbxModel.h"

// 光源情報 (model.cpp内)
//CFbxLight	g_light;		// 光源

// 構造体定義
//----- 頂点座標
struct VERTEX {
	XMFLOAT3	position;
	XMFLOAT3	normal;
};
//----- シェーダに渡す値
struct SHADER_GLOBAL {
	XMMATRIX	mWVP;		// ワールド×ビュー×射影行列(転置行列)
	XMMATRIX	mW;			// ワールド行列(転置行列)
};
struct SHADER_GLOBAL2 {
	XMVECTOR	vEye;		// 視点座標
	// 光源
	XMVECTOR	vLightDir;	// 光源方向
	XMVECTOR	vLa;		// 光源色(アンビエント)
	XMVECTOR	vLd;		// 光源色(ディフューズ)
	XMVECTOR	vLs;		// 光源色(スペキュラ)
	// マテリアル
	XMVECTOR	vAmbient;	// アンビエント色
	XMVECTOR	vDiffuse;	// ディフューズ色
	XMVECTOR	vSpecular;	// スペキュラ色(+スペキュラ強度)
	XMVECTOR	vEmissive;	// エミッシブ色
};

// 静的メンバ
ID3D11Buffer* Cube::m_pConstantBuffer[2] = { nullptr };
ID3D11VertexShader* Cube::m_pVertexShader = nullptr;
ID3D11InputLayout* Cube::m_pInputLayout = nullptr;
ID3D11PixelShader* Cube::m_pPixelShader = nullptr;
ID3D11Buffer* Cube::m_pIndexBuffer = nullptr;
int Cube::m_nRef = 0;

// コンストラクタ
Cube::Cube() : m_pVertexBuffer(nullptr),
m_color(1.0f, 1.0f, 1.0f, 0.5f)
{
	XMStoreFloat4x4(&m_world, XMMatrixIdentity());
}

// デストラクタ
Cube::~Cube()
{
	SAFE_RELEASE(m_pVertexBuffer);
}

// 初期化
HRESULT Cube::Init(XMFLOAT3* pHalfSize)
{
	HRESULT hr = S_OK;
	ID3D11Device* pDevice = GetDevice();

	if (m_nRef == 0) {
		// シェーダ初期化
		static const D3D11_INPUT_ELEMENT_DESC layout[] = {
			{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0, 0,                            D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		};
		hr = LoadShader("Vertex", "Pixel",
			&m_pVertexShader, &m_pInputLayout, &m_pPixelShader, layout, _countof(layout));
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
		hr = pDevice->CreateBuffer(&bd, nullptr, &m_pConstantBuffer[0]);
		if (FAILED(hr)) return hr;
		bd.ByteWidth = sizeof(SHADER_GLOBAL2);
		hr = pDevice->CreateBuffer(&bd, nullptr, &m_pConstantBuffer[1]);
		if (FAILED(hr)) return hr;

		// インデックス バッファ生成
		int index[36] = { 0, 1, 2, 2, 1, 3,
			4, 5, 6, 6, 5, 7,
			8, 9, 10, 10, 9, 11,
			12, 13, 14, 14, 13, 15,
			16, 17, 18, 18, 17, 19,
			20, 21, 22, 22, 21, 23,
		};
		CD3D11_BUFFER_DESC ibd(sizeof(index), D3D11_BIND_INDEX_BUFFER);
		D3D11_SUBRESOURCE_DATA initData;
		ZeroMemory(&initData, sizeof(initData));
		initData.pSysMem = index;
		hr = pDevice->CreateBuffer(&ibd, &initData, &m_pIndexBuffer);
		if (FAILED(hr)) return hr;
	}
	++m_nRef;
	// 頂点バッファ生成
	static XMFLOAT3 pos[24] = {
		{-1,  1, -1}, { 1,  1, -1}, {-1, -1, -1}, { 1, -1, -1},
		{ 1,  1, -1}, { 1,  1,  1}, { 1, -1, -1}, { 1, -1,  1},
		{ 1,  1,  1}, {-1,  1,  1}, { 1, -1,  1}, {-1, -1,  1},
		{-1,  1,  1}, {-1,  1, -1}, {-1, -1,  1}, {-1, -1, -1},
		{-1,  1,  1}, { 1,  1,  1}, {-1,  1, -1}, { 1,  1, -1},
		{-1, -1, -1}, { 1, -1, -1}, {-1, -1,  1}, { 1, -1,  1},
	};
	static XMFLOAT3 dir[6] = {
		{ 0,  0, -1},
		{ 1,  0,  0},
		{ 0,  0,  1},
		{-1,  0,  0},
		{ 0,  1,  0},
		{ 0, -1,  0},
	};
	VERTEX vertex[24];
	for (int i = 0; i < 24; ++i) {
		vertex[i].position.x = pHalfSize->x * pos[i].x;
		vertex[i].position.y = pHalfSize->y * pos[i].y;
		vertex[i].position.z = pHalfSize->z * pos[i].z;
		vertex[i].normal = dir[i / 4];
	}
	// 頂点バッファ生成
	D3D11_BUFFER_DESC vbd;
	ZeroMemory(&vbd, sizeof(vbd));
	vbd.Usage = D3D11_USAGE_DYNAMIC;
	vbd.ByteWidth = sizeof(vertex);
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vbd.MiscFlags = 0;
	D3D11_SUBRESOURCE_DATA initData;
	ZeroMemory(&initData, sizeof(initData));
	initData.pSysMem = vertex;
	hr = pDevice->CreateBuffer(&vbd, &initData, &m_pVertexBuffer);
	if (FAILED(hr)) {
		return hr;
	}
	return hr;
}

// 終了処理
void Cube::Uninit()
{
	// 頂点バッファ解放
	SAFE_RELEASE(m_pVertexBuffer);
	// 参照カウンタ減算
	--m_nRef;
	if (m_nRef == 0) {
		// インデックスバッファ解放
		SAFE_RELEASE(m_pIndexBuffer);
		// 定数バッファの解放
		for (int i = 0; i < _countof(m_pConstantBuffer); ++i) {
			SAFE_RELEASE(m_pConstantBuffer[i]);
		}
		// ピクセルシェーダ解放
		SAFE_RELEASE(m_pPixelShader);
		// 頂点フォーマット解放
		SAFE_RELEASE(m_pInputLayout);
		// 頂点シェーダ解放
		SAFE_RELEASE(m_pVertexShader);
	}
}

// 更新
void Cube::Update()
{

}

// 描画
void Cube::Draw()
{
	// シェーダ設定
	ID3D11DeviceContext* pDeviceContext = GetDeviceContext();
	pDeviceContext->VSSetShader(m_pVertexShader, nullptr, 0);
	pDeviceContext->PSSetShader(m_pPixelShader, nullptr, 0);
	pDeviceContext->IASetInputLayout(m_pInputLayout);

	// プリミティブ形状をセット
	pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// 頂点バッファ設定
	UINT stride = sizeof(VERTEX);
	UINT offset = 0;
	pDeviceContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);
	// インデックスバッファ設定
	pDeviceContext->IASetIndexBuffer(m_pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
	// 定数バッファ設定
	SHADER_GLOBAL cb;
	XMMATRIX mtxWorld = XMLoadFloat4x4(&m_world);
	Camera3D* pCamera = GetMainCamera();
	cb.mWVP = XMMatrixTranspose(mtxWorld *
		XMLoadFloat4x4(&pCamera->GetViewMatrix()) *
		XMLoadFloat4x4(&pCamera->GetProjMatrix()));
	cb.mW = XMMatrixTranspose(mtxWorld);
	pDeviceContext->UpdateSubresource(m_pConstantBuffer[0], 0, nullptr, &cb, 0, 0);
	pDeviceContext->VSSetConstantBuffers(0, 1, &m_pConstantBuffer[0]);
	SHADER_GLOBAL2 cb2;
	cb2.vEye = XMLoadFloat3(&pCamera->GetCameraPos());
	Light3D* pLight = GetMainLight();
	if (!pLight)
		return;
	CFbxLight* light = pLight->GetLight();
	cb2.vLightDir = XMVectorSet(light->m_direction.x, light->m_direction.y, light->m_direction.z, 0.f);
	cb2.vLa = XMLoadFloat4(&light->m_ambient);
	cb2.vLd = XMLoadFloat4(&light->m_diffuse);
	cb2.vLs = XMLoadFloat4(&light->m_specular);
	cb2.vDiffuse = XMLoadFloat4(&m_color);
	cb2.vAmbient = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	cb2.vSpecular = XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f);
	cb2.vEmissive = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	pDeviceContext->UpdateSubresource(m_pConstantBuffer[1], 0, nullptr, &cb2, 0, 0);
	pDeviceContext->PSSetConstantBuffers(1, 1, &m_pConstantBuffer[1]);
	// 描画
	pDeviceContext->DrawIndexed(36, 0, 0);
}
