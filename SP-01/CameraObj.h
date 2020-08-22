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
	float		fZoomOut;
	//揺るぐ
	float		fShakeIntensity;
	int			nShakeFrames;
	int			nFramesPerShake;
	//ズームイン
	float		fZoomTargetIntensity;
	float		fZoomIntensity;
	float		fZoomSpeed;
	int			nZoomFrames;
	float		fZoomAcceleration;
public:
	Camera3D(int type);
	~Camera3D();
	void Init();
	void Update();
	void PlayerCameraControl();
	void Draw();
	void End();
	XMFLOAT3 GetRotation();
	XMFLOAT3 GetRotation(bool Offset);
	void SetObjectToFollow(Object3D* pObj) { pFollowObj = pObj;};
	XMFLOAT4X4* GetCameraWorld() { return &g_mtxWorld; };
	void SetShaking(float intensity, int frames, int framespershake) { if (nShakeFrames > 0 && intensity == fShakeIntensity && framespershake == nFramesPerShake)return; fShakeIntensity = intensity;  nShakeFrames = frames; nFramesPerShake = framespershake; };
	void SetZooming(float intensity, int frames, float speed, float accel) { if (nZoomFrames > 0 && intensity == fZoomTargetIntensity && fZoomSpeed == speed && fZoomAcceleration == accel)return; nZoomFrames = frames; fZoomTargetIntensity = intensity; fZoomSpeed = speed; fZoomAcceleration = accel;};
	void SetZoomOut(float zoom) { fZoomOut = zoom; };
	float GetTempZoomIntensity() {return fZoomTargetIntensity;};
};

