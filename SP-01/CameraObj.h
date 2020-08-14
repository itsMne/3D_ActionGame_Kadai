#pragma once
#include "Object3D.h"

enum CAMERA_TYPE
{
	CAMERA_PLAYER,
	MAX_CAMERA_TYPE
};

class Camera3D :
	public Object3D
{
private:
	Object3D*	pFollowObj;
	XMFLOAT4X4	g_mtxWorld;		// ワールドマトリックス
	XMFLOAT3	x3TargetRotation;
	float	    fLockOnOffset;
	int			nCameraType;
public:
	Camera3D(int type);
	~Camera3D();
	void Init();
	void Update();
	void PlayerCameraControl();
	void Draw();
	void End();
	XMFLOAT3 GetRotation();
	void SetObjectToFollow(Object3D* pObj) { pFollowObj = pObj;};
	XMFLOAT4X4* GetCameraWorld() { return &g_mtxWorld; };
};

