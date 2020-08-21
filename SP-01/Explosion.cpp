// �����G�t�F�N�g �N���X [Explosion.cpp]
#include "Explosion.h"
#include "Shader.h"
#include "TechnicalCamera.h"
#include "CameraObj.h"


// �\���̒�`
//----- ���_���W
struct VERTEX {
	XMFLOAT3	position;
	XMFLOAT3	normal;
};
//----- �V�F�[�_�ɓn���l
struct SHADER_GLOBAL {
	XMMATRIX	mWVP;		// ���[���h�~�r���[�~�ˉe�s��(�]�u�s��)
	XMMATRIX	mW;			// ���[���h�s��(�]�u�s��)
};
struct SHADER_GLOBAL2 {
	XMVECTOR	vEye;		// ���_���W
	// ����
	XMVECTOR	vLightDir;	// ��������
	XMVECTOR	vLa;		// �����F(�A���r�G���g)
	XMVECTOR	vLd;		// �����F(�f�B�t���[�Y)
	XMVECTOR	vLs;		// �����F(�X�y�L����)
	// �}�e���A��
	XMVECTOR	vAmbient;	// �A���r�G���g�F
	XMVECTOR	vDiffuse;	// �f�B�t���[�Y�F
	XMVECTOR	vSpecular;	// �X�y�L�����F(+�X�y�L�������x)
	XMVECTOR	vEmissive;	// �G�~�b�V�u�F
};

// �ÓI�����o�ϐ�
ID3D11Buffer* CExplosion::m_pConstantBuffer[2] = {nullptr, nullptr};
ID3D11VertexShader* CExplosion::m_pVertexShader = nullptr;
ID3D11InputLayout* CExplosion::m_pInputLayout = nullptr;
ID3D11PixelShader* CExplosion::m_pPixelShader = nullptr;
int CExplosion::m_nRef = 0;	// �Q�ƃJ�E���^

//-----------------------------------------------
// �j��
//-----------------------------------------------
// �R���X�g���N�^
CDebris::CDebris(CExplosion* pExp) : m_pParent(pExp), m_pVertexBuffer(nullptr)
{
	// ���X�g�ɒǉ� (��ɐ擪�ɑ}��)
	CDebris* pTop = GetTop();
	m_pNext = pTop;
	if (pTop)
		pTop->m_pBack = this;
	m_pBack = nullptr;
	SetTop(this);
	Scale = { 1,1,1 };
}

// �f�X�g���N�^
CDebris::~CDebris()
{
	// ���X�g����폜
	if (m_pBack)
		m_pBack->m_pNext = m_pNext;
	else
		SetTop(m_pNext);
	if (m_pNext)
		m_pNext->m_pBack = m_pBack;
}

// ���X�g�擪���擾
CDebris* CDebris::GetTop()
{
	if (m_pParent)
		return m_pParent->GetTop();
	return nullptr;
}

// ���X�g�擪��ݒ�
void CDebris::SetTop(CDebris* pTop)
{
	if (m_pParent)
		m_pParent->SetTop(pTop);
}

// ������
HRESULT CDebris::Init(XMFLOAT3& v0, XMFLOAT3& v1, XMFLOAT3& v2,
	float fRadius, XMFLOAT4X4* pWorld, XMFLOAT3* pVelocity)
{
	// ���[���h��Ԃɔz�u
	XMMATRIX world = XMLoadFloat4x4(pWorld);
	XMFLOAT3 v[3];
	XMStoreFloat3(&v[0], XMVector3TransformCoord(XMLoadFloat3(&v0), world));
	XMStoreFloat3(&v[1], XMVector3TransformCoord(XMLoadFloat3(&v1), world));
	XMStoreFloat3(&v[2], XMVector3TransformCoord(XMLoadFloat3(&v2), world));
	// 3���_�̒��S���W(=���f����Ԍ��_�̃��[���h���W)
	XMFLOAT3 c = {
		(v[0].x + v[1].x + v[2].x) / 3.0f,
		(v[0].y + v[1].y + v[2].y) / 3.0f,
		(v[0].z + v[1].z + v[2].z) / 3.0f
	};
	// ���_���W�����f�����W�n�ɕϊ�
	for (int i = 0; i < 3; ++i) {
		v[i].x -= c.x;
		v[i].y -= c.y;
		v[i].z -= c.z;
	}
	// �O�p�`��1��
	XMVECTOR edge = XMVectorSet(v[1].x - v[0].x, v[1].y - v[0].y, v[1].z - v[0].z, 0.0f);
	// ����1�ӂƊO�ς��Ƃ�A�@���x�N�g���Ƃ��Đ��K��
	XMVECTOR n = XMVector3Normalize(XMVector3Cross(edge,
		XMVectorSet(v[2].x - v[1].x, v[2].y - v[1].y, v[2].z - v[1].z, 0.0f)));
	XMFLOAT3 normal;
	XMStoreFloat3(&normal, n);
	// ���_�o�b�t�@����
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
	// �s�����x
	m_alpha = 1.0f;
	// ��]������
	XMMATRIX rotate = XMMatrixRotationAxis(n, XMConvertToRadians(float(rand() % 360)));
	XMStoreFloat3(&m_axis, XMVector3TransformNormal(XMVector3Normalize(edge), rotate));
	// ��]���x (�P��:�x�A1�`2�b��1��])
	m_rotate = (180 + rand() % 180) / 60.0f;
	// ���x
	m_velocity.x = normal.x * fRadius * 0.02f;
	m_velocity.y = normal.y * fRadius * 0.02f;
	m_velocity.z = normal.z * fRadius * 0.02f;
	if (pVelocity) {
		m_velocity.x += pVelocity->x;
		m_velocity.y += pVelocity->y;
		m_velocity.z += pVelocity->z;
	}
	// �����x
	m_accel = XMFLOAT3(0.0f, -0.05f, 0.0f);
	// ���[���h�}�g���b�N�X
	XMStoreFloat4x4(&m_world, XMMatrixTranslation(c.x, c.y, c.z));

	return hr;
}

// �I������
void CDebris::Uninit()
{
	// ���_�o�b�t�@���
	SAFE_RELEASE(m_pVertexBuffer);
	// �C���X�^���X���̂��폜
	delete this;
}

// �X�V
void CDebris::Update()
{
	// �s�����x�X�V
	m_alpha -= (1.0f / 180.0f);	// 3�b�Ԑ���
	if (m_alpha < 1.0f / 255.0f) {
		Uninit();
		return;
	}
	// ���x���X�V
	m_velocity.x += m_accel.x;
	m_velocity.y += m_accel.y;
	m_velocity.z += m_accel.z;
	// ���݈ʒu��ۑ�&�N���A
	XMFLOAT3 pos = { m_world._41, m_world._42, m_world._43 };
	m_world._41 = m_world._42 = m_world._43 = 0.0f;
	// ���W���X�V
	pos.x += m_velocity.x*Scale.x;
	pos.y += m_velocity.y*Scale.y;
	pos.z += m_velocity.z*Scale.z;
	// �X�P�[���𔽉f
	m_world._11 = Scale.x;
	m_world._22 = Scale.y;
	m_world._33 = Scale.z;
	// ��]
	XMStoreFloat4x4(&m_world, XMMatrixMultiply(XMLoadFloat4x4(&m_world),
		XMMatrixRotationAxis(XMLoadFloat3(&m_axis), XMConvertToRadians(m_rotate))));
	// ���W�𔽉f
	m_world._41 = pos.x;
	m_world._42 = pos.y;
	m_world._43 = pos.z;
}

// �`��
void CDebris::Draw()
{
	// ���_�o�b�t�@�ݒ�
	ID3D11DeviceContext* pDeviceContext = GetDeviceContext();
	UINT stride = sizeof(VERTEX);
	UINT offset = 0;
	pDeviceContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);
	// �萔�o�b�t�@�ݒ�
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
	// �`��
	pDeviceContext->Draw(3, 0);
}

void CDebris::SetScale(XMFLOAT3 xScale)
{
	Scale = xScale;
}

//-----------------------------------------------
// �����G�t�F�N�g
//-----------------------------------------------
// �R���X�g���N�^
CExplosion::CExplosion() : m_pDebris(nullptr)
{
}

// �f�X�g���N�^
CExplosion::~CExplosion()
{
}

// ������
HRESULT CExplosion::Init()
{
	Scale = { 1,1,1 };
	HRESULT hr = S_OK;
	if (m_nRef == 0) {
		ID3D11Device* pDevice = GetDevice();

		// �V�F�[�_������
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
		
		// �萔�o�b�t�@����
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

// �I������
void CExplosion::Uninit()
{
	// �S�Ă̔j�Ђ��I��
	CDebris* pDeb;
	CDebris* pNext;
	for (pDeb = GetTop(); pDeb; pDeb = pNext) {
		pNext = pDeb->GetNext();
		pDeb->Uninit();
	}
	// �Q�ƃJ�E���^���Z
	--m_nRef;
	if (m_nRef == 0) {
		// �萔�o�b�t�@�̉��
		for (int i = 0; i < _countof(m_pConstantBuffer); ++i) {
			SAFE_RELEASE(m_pConstantBuffer[i]);
		}
		// �s�N�Z���V�F�[�_���
		SAFE_RELEASE(m_pPixelShader);
		// ���_�t�H�[�}�b�g���
		SAFE_RELEASE(m_pInputLayout);
		// ���_�V�F�[�_���
		SAFE_RELEASE(m_pVertexShader);
	}
}

// �X�V
void CExplosion::Update()
{
	// �S�Ă̔j�Ђ��X�V
	CDebris* pDeb;
	CDebris* pNext;
	for (pDeb = GetTop(); pDeb; pDeb = pNext) {
		pNext = pDeb->GetNext();// �폜�ɔ����Ď擾���Ă���.
		pDeb->Update();
	}
}

// �`��
void CExplosion::Draw()
{
	ID3D11DeviceContext* pDeviceContext = GetDeviceContext();

	// �V�F�[�_�ݒ�
	pDeviceContext->VSSetShader(m_pVertexShader, nullptr, 0);
	pDeviceContext->PSSetShader(m_pPixelShader, nullptr, 0);
	pDeviceContext->IASetInputLayout(m_pInputLayout);

	// �v���~�e�B�u�`����Z�b�g
	pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// �S�Ă̔j�Ђ�`��
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

// �����J�n
HRESULT CExplosion::Start(CFbxModel* pModel, XMFLOAT4X4* pWorld, XMFLOAT3* pVelocity)
{
	HRESULT hr = S_OK;

	if (!pModel) return E_FAIL;
	// ���_�z��A�C���f�b�N�X�z��擾
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
	// ���E�����a�擾
	float fRadius = pModel->GetRadius();
	// �j�Ђ̐���
	for (int i = 0; i < nIndex; i += 3) {
		XMFLOAT3& v0 = pVertex[pIndex[i + 2]].vPos;
		XMFLOAT3& v1 = pVertex[pIndex[i + 1]].vPos;
		XMFLOAT3& v2 = pVertex[pIndex[i + 0]].vPos;
		CDebris* pDeb = new CDebris(this);
		hr = pDeb->Init(v0, v1, v2, fRadius, pWorld, pVelocity);
		if (FAILED(hr))
			break;
	}
	// ���_�z��A�C���f�b�N�X�z����
	delete[] pIndex;
	delete[] pVertex;
	return hr;
}

// �V�F�[�_�̒萔�o�b�t�@���X�V
void CExplosion::UpdateBuffer(int nShader, void* pData)
{
	ID3D11DeviceContext* pDeviceContext = GetDeviceContext();
	if (nShader) {
		// �s�N�Z���V�F�[�_
		pDeviceContext->UpdateSubresource(m_pConstantBuffer[1], 0, nullptr, pData, 0, 0);
		pDeviceContext->PSSetConstantBuffers(1, 1, &m_pConstantBuffer[1]);
	} else {
		// ���_�V�F�[�_
		pDeviceContext->UpdateSubresource(m_pConstantBuffer[0], 0, nullptr, pData, 0, 0);
		pDeviceContext->VSSetConstantBuffers(0, 1, &m_pConstantBuffer[0]);
	}
}
