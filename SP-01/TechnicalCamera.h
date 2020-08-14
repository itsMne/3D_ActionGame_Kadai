//*****************************************************************************
// Camera3D.h
//*****************************************************************************
#pragma once

#include "DXWindow3D.h"
#include "UniversalStructures.h"
//*****************************************************************************
// クラス
//*****************************************************************************
class TechCamera
{
private:
	 XMFLOAT3			g_posCameraP;
	 XMFLOAT3			g_posCameraR;
	 XMFLOAT3			g_vecCameraU;
	 XMFLOAT3			g_rotCamera;
	 float				g_fLengthInterval;
	 XMFLOAT4X4			m_mProj;
	 XMFLOAT4X4			g_mtxView;
	 void*				FocalPoint;
	 XMFLOAT3			vEye;
	 XMFLOAT3			vsOffset;
	 XMFLOAT2			ShakeForce;
	 float				fAcceleration;
	 float				fLockOnZoom;
	 float				fLockOnOffset;
	 float				fAttackZoom;
	 float				fMaxAttackZoom;
	 float				fAttackZoomFrames;
	 int				nShakeFrames;

	 //
	 XMFLOAT3		Position;		// 視点座標
	 XMFLOAT3		LookEye;	// 注視点座標
	 XMFLOAT3		m_vUp;		// 上方ベクトル
	 //XMFLOAT4X4		m_mView;	// ビュー変換
	 float			m_fFOVY;	// 視野角
	 float			m_fAspect;	// アスペクト比
	 float			m_fNearZ;	// 前方クリップ距離
	 float			m_fFarZ;	// 後方クリップ距離
	 XMFLOAT3	m_vNowEye;	// 現在の視点
	 XMFLOAT3	m_vNowLook;	// 現在の注視点
	 XMFLOAT3	m_vNowUp;	// 現在の上方ベクトル
public:
	TechCamera();
	TechCamera(bool bisMainCamera);
	~TechCamera();
	HRESULT Init();
	void Update();
	void End();
	void SetCamera();
	XMFLOAT3& GetCameraPos();
	XMFLOAT3& GetCameraAngle();
	XMFLOAT4X4& GetViewMatrix();
	XMFLOAT4X4& GetProjMatrix();
	XMFLOAT3 GetForwardCameraVector();
	void SetFocalPoint(void* newFocalPoint);
	void SetFocalPointGO(void* newFocalPoint);
	void ZoomIn(float inc);
	void ResetOffset();
	void SetZoomLock(float flock);
	void SetYOffsetLock(float OffsetLockOnY);
	void SetAttackZoom(float Zoom, float AttackZoomFrames);
	void SetShake(float ShakeForce, float Frames);
	XMFLOAT4X4& GetView() { return g_mtxView; }
	XMFLOAT4X4& GetProj() { return m_mProj; }
	XMFLOAT3& GetEye() { return vEye; }
	XMFLOAT3& GetLook() { return LookEye; }
	void* GetFocalPoint() { return FocalPoint; };
};

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
TechCamera* GetMainCamera();
