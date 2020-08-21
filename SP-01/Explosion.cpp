// 爆発エフェクト クラス [Explosion.cpp]
#include "Explosion.h"
#include "Shader.h"
#include "TechnicalCamera.h"
#include "CameraObj.h"


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

// 静的メンバ変数
ID3D11Buffer* CExplosion::m_pConstantBuffer[2] = {nullptr, nullptr};
ID3D11VertexShader* CExplosion::m_pVertexShader = nullptr;
ID3D11InputLayout* CExplosion::m_pInputLayout = nullptr;
ID3D11PixelShader* CExplosion::m_pPixelShader = nullptr;
int CExplosion::m_nRef = 0;	// 参照カウンタ

//-----------------------------------------------
// 破片
//-----------------------------------------------
// コンストラクタ
CDebris::CDebris(CExplosion* pExp) : m_pParent(pExp), m_pVertexBuffer(nullptr)
{
	// リストに追加 (常に先頭に挿入)
	CDebris* pTop = GetTop();
	m_pNext = pTop;
	if (pTop)
		pTop->m_pBack = this;
	m_pBack = nullptr;
	SetTop(this);
	Scale = { 1,1,1 };
}

// デストラクタ
CDebris::~CDebris()
{
	// リストから削除
	if (m_pBack)
		m_pBack->m_pNext = m_pNext;
	else
		SetTop(m_pNext);
	if (m_pNext)
		m_pNext->m_pBack = m_pBack;
}

// リスト先頭を取得
CDebris* CDebris::GetTop()
{
	if (m_pParent)
		return m_pParent->GetTop();
	return nullptr;
}

// リスト先頭を設定
void CDebris::SetTop(CDebris* pTop)
{
	if (m_pParent)
		m_pParent->SetTop(pTop);
}

// 初期化
HRESULT CDebris::Init(XMFLOAT3& v0, XMFLOAT3& v1, XMFLOAT3& v2,
	float fRadius, XMFLOAT4X4* pWorld, XMFLOAT3* pVelocity)
{
	// ワールド空間に配置
	XMMATRIX world = XMLoadFloat4x4(pWorld);
	XMFLOAT3 v[3];
	XMStoreFloat3(&v[0], XMVector3TransformCoord(XMLoadFloat3(&v0), world));
	XMStoreFloat3(&v[1], XMVector3TransformCoord(XMLoadFloat3(&v1), world));
	XMStoreFloat3(&v[2], XMVector3TransformCoord(XMLoadFloat3(&v2), world));
	// 3頂点の中心座標(=モデル空間原点のワールド座標)
	XMFLOAT3 c = {
		(v[0].x + v[1].x + v[2].x) / 3.0f,
		(v[0].y + v[1].y + v[2].y) / 3.0f,
		(v[0].z + v[1].z + v[2].z) / 3.0f
	};
	// 頂点座標をモデル座標系に変換
	for (int i = 0; i < 3; ++i) {
		v[i].x -= c.x;
		v[i].y -= c.y;
		v[i].z -= c.z;
	}
	// 三角形の1辺
	XMVECTOR edge = XMVectorSet(v[1].x - v[0].x, v[1].y - v[0].y, v[1].z - v[0].z, 0.0f);
	// もう1辺と外積をとり、法線ベクトルとして正規化
	XMVECTOR n = XMVector3Normalize(XMVector3Cross(edge,
		XMVectorSet(v[2].x - v[1].x, v[2].y - v[1].y, v[2].z - v[1].z, 0.0f)));
	XMFLOAT3 normal;
	XMStoreFloat3(&normal, n);
	// 頂点バッファ生成
	D3D11_BUFFER_DESC vbd;
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = sizeof(VERTEX) * 3;
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	vbd.MiscFlags = 0;
	D3D11_SUBRESOURCE_DATA initData;
	VERTEX vertex[3];
	for (int i = 0; i < 3; ++i) {
		vertex[i].position = v[i];
		vertex[i].normal = normal;
	}
	initData.pSysMem = &vertex[0];
	ID3D11Device* pDevice = GetDevice();
	HRESULT hr = pDevice->CreateBuffer(&vbd, &initData, &m_pVertexBuffer);
	if (FAILED(hr)) return hr;
	// 不透明度
	m_alpha = 1.0f;
	// 回転軸生成
	XMMATRIX rotate = XMMatrixRotationAxis(n, XMConvertToRadians(float(rand() % 360)));
	XMStoreFloat3(&m_axis, XMVector3TransformNormal(XMVector3Normalize(edge), rotate));
	// 回転速度 (単位:度、1～2秒で1回転)
	m_rotate = (180 + rand() % 180) / 60.0f;
	// 速度
	m_velocity.x = normal.x * fRadius * 0.02f;
	m_velocity.y = normal.y * fRadius * 0.02f;
	m_velocity.z = normal.z * fRadius * 0.02f;
	if (pVelocity) {
		m_velocity.x += pVelocity->x;
		m_velocity.y += pVelocity->y;
		m_velocity.z += pVelocity->z;
	}
	// 加速度
	m_accel = XMFLOAT3(0.0f, -0.05f, 0.0f);
	// ワールドマトリックス
	XMStoreFloat4x4(&m_world, XMMatrixTranslation(c.x, c.y, c.z));

	return hr;
}

// 終了処理
void CDebris::Uninit()
{
	// 頂点バッファ解放
	SAFE_RELEASE(m_pVertexBuffer);
	// インスタンス自体を削除
	delete this;
}

// 更新
void CDebris::Update()
{
	// 不透明度更新
	m_alpha -= (1.0f / 180.0f);	// 3秒間生存
	if (m_alpha < 1.0f / 255.0f) {
		Uninit();
		return;
	}
	// 速度を更新
	m_velocity.x += m_accel.x;
	m_velocity.y += m_accel.y;
	m_velocity.z += m_accel.z;
	// 現在位置を保存&クリア
	XMFLOAT3 pos = { m_world._41, m_world._42, m_world._43 };
	m_world._41 = m_world._42 = m_world._43 = 0.0f;
	// 座標を更新
	pos.x += m_velocity.x*Scale.x;
	pos.y += m_velocity.y*Scale.y;
	pos.z += m_velocity.z*Scale.z;
	// スケールを反映
	m_world._11 = Scale.x;
	m_world._22 = Scale.y;
	m_world._33 = Scale.z;
	// 回転
	XMStoreFloat4x4(&m_world, XMMatrixMultiply(XMLoadFloat4x4(&m_world),
		XMMatrixRotationAxis(XMLoadFloat3(&m_axis), XMConvertToRadians(m_rotate))));
	// 座標を反映
	m_world._41 = pos.x;
	m_world._42 = pos.y;
	m_world._43 = pos.z;
}

// 描画
void CDebris::Draw()
{
	// 頂点バッファ設定
	ID3D11DeviceContext* pDeviceContext = GetDeviceContext();
	UINT stride = sizeof(VERTEX);
	UINT offset = 0;
	pDeviceContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);
	// 定数バッファ設定
	SHADER_GLOBAL cb;
	XMMATRIX mtxWorld = XMLoadFloat4x4(&m_world);
	TechCamera* pCamera = GetMainCamera();
	cb.mWVP = XMMatrixTranspose(mtxWorld *
		XMLoadFloat4x4(&pCamera->GetView()) *
		XMLoadFloat4x4(&pCamera->GetProj()));
	cb.mW = XMMatrixTranspose(mtxWorld);
	m_pParent->UpdateBuffer(0, &cb);
	SHADER_GLOBAL2 cb2;
	cb2.vEye = XMLoadFloat3(&pCamera->GetEye());
	CFbxLight* light = GetMainLight()->GetLight();
	cb2.vLightDir = XMVectorSet(light->m_direction.x, light->m_direction.y, light->m_direction.z, 0.f);
	cb2.vLa = XMLoadFloat4(&light->m_ambient);
	cb2.vLd = XMLoadFloat4(&light->m_diffuse);
	cb2.vLs = XMLoadFloat4(&light->m_specular);
	cb2.vDiffuse = XMVectorSet(1.0f, 0.0f, 0.0f, m_alpha);
	cb2.vAmbient = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	cb2.vSpecular = XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f);
	cb2.vEmissive = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	m_pParent->UpdateBuffer(1, &cb2);
	// 描画
	pDeviceContext->Draw(3, 0);
}

void CDebris::SetScale(XMFLOAT3 xScale)
{
	Scale = xScale;
}

//-----------------------------------------------
// 爆発エフェクト
//-----------------------------------------------
// コンストラクタ
CExplosion::CExplosion() : m_pDebris(nullptr)
{
}

// デストラクタ
CExplosion::~CExplosion()
{
}

// 初期化
HRESULT CExplosion::Init()
{
	Scale = { 1,1,1 };
	HRESULT hr = S_OK;
	if (m_nRef == 0) {
		ID3D11Device* pDevice = GetDevice();

		// シェーダ初期化
		static const D3D11_INPUT_ELEMENT_DESC layout[] = {
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0, 0,                            D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,       0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
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
	}
	++m_nRef;
	return hr;
}

// 終了処理
void CExplosion::Uninit()
{
	// 全ての破片を終了
	CDebris* pDeb;
	CDebris* pNext;
	for (pDeb = GetTop(); pDeb; pDeb = pNext) {
		pNext = pDeb->GetNext();
		pDeb->Uninit();
	}
	// 参照カウンタ減算
	--m_nRef;
	if (m_nRef == 0) {
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
void CExplosion::Update()
{
	// 全ての破片を更新
	CDebris* pDeb;
	CDebris* pNext;
	for (pDeb = GetTop(); pDeb; pDeb = pNext) {
		pNext = pDeb->GetNext();// 削除に備えて取得しておく.
		pDeb->Update();
	}
}

// 描画
void CExplosion::Draw()
{
	ID3D11DeviceContext* pDeviceContext = GetDeviceContext();

	// シェーダ設定
	pDeviceContext->VSSetShader(m_pVertexShader, nullptr, 0);
	pDeviceContext->PSSetShader(m_pPixelShader, nullptr, 0);
	pDeviceContext->IASetInputLayout(m_pInputLayout);

	// プリミティブ形状をセット
	pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// 全ての破片を描画
	CDebris* pDeb;
	for (pDeb = GetTop(); pDeb; pDeb = pDeb->GetNext()) {
		pDeb->SetScale(Scale);
		pDeb->Draw();
	}
}

void CExplosion::SetScale(XMFLOAT3 xScale)
{
	Scale = xScale;
}

// 爆発開始
HRESULT CExplosion::Start(CFbxModel* pModel, XMFLOAT4X4* pWorld, XMFLOAT3* pVelocity)
{
	HRESULT hr = S_OK;

	if (!pModel) return E_FAIL;
	// 頂点配列、インデックス配列取得
	int nVertex = pModel->GetVertexCount();
	int nIndex = pModel->GetIndexCount();
	if (nVertex <= 0 || nIndex <= 0) return E_FAIL;
	TFbxVertex* pVertex = new TFbxVertex[nVertex];
	int* pIndex = new int[nIndex];
	if (pModel->GetVertex(pVertex, nVertex) != nVertex
	 || pModel->GetIndex(pIndex, nIndex) != nIndex) {
		delete[] pIndex;
		delete[] pVertex;
		return E_FAIL;
	}
	// 境界球半径取得
	float fRadius = pModel->GetRadius();
	// 破片の生成
	for (int i = 0; i < nIndex; i += 3) {
		XMFLOAT3& v0 = pVertex[pIndex[i + 2]].vPos;
		XMFLOAT3& v1 = pVertex[pIndex[i + 1]].vPos;
		XMFLOAT3& v2 = pVertex[pIndex[i + 0]].vPos;
		CDebris* pDeb = new CDebris(this);
		hr = pDeb->Init(v0, v1, v2, fRadius, pWorld, pVelocity);
		if (FAILED(hr))
			break;
	}
	// 頂点配列、インデックス配列解放
	delete[] pIndex;
	delete[] pVertex;
	return hr;
}

// シェーダの定数バッファを更新
void CExplosion::UpdateBuffer(int nShader, void* pData)
{
	ID3D11DeviceContext* pDeviceContext = GetDeviceContext();
	if (nShader) {
		// ピクセルシェーダ
		pDeviceContext->UpdateSubresource(m_pConstantBuffer[1], 0, nullptr, pData, 0, 0);
		pDeviceContext->PSSetConstantBuffers(1, 1, &m_pConstantBuffer[1]);
	} else {
		// 頂点シェーダ
		pDeviceContext->UpdateSubresource(m_pConstantBuffer[0], 0, nullptr, pData, 0, 0);
		pDeviceContext->VSSetConstantBuffers(0, 1, &m_pConstantBuffer[0]);
	}
}
