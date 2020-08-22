//*****************************************************************************
// Billboard2D.cpp
// �r�[���{�[�h���Ǘ�����
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
// �}�N����`
//*****************************************************************************
#define	VALUE_MOVE_TREE		(3.00f)							// �ړ����x

//*****************************************************************************
//�R���X�g���N�^�֐�
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
//Init�֐�
//�������֐�
//�����Fvoid
//�߁FHRESULT
//*****************************************************************************
HRESULT Billboard::Init(const char* szpath)
{
	ID3D11Device* pDevice = GetDevice();
	Position = XMFLOAT3(0.0f, 0.0f, 0.0f);
	Scale = XMFLOAT3(1.0f, 1.0f, 1.0f);
	Rotation = XMFLOAT3(0.0f, 0.0f, 0.0f);
	MakeVertex(pDevice);

	// �e�N�X�`���̓ǂݍ���
	

	//pMesh->pTexture = pTexture;
	//XMStoreFloat4x4(&pMesh->mtxTexture, XMMatrixIdentity());

	// �}�e���A���̐ݒ�
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
//Update�֐�
//�ύX�֐�
//�����Fvoid
//�߁Fvoid
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
//Draw�֐�
//�����_�����O�֐�
//�����Fvoid
//�߁Fvoid
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

	// ���[���h�}�g���b�N�X�̏�����
	mtx_World = XMMatrixIdentity();
	XMStoreFloat4x4(&mtxWorld, mtx_World);

	// �|���S���𐳖ʂɌ�����
	this->mtxWorld._11 = mtxView._11;
	this->mtxWorld._12 = mtxView._21;
	this->mtxWorld._13 = mtxView._31;
	this->mtxWorld._21 = mtxView._12;
	this->mtxWorld._22 = mtxView._22;
	this->mtxWorld._23 = mtxView._32;
	this->mtxWorld._31 = mtxView._13;
	this->mtxWorld._32 = mtxView._23;
	this->mtxWorld._33 = mtxView._33;

	// �X�P�[���𔽉f
	mtxScale = XMMatrixScaling(fWidth,
		fHeight, 1.0f);
	mtx_World = XMMatrixMultiply(mtxScale, mtx_World);
	mtxRotate = XMMatrixRotationRollPitchYaw(Rotation.x, Rotation.y, Rotation.z);
	mtx_World = XMMatrixMultiply(mtx_World, mtxRotate);

	
	// �ړ��𔽉f
	mtxTranslate = XMMatrixTranslation(Position.x,
		Position.y,
		Position.z);
	mtx_World = XMMatrixMultiply(mtx_World, mtxTranslate);

	// ���[���h�}�g���b�N�X�̐ݒ�
	XMStoreFloat4x4(&this->mtxWorld, mtx_World);

	// ���b�V���̕`��
	XMMATRIX mtxTex = XMMatrixScaling(1.0f / (float)nFrameX, 1.0f / (float)nFrameY, 0.0f);
	mtxTex *= XMMatrixTranslation((float)uv.U / (float)nFrameX, (float)uv.V / (float)nFrameY, 0.0f);
	XMStoreFloat4x4(&mtxTexture, mtxTex);


	Mesh3D::Draw(pDeviceContext);
	g_bInTree = false;
	GetMainLight()->SetLightEnable(true);
}

//*****************************************************************************
//End�֐�
//�I���֐�
//�����Fvoid
//�߁Fvoid
//*****************************************************************************
void Billboard::End()
{
	// ���b�V���̊J��
	if(!bUsingOutsideTexture)
		SAFE_RELEASE(pTexture);
	ReleaseMesh();
}

//*****************************************************************************
//SetBillboard�֐�
//�r�[���{�[�h��ݒ肷��
//�����FXMFLOAT3, float, float, XMFLOAT4
//�߁Fvoid
//*****************************************************************************
void Billboard::SetBillboard(XMFLOAT3 pos, float fWidth, float fHeight, XMFLOAT4 color)
{
	pos = pos;
	bUse = true;
	// ���_���W�̐ݒ�
	SetVertex(fWidth, fHeight);
	// ���_�J���[�̐ݒ�
	SetColor(color);
}

//*****************************************************************************
//MakeVertex�֐�
//�o�[�e�b�N�X�����
//�����FID3D11Device*
//�߁FHRESULT
//*****************************************************************************
HRESULT Billboard::MakeVertex(ID3D11Device * pDevice)
{
	nNumVertex = 4;
	nNumIndex = 4;

	// �I�u�W�F�N�g�̒��_�z��𐶐�
	VERTEX_3D* pVertexWk = new VERTEX_3D[nNumVertex];

	// ���_�z��̒��g�𖄂߂�
	VERTEX_3D* pVtx = pVertexWk;

	// ���_���W�̐ݒ�
	pVtx[0].vtx = XMFLOAT3(-1.0f / 2, 0.0f, 0.0f);
	pVtx[1].vtx = XMFLOAT3(-1.0f / 2, 1.0f, 0.0f);
	pVtx[2].vtx = XMFLOAT3(1.0f / 2, 0.0f, 0.0f);
	pVtx[3].vtx = XMFLOAT3(1.0f / 2, 1.0f, 0.0f);

	// �@���̐ݒ�
	pVtx[0].nor = XMFLOAT3(0.0f, 0.0f, -1.0f);
	pVtx[1].nor = XMFLOAT3(0.0f, 0.0f, -1.0f);
	pVtx[2].nor = XMFLOAT3(0.0f, 0.0f, -1.0f);
	pVtx[3].nor = XMFLOAT3(0.0f, 0.0f, -1.0f);

	// ���ˌ��̐ݒ�
	pVtx[0].diffuse = Color;
	pVtx[1].diffuse = Color;
	pVtx[2].diffuse = Color;
	pVtx[3].diffuse = Color;

	// �e�N�X�`�����W�̐ݒ�
	pVtx[0].tex = XMFLOAT2(0.0f, 1.0f);
	pVtx[1].tex = XMFLOAT2(0.0f, 0.0f);
	pVtx[2].tex = XMFLOAT2(1.0f, 1.0f);
	pVtx[3].tex = XMFLOAT2(1.0f, 0.0f);

	// �C���f�b�N�X�z��𐶐�
	int* pIndexWk = new int[nNumIndex];

	// �C���f�b�N�X�z��̒��g�𖄂߂�
	pIndexWk[0] = 0;
	pIndexWk[1] = 1;
	pIndexWk[2] = 2;
	pIndexWk[3] = 3;

	// ���_�o�b�t�@����
	HRESULT hr = MakeMeshVertex(pDevice, pVertexWk, pIndexWk);
	// �ꎞ�z��̉��
	delete[] pIndexWk;
	delete[] pVertexWk;

	return hr;
}

//*****************************************************************************
//SetVertex�֐�
//�o�[�e�b�N�X��ݒ肷��
//�����Ffloat, float
//�߁Fvoid
//*****************************************************************************
void Billboard::SetVertex(float fWidth, float fHeight)
{
	this->fWidth = fWidth;
	this->fHeight = fHeight;
}

//*****************************************************************************
//SetColor�֐�
//�F��ݒ肷��
//�����FXMFLOAT4
//�߁Fvoid
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
//SetPosition�֐�
//���_��ݒ肷��
//�����FXMFLOAT3
//�߁Fvoid
//*****************************************************************************
void Billboard::SetPosition(XMFLOAT3 newPosition)
{
	Position = newPosition;
}

//*****************************************************************************
//SetWidth�֐�
//����ݒ肷��
//�����Ffloat
//�߁Fvoid
//*****************************************************************************
void Billboard::SetWidth(float newWidth)
{
	fWidth = newWidth;
}

//*****************************************************************************
//SetHeight�֐�
//������ݒ肷��
//�����Ffloat
//�߁Fvoid
//*****************************************************************************
void Billboard::SetHeight(float newHeight)
{
	fHeight = newHeight;
}

//*****************************************************************************
//SetUVFrames�֐�
//UV�t���[����ݒ肷��
//�����Fint, int
//�߁Fvoid
//*****************************************************************************
void Billboard::SetUVFrames(int nX, int nY, int nSlow)
{
	nFrameX = nX;
	nFrameY = nY;
	nSlowness = nSlow;
}

//*****************************************************************************
//SetScale�֐�
//�傫����ݒ肷��
//�����Ffloat
//�߁Fvoid
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
//ScaleUp�֐�
//�傫���𑝂₷
//�����Ffloat
//�߁Fvoid
//*****************************************************************************
void Billboard::ScaleUp(float nS_Up)
{
	fHeight += nS_Up;
	fWidth  += nS_Up;
}

//*****************************************************************************
//GetAlpha�֐�
//�A���t�@��߂�
//�����Fvoid
//�߁Ffloat
//*****************************************************************************
float Billboard::GetAlpha()
{
	return Color.w;
}

//*****************************************************************************
//GetUse�֐�
//�g�����Ƃ��m�F����
//�����Fvoid
//�߁Fbool
//*****************************************************************************
bool Billboard::GetUse()
{
	return bUse;
}

//*****************************************************************************
//SetUse�֐�
//�g�����Ƃ�ݒ肷��
//�����Fbool
//�߁Fvoid
//*****************************************************************************
void Billboard::SetUse(bool use)
{
	bUse = use;
}

//*****************************************************************************
//ResetUV�֐�
//UV�����Z�b�g����
//�����Fvoid
//�߁Fvoid
//*****************************************************************************
void Billboard::ResetUV()
{
	uv = { 0,0 };
}

//*****************************************************************************
//SetUnusableAfterAnimation�֐�
//�A�j���[�V�������I�������A�I�u�W�F�N�g�̏�Ԃ��g��Ȃ����Ƃɂ���
//�����Fbool
//�߁Fvoid
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
