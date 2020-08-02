// ���E�{�b�N�X �f�o�b�O�\�� [Box.cpp]
#include "Cube.h"
#include "Shader.h"
#include "Camera3D.h"
#include "Light3D.h"
#include "FbxModel.h"

// ������� (model.cpp��)
//CFbxLight	g_light;		// ����

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

// �ÓI�����o
ID3D11Buffer* Cube::m_pConstantBuffer[2] = { nullptr };
ID3D11VertexShader* Cube::m_pVertexShader = nullptr;
ID3D11InputLayout* Cube::m_pInputLayout = nullptr;
ID3D11PixelShader* Cube::m_pPixelShader = nullptr;
ID3D11Buffer* Cube::m_pIndexBuffer = nullptr;
int Cube::m_nRef = 0;

// �R���X�g���N�^
Cube::Cube() : m_pVertexBuffer(nullptr),
m_color(1.0f, 1.0f, 1.0f, 0.5f)
{
	XMStoreFloat4x4(&m_world, XMMatrixIdentity());
}

// �f�X�g���N�^
Cube::~Cube()
{
	SAFE_RELEASE(m_pVertexBuffer);
}

// ������
HRESULT Cube::Init(XMFLOAT3* pHalfSize)
{
	HRESULT hr = S_OK;
	ID3D11Device* pDevice = GetDevice();

	if (m_nRef == 0) {
		// �V�F�[�_������
		static const D3D11_INPUT_ELEMENT_DESC layout[] = {
			{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0, 0,                            D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
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

		// �C���f�b�N�X �o�b�t�@����
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
	// ���_�o�b�t�@����
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
	// ���_�o�b�t�@����
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

// �I������
void Cube::Uninit()
{
	// ���_�o�b�t�@���
	SAFE_RELEASE(m_pVertexBuffer);
	// �Q�ƃJ�E���^���Z
	--m_nRef;
	if (m_nRef == 0) {
		// �C���f�b�N�X�o�b�t�@���
		SAFE_RELEASE(m_pIndexBuffer);
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
void Cube::Update()
{

}

// �`��
void Cube::Draw()
{
	// �V�F�[�_�ݒ�
	ID3D11DeviceContext* pDeviceContext = GetDeviceContext();
	pDeviceContext->VSSetShader(m_pVertexShader, nullptr, 0);
	pDeviceContext->PSSetShader(m_pPixelShader, nullptr, 0);
	pDeviceContext->IASetInputLayout(m_pInputLayout);

	// �v���~�e�B�u�`����Z�b�g
	pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// ���_�o�b�t�@�ݒ�
	UINT stride = sizeof(VERTEX);
	UINT offset = 0;
	pDeviceContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);
	// �C���f�b�N�X�o�b�t�@�ݒ�
	pDeviceContext->IASetIndexBuffer(m_pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
	// �萔�o�b�t�@�ݒ�
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
	// �`��
	pDeviceContext->DrawIndexed(36, 0, 0);
}
