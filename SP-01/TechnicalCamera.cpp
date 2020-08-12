//*****************************************************************************
// Camera3D.cpp
// �J�������Ǘ�����
// Author : Mane
//*****************************************************************************
#include "TechnicalCamera.h"	
#include "input.h"
#include "Model3D.h"
#include "Object3D.h"
#include "CameraObj.h"
#include "InputManager.h"
#include <stdio.h>

//*****************************************************************************
// �}�N����`
//*****************************************************************************


#define VIEW_ANGLE			(XMConvertToRadians(55.0f))//45
#define VIEW_ASPECT			((float)SCREEN_WIDTH/(float)SCREEN_HEIGHT)
#define VIEW_NEAR			(10.0f)
#define VIEW_FAR			(2000.0f)//1000
#define VALUE_MOVE_CAMERA	(2.0f)	
#define VALUE_ROTATE_CAMERA	(XM_PI*0.01f)	


#define INITIAL_CAM_POS {0, 65, -200}//382
#define INITIAL_CAM_EYE {0, 60, 0}
//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
TechCamera* MainCamera = nullptr;
bool bFocalPointIsGameObject = false;


//*****************************************************************************
// �R���X�g���N�^�֐�
//*****************************************************************************
TechCamera::TechCamera(): FocalPoint(nullptr)
{
	MainCamera = this;
	Init();
}

TechCamera::TechCamera(bool bisMainCamera) : FocalPoint(nullptr)
{
	if (bisMainCamera)
		MainCamera = this;
	Init();
}


TechCamera::~TechCamera()
{
	End();
}

//*****************************************************************************
//Init�֐�
//�������֐�
//�����Fvoid
//�߁Fvoid
//*****************************************************************************
HRESULT TechCamera::Init()
{
	g_posCameraP = XMFLOAT3(0, 0, 0);
	g_posCameraR = XMFLOAT3(0, 0, 0);
	g_vecCameraU = XMFLOAT3(0, 1, 0);
	g_rotCamera = XMFLOAT3(0, 0, 0);
	
	//

	HRESULT hr = S_OK;
	//Position = XMFLOAT3(0.0f, 200.0f, -400.0f);
	Position = INITIAL_CAM_POS;
	LookEye = INITIAL_CAM_EYE;
	m_vUp = XMFLOAT3(0.0f, 1.0f, 0.0f);
	m_fFOVY = VIEW_ANGLE;
	m_fAspect = ((float)SCREEN_WIDTH / (float)SCREEN_HEIGHT);
	m_fNearZ = VIEW_NEAR;
	m_fFarZ = VIEW_FAR;
	m_vNowEye = Position;
	m_vNowLook = LookEye;
	m_vNowUp = m_vUp;
	Update();
	return S_OK;
}

//*****************************************************************************
//Update�֐�
//�ύX�֐�
//�����Fvoid
//�߁Fvoid
//*****************************************************************************
void TechCamera::Update()
{
	Camera3D* FocusPoint = ((Camera3D*)FocalPoint);
	if (!FocusPoint)
		return;
	//Model3D* FocusPoint = ((GameObject3D*)FocalPoint)->GetModel();
	XMFLOAT3 vLookAt = XMFLOAT3(LookEye.x, LookEye.y, 0);//�����_

	//if (GetAxis(MOVEMENT_AXIS_VERTICAL))
	//{
	//	LookEye.y += 10 * GetAxis(MOVEMENT_AXIS_VERTICAL);
	//	printf("%f\n", LookEye.y);
	//}

	vEye = Position;
	XMMATRIX mtxWorld = XMLoadFloat4x4(FocusPoint->GetCameraWorld());
	//���_
	XMStoreFloat3(&g_posCameraP, XMVector3TransformCoord(XMLoadFloat3(&vEye), mtxWorld));

	//�����_
	XMStoreFloat3(&g_posCameraR, XMVector3TransformCoord(XMLoadFloat3(&vLookAt), mtxWorld));

	g_rotCamera = FocusPoint->GetRotation();
	
	//printf("%f, %f, %f\n", g_posCameraR.x, g_posCameraR.y, g_posCameraR.z);
	XMStoreFloat4x4(&g_mtxView, XMMatrixLookAtLH(
		XMVectorSet(g_posCameraP.x, g_posCameraP.y, g_posCameraP.z, 1),
		XMVectorSet(g_posCameraR.x, g_posCameraR.y, g_posCameraR.z, 1),
		XMVectorSet(g_vecCameraU.x, g_vecCameraU.y, g_vecCameraU.z, 0)
	)
	);
	XMStoreFloat4x4(&m_mProj,
		XMMatrixPerspectiveFovLH(m_fFOVY, m_fAspect,
			m_fNearZ, m_fFarZ));
	return;
	//XMStoreFloat4x4(&m_mProj, XMMatrixPerspectiveFovLH(VIEW_ANGLE, VIEW_ASPECT, VIEW_NEAR, VIEW_FAR));
	
}

//*****************************************************************************
//Uninit�֐�
//�I���֐�
//�����Fvoid
//�߁Fvoid
//*****************************************************************************
void TechCamera::End()
{
	//�Ȃ�
}

//*****************************************************************************
//SetCamera�֐�
//���݂̃t���[���ɃJ�����̋��_�Ɖ�]��ݒ肷��
//�����Fvoid
//�߁Fvoid
//*****************************************************************************
void TechCamera::SetCamera()
{
	XMStoreFloat4x4(&g_mtxView, XMMatrixLookAtLH(
		XMVectorSet(g_posCameraP.x, g_posCameraP.y, g_posCameraP.z, 1),
		XMVectorSet(g_posCameraR.x, g_posCameraR.y, g_posCameraR.z, 1),
		XMVectorSet(g_vecCameraU.x, g_vecCameraU.y, g_vecCameraU.z, 0)
	)
	);
	XMStoreFloat4x4(&m_mProj, XMMatrixPerspectiveFovLH(VIEW_ANGLE, VIEW_ASPECT, VIEW_NEAR, VIEW_FAR));
}

//*****************************************************************************
//GetCameraPos�֐�
//�J�����̋��_��߂�
//�����Fvoid
//�߁FXMFLOAT3&
//*****************************************************************************
XMFLOAT3& TechCamera::GetCameraPos()
{
	return g_posCameraP;
}

//*****************************************************************************
//GetCameraAngle�֐�
//�J�����̉�]��߂�
//�����Fvoid
//�߁FXMFLOAT3&
//*****************************************************************************
XMFLOAT3& TechCamera::GetCameraAngle(void)
{
	Camera3D* FocusPoint = ((Camera3D*)FocalPoint);
	if (FocusPoint)
		g_rotCamera = FocusPoint->GetRotation();
	return g_rotCamera;
}

//*****************************************************************************
//GetViewMatrix�֐�
//�J�����̃r���[�}�g���b�N�X��߂�
//�����Fvoid
//�߁FXMFLOAT4X4&
//*****************************************************************************
XMFLOAT4X4 & TechCamera::GetViewMatrix()
{
	return g_mtxView;
}

//*****************************************************************************
//GetProjMatrix�֐�
//�J�����̃v���W�F�N�V�����}�g���b�N�X��߂�
//�����Fvoid
//�߁FXMFLOAT4X4&
//*****************************************************************************
XMFLOAT4X4 & TechCamera::GetProjMatrix()
{
	return m_mProj;
}

//*****************************************************************************
//GetForwardCameraVector�֐�
//�J�����̃t�H�[���[�h�x�N�g����߂�
//�����Fvoid
//�߁FXMFLOAT3
//*****************************************************************************
XMFLOAT3 TechCamera::GetForwardCameraVector()
{
	float pitch = g_rotCamera.y;
	float yaw = g_rotCamera.y;
	return { cosf(pitch)*cosf(yaw),cosf(pitch)*sinf(yaw),sinf(pitch) };
}

//*****************************************************************************
//SetFocalPoint�֐�
//�J�����̏œ_��ݒ肷��
//�����Fvoid*
//�߁Fvoid
//*****************************************************************************
void TechCamera::SetFocalPoint(void * newFocalPoint)
{
	FocalPoint = newFocalPoint;
	bFocalPointIsGameObject = false;
}

//*****************************************************************************
//SetFocalPointGO�֐�
//�J�����̏œ_��ݒ肷��
//�����Fvoid*
//�߁Fvoid
//*****************************************************************************
void TechCamera::SetFocalPointGO(void * newFocalPoint)
{
	FocalPoint = newFocalPoint;
	bFocalPointIsGameObject = true;
}

//*****************************************************************************
//ZoomIn�֐�
//�J�����̃Y�[���A�E�g�iZ���W�j��ݒ肷��
//�����Ffloat
//�߁Fvoid
//*****************************************************************************
void TechCamera::ZoomIn(float inc)
{
	vsOffset.z += inc;
}

//*****************************************************************************
//ResetOffset�֐�
//�J�����̃Y�[���I�t�Z�b�g�����Z�b�g����
//�����Fvoid
//�߁Fvoid
//*****************************************************************************
void TechCamera::ResetOffset()
{
	vsOffset = { 0,0,0 };
}

//*****************************************************************************
//SetZoomLock�֐�
//�J�����̃Y�[���A�E�g�iZ���W�j��ݒ肷��
//�����Ffloat
//�߁Fvoid
//*****************************************************************************
void TechCamera::SetZoomLock(float flock)
{
	fLockOnZoom = flock;
}

//*****************************************************************************
//SetYOffsetLock�֐�
//�J�����̃Y�[���A�E�g�iY���W�j��ݒ肷��
//�����Ffloat
//�߁Fvoid
//*****************************************************************************
void TechCamera::SetYOffsetLock(float OffsetLockOnY)
{
	fLockOnOffset = OffsetLockOnY;
}

//*****************************************************************************
//SetAttackZoom�֐�
//�U������̎��ɃJ�����̃Y�[���A�E�g�iZ���W�j��ݒ肷��
//�����Ffloat, float
//�߁Fvoid
//*****************************************************************************
void TechCamera::SetAttackZoom(float Zoom, float AttackZoomFrames)
{
	fAttackZoomFrames = AttackZoomFrames;
	fMaxAttackZoom = Zoom;
}

//*****************************************************************************
//ShakeCamera�֐�
//�J�����ɗh�点��
//�����Ffloat, float
//�߁Fvoid
//*****************************************************************************
void TechCamera::SetShake(float fShakeForce, float Frames)
{
	ShakeForce = {fShakeForce, fShakeForce};
	nShakeFrames = Frames;
}

//*****************************************************************************
//GetMainCamera�֐�
//���C���J������߂�
//�����Fvoid
//�߁FCamera3D*
//*****************************************************************************
TechCamera * GetMainCamera()
{
	return MainCamera;
}
