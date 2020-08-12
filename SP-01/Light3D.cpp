//*****************************************************************************
// Light3D.cpp
// ���̊Ǘ�
// Author: Mane
//*****************************************************************************
#include "Light3D.h"
#include "InputManager.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define LIGHT0_DIFFUSE	XMFLOAT4(1.0f,1.0f,1.0f,1.0f)
#define LIGHT0_AMBIENT	XMFLOAT4(0.2f,0.2f,0.2f,1.0f)
#define LIGHT0_SPECULAR	XMFLOAT4(1.0f,1.0f,1.0f,1.0f)
#define LIGHT0_DIR_X	(-1.0f)
#define LIGHT0_DIR_Y	(-1.0f)
#define LIGHT0_DIR_Z	(1.0f)
//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
Light3D* MainLightSource = nullptr;

//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
Light3D::Light3D()
{
	Init();
}

Light3D::Light3D(bool bMainLight)
{
	if (bMainLight)
		MainLightSource = this;
	Init();
}


Light3D::~Light3D()
{
	End();
}

//*****************************************************************************
//Init�֐�
//�������֐�
//�����Fvoid
//�߁Fvoid
//*****************************************************************************
HRESULT Light3D::Init(void)
{
	XMStoreFloat3(&g_UnivLight.m_direction,
		XMVector3Normalize(XMVectorSet(LIGHT0_DIR_X, LIGHT0_DIR_Y, LIGHT0_DIR_Z, 0.0f)));
	g_UnivLight.m_diffuse = LIGHT0_DIFFUSE;
	g_UnivLight.m_ambient = LIGHT0_AMBIENT;
	g_UnivLight.m_specular = LIGHT0_SPECULAR;
	return S_OK;
}

//*****************************************************************************
//End�֐�
//�I���֐�
//�����Fvoid
//�߁Fvoid
//*****************************************************************************
void Light3D::End(void)
{
	//����
}

//*****************************************************************************
//Update�֐�
//�ύX�֐�
//�����Fvoid
//�߁Fvoid
//*****************************************************************************
void Light3D::Update(void)
{
	//����
}

//*****************************************************************************
//Update�֐�
//�ύX�֐�
//�����Fvoid
//�߁Fvoid
//*****************************************************************************
CFbxLight* Light3D::GetLight(void)
{
	return &g_UnivLight;
}

//*****************************************************************************
//SetDiffuse�֐�
//�g�U���̐ݒ�
//�����FXMFLOAT4
//�߁Fvoid
//*****************************************************************************
void Light3D::SetDiffuse(XMFLOAT4 newDif)
{
	g_UnivLight.m_diffuse = newDif;
}

//*****************************************************************************
//SetAmbient�֐�
//�X�y�L�����[�̐ݒ�
//�����FXMFLOAT4
//�߁Fvoid
//*****************************************************************************
void Light3D::SetSpecular(XMFLOAT4 newSpec)
{
	g_UnivLight.m_specular = newSpec;
}

//*****************************************************************************
//SetAmbient�֐�
//�A���r�G���g���C�g�̐ݒ�
//�����FXMFLOAT4
//�߁Fvoid
//*****************************************************************************
void Light3D::SetAmbient(XMFLOAT4 newAmb)
{
	g_UnivLight.m_ambient = newAmb;
}

//*****************************************************************************
//SetDirection�֐�
//������ݒ肷��
//�����FXMFLOAT3
//�߁Fvoid
//*****************************************************************************
void Light3D::SetDirection(XMFLOAT3 newDir)
{
	g_UnivLight.m_direction = newDir;
}

//*****************************************************************************
//GetMainLight�֐�
//���C�g�̃A�h���X��߂�
//�����Fvoid
//�߁FLight3D*
//*****************************************************************************
Light3D * GetMainLight()
{
	return MainLightSource;
}

//*****************************************************************************
//SetLightEnable�֐�
//����L������
//�����Fbool
//�߁Fvoid
//*****************************************************************************
void  Light3D::SetLightEnable(bool bEnable)
{
	if (bEnable) {
		XMStoreFloat3(&g_UnivLight.m_direction,
			XMVector3Normalize(XMVectorSet(LIGHT0_DIR_X, LIGHT0_DIR_Y, LIGHT0_DIR_Z, 0.0f)));
	}
	else {
		g_UnivLight.m_direction = XMFLOAT3(0, 0, 0);
	}
	g_UnivLight.m_diffuse = LIGHT0_DIFFUSE;
	g_UnivLight.m_ambient = LIGHT0_AMBIENT;
	g_UnivLight.m_specular = LIGHT0_SPECULAR;
}