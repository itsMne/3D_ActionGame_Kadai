#include "CameraObj.h"
#include "InputManager.h"

#define OFFSET_Y 0.35f

Camera3D::Camera3D(int nCamType):Object3D(GO_CAMERA), pFollowObj(nullptr)
{
	Init();
	nCameraType = nCamType;
	fLockOnOffset = 0;
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
	mtxTranslate = XMMatrixTranslation(Position.x + ParentPos.x, Position.y + ParentPos.y, Position.z + ParentPos.z + fLockOnOffset*30);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);



	// ワールドマトリックスの設定
	XMStoreFloat4x4(&g_mtxWorld, mtxWorld);

}

void Camera3D::PlayerCameraControl()
{
	//printf("Cur: %f  --  Tar: %f\n", Rotation.y, x3TargetRotation.y);

	static bool bLockOnActivated = false;
	static float fAcceleration = 0;
	if (GetInput(INPUT_LOCKON))
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
		if (pFollowObj) {
			printf("a");
			x3TargetRotation = pFollowObj->GetModel()->GetRotation();
			bLockOnActivated = true;
		}
		else
			return;
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
