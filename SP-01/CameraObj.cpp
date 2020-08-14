#include "CameraObj.h"
#include "Player3D.h"
#include "InputManager.h"

#define OFFSET_Y 0.35f

Camera3D::Camera3D(int nCamType):Object3D(GO_CAMERA), pFollowObj(nullptr)
{
	Init();
	nCameraType = nCamType;
	nFramesPerShake = fShakeIntensity = nShakeFrames = fLockOnOffset = 0;
	fZoomSpeed = fZoomTargetIntensity = fZoomIntensity = nZoomFrames = fZoomAcceleration = 0;
}


Camera3D::~Camera3D()
{
}

void Camera3D::Init()
{
}

void Camera3D::Update()
{
	switch (nCameraType)
	{
	case CAMERA_PLAYER:
		PlayerCameraControl();
		break;
	default:
		break;
	}

	if (Rotation.y > XM_2PI) {
		Rotation.y -= XM_2PI;
	}
	if (Rotation.y < -XM_2PI) {
		Rotation.y += XM_2PI;
	}

	//揺るぐ
	if (--nShakeFrames <= 0)
	{
		nShakeFrames = 0;
		fShakeIntensity = 0;
	}
	else {
		static int FrameCount = nFramesPerShake;
		if (--FrameCount <= 0)
		{
			FrameCount = nFramesPerShake;
			fShakeIntensity *= -1;
		}
	}
	//ズームイン
	if (--nZoomFrames <= 0)
	{
		fZoomTargetIntensity = nZoomFrames = 0;
	}
	fZoomSpeed += fZoomAcceleration;
	if (fZoomIntensity != fZoomTargetIntensity)
	{
		if (fZoomIntensity < fZoomTargetIntensity)
		{
			fZoomIntensity += fZoomSpeed;
			if (fZoomIntensity > fZoomTargetIntensity)
				fZoomIntensity = fZoomTargetIntensity;
		}
		else if (fZoomIntensity > fZoomTargetIntensity)
		{
			fZoomIntensity -= fZoomSpeed;
			if (fZoomIntensity < fZoomTargetIntensity)
				fZoomIntensity = fZoomTargetIntensity;
		}
	}
	else
	{
		fZoomSpeed = 0;
	}
	//カメラ設定
	XMFLOAT3 ParentPos = { 0,0,0 };
	XMFLOAT3 ParentScale = { 1,1,1 };//親の拠点と親の大きさ
	if (pFollowObj) {
		ParentPos = pFollowObj->GetPosition();
		ParentScale = pFollowObj->GetScale();
	}
	ID3D11Device* pDevice = GetDevice();
	ID3D11DeviceContext* pDeviceContext = GetDeviceContext();
	XMMATRIX mtxWorld, mtxRot, mtxTranslate, mtxScale;

	//if(pFollowObj)
	//	mtxWorld = XMLoadFloat4x4(pFollowObj->GetModelWorld());
	//else
		mtxWorld = XMMatrixIdentity();

	//サイズ
	mtxScale = XMMatrixScaling(Scale.x * ParentScale.x, Scale.y * ParentScale.y, Scale.z * ParentScale.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxScale);

	// 回転を反映
	mtxRot = XMMatrixRotationRollPitchYaw(Rotation.x, Rotation.y+ fLockOnOffset, Rotation.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);



	// 移動を反映
	mtxTranslate = XMMatrixTranslation(Position.x + ParentPos.x + fShakeIntensity - (sinf(XM_PI + Rotation.y)*fZoomIntensity), Position.y + ParentPos.y, Position.z + ParentPos.z + (fLockOnOffset*30) + fShakeIntensity - (cosf(XM_PI + Rotation.y)*fZoomIntensity));
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);


	// ワールドマトリックスの設定
	XMStoreFloat4x4(&g_mtxWorld, mtxWorld);

}

void Camera3D::PlayerCameraControl()
{
	//printf("Cur: %f  --  Tar: %f\n", Rotation.y, x3TargetRotation.y);
	static bool bLockOnActivated = false;
	static float fAcceleration = 0;
	Player3D* pPlayer = GetPlayer();
	if (GetInput(INPUT_LOCKON) && pPlayer->GetLockedEnemy())
	{
		if (fLockOnOffset < OFFSET_Y)
			fLockOnOffset += 0.05f;
		if (fLockOnOffset > OFFSET_Y)
			fLockOnOffset = OFFSET_Y;
	}
	else
	{
		if (fLockOnOffset > 0)
			fLockOnOffset -= 0.045f;
		if (fLockOnOffset < 0)
			fLockOnOffset = 0;
	}
	if (GetInput(INPUT_CAMERA) && !bLockOnActivated) {
		x3TargetRotation = pPlayer->GetModel()->GetRotation();
		bLockOnActivated = true;
	}
	if (!bLockOnActivated && pPlayer->GetLockedEnemy() && (!pPlayer->GetCurrentAttack() || (pPlayer->GetCurrentAttack() && pPlayer->GetCurrentAttack()->Animation!=ROULETTE)))
	{
		float calc = abs(pPlayer->GetModel()->GetRotation().y - x3TargetRotation.y);
		//float fDistance = (x2*x1)2 + (y2*y1)2 + (z2*z1)2
		
		if (calc >= 3.0f && calc<=3.15f)
		{
			bLockOnActivated = true;
			x3TargetRotation.y = pPlayer->GetModel()->GetRotation().y-0.1f;
		}
	}

	if (!bLockOnActivated)
		return;

	if (Rotation.y > x3TargetRotation.y) {
		RotateAroundY(0.01f+ fAcceleration);
		if (Rotation.y < x3TargetRotation.y)
			Rotation.y = x3TargetRotation.y;
	}
	if (Rotation.y < x3TargetRotation.y)
	{
		RotateAroundY(-0.01f- fAcceleration);
		if (Rotation.y > x3TargetRotation.y)
			Rotation.y = x3TargetRotation.y;
	}
	fAcceleration += 0.025f;
	if (Rotation.y == x3TargetRotation.y) {
		bLockOnActivated = false;
		fAcceleration = 0;
	}

}

void Camera3D::Draw()
{
	//何もなし
	Object3D::Draw();
}

void Camera3D::End()
{
}

//*****************************************************************************
//GetRotation関数
//回転を戻す
//引数：void
//戻：XMFLOAT3
//*****************************************************************************
XMFLOAT3 Camera3D::GetRotation()
{
	return Rotation;
}
