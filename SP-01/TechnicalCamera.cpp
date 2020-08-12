//*****************************************************************************
// Camera3D.cpp
// カメラを管理する
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
// マクロ定義
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
// グローバル変数
//*****************************************************************************
TechCamera* MainCamera = nullptr;
bool bFocalPointIsGameObject = false;


//*****************************************************************************
// コンストラクタ関数
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
//Init関数
//初期化関数
//引数：void
//戻：void
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
//Update関数
//変更関数
//引数：void
//戻：void
//*****************************************************************************
void TechCamera::Update()
{
	Camera3D* FocusPoint = ((Camera3D*)FocalPoint);
	if (!FocusPoint)
		return;
	//Model3D* FocusPoint = ((GameObject3D*)FocalPoint)->GetModel();
	XMFLOAT3 vLookAt = XMFLOAT3(LookEye.x, LookEye.y, 0);//注視点

	//if (GetAxis(MOVEMENT_AXIS_VERTICAL))
	//{
	//	LookEye.y += 10 * GetAxis(MOVEMENT_AXIS_VERTICAL);
	//	printf("%f\n", LookEye.y);
	//}

	vEye = Position;
	XMMATRIX mtxWorld = XMLoadFloat4x4(FocusPoint->GetCameraWorld());
	//視点
	XMStoreFloat3(&g_posCameraP, XMVector3TransformCoord(XMLoadFloat3(&vEye), mtxWorld));

	//注視点
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
//Uninit関数
//終了関数
//引数：void
//戻：void
//*****************************************************************************
void TechCamera::End()
{
	//なし
}

//*****************************************************************************
//SetCamera関数
//現在のフレームにカメラの拠点と回転を設定する
//引数：void
//戻：void
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
//GetCameraPos関数
//カメラの拠点を戻す
//引数：void
//戻：XMFLOAT3&
//*****************************************************************************
XMFLOAT3& TechCamera::GetCameraPos()
{
	return g_posCameraP;
}

//*****************************************************************************
//GetCameraAngle関数
//カメラの回転を戻す
//引数：void
//戻：XMFLOAT3&
//*****************************************************************************
XMFLOAT3& TechCamera::GetCameraAngle(void)
{
	Camera3D* FocusPoint = ((Camera3D*)FocalPoint);
	if (FocusPoint)
		g_rotCamera = FocusPoint->GetRotation();
	return g_rotCamera;
}

//*****************************************************************************
//GetViewMatrix関数
//カメラのビューマトリックスを戻す
//引数：void
//戻：XMFLOAT4X4&
//*****************************************************************************
XMFLOAT4X4 & TechCamera::GetViewMatrix()
{
	return g_mtxView;
}

//*****************************************************************************
//GetProjMatrix関数
//カメラのプロジェクションマトリックスを戻す
//引数：void
//戻：XMFLOAT4X4&
//*****************************************************************************
XMFLOAT4X4 & TechCamera::GetProjMatrix()
{
	return m_mProj;
}

//*****************************************************************************
//GetForwardCameraVector関数
//カメラのフォーワードベクトルを戻す
//引数：void
//戻：XMFLOAT3
//*****************************************************************************
XMFLOAT3 TechCamera::GetForwardCameraVector()
{
	float pitch = g_rotCamera.y;
	float yaw = g_rotCamera.y;
	return { cosf(pitch)*cosf(yaw),cosf(pitch)*sinf(yaw),sinf(pitch) };
}

//*****************************************************************************
//SetFocalPoint関数
//カメラの焦点を設定する
//引数：void*
//戻：void
//*****************************************************************************
void TechCamera::SetFocalPoint(void * newFocalPoint)
{
	FocalPoint = newFocalPoint;
	bFocalPointIsGameObject = false;
}

//*****************************************************************************
//SetFocalPointGO関数
//カメラの焦点を設定する
//引数：void*
//戻：void
//*****************************************************************************
void TechCamera::SetFocalPointGO(void * newFocalPoint)
{
	FocalPoint = newFocalPoint;
	bFocalPointIsGameObject = true;
}

//*****************************************************************************
//ZoomIn関数
//カメラのズームアウト（Z座標）を設定する
//引数：float
//戻：void
//*****************************************************************************
void TechCamera::ZoomIn(float inc)
{
	vsOffset.z += inc;
}

//*****************************************************************************
//ResetOffset関数
//カメラのズームオフセットをリセットする
//引数：void
//戻：void
//*****************************************************************************
void TechCamera::ResetOffset()
{
	vsOffset = { 0,0,0 };
}

//*****************************************************************************
//SetZoomLock関数
//カメラのズームアウト（Z座標）を設定する
//引数：float
//戻：void
//*****************************************************************************
void TechCamera::SetZoomLock(float flock)
{
	fLockOnZoom = flock;
}

//*****************************************************************************
//SetYOffsetLock関数
//カメラのズームアウト（Y座標）を設定する
//引数：float
//戻：void
//*****************************************************************************
void TechCamera::SetYOffsetLock(float OffsetLockOnY)
{
	fLockOnOffset = OffsetLockOnY;
}

//*****************************************************************************
//SetAttackZoom関数
//攻撃動作の時にカメラのズームアウト（Z座標）を設定する
//引数：float, float
//戻：void
//*****************************************************************************
void TechCamera::SetAttackZoom(float Zoom, float AttackZoomFrames)
{
	fAttackZoomFrames = AttackZoomFrames;
	fMaxAttackZoom = Zoom;
}

//*****************************************************************************
//ShakeCamera関数
//カメラに揺らせる
//引数：float, float
//戻：void
//*****************************************************************************
void TechCamera::SetShake(float fShakeForce, float Frames)
{
	ShakeForce = {fShakeForce, fShakeForce};
	nShakeFrames = Frames;
}

//*****************************************************************************
//GetMainCamera関数
//メインカメラを戻す
//引数：void
//戻：Camera3D*
//*****************************************************************************
TechCamera * GetMainCamera()
{
	return MainCamera;
}
