//*****************************************************************************
// Camera3D.h
//*****************************************************************************
#pragma once

#include "DXWindow3D.h"
#include "UniversalStructures.h"
//*****************************************************************************
// �N���X
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
	 XMFLOAT3		Position;		// ���_���W
	 XMFLOAT3		LookEye;	// �����_���W
	 XMFLOAT3		m_vUp;		// ����x�N�g��
	 //XMFLOAT4X4		m_mView;	// �r���[�ϊ�
	 float			m_fFOVY;	// ����p
	 float			m_fAspect;	// �A�X�y�N�g��
	 float			m_fNearZ;	// �O���N���b�v����
	 float			m_fFarZ;	// ����N���b�v����
	 XMFLOAT3	m_vNowEye;	// ���݂̎��_
	 XMFLOAT3	m_vNowLook;	// ���݂̒����_
	 XMFLOAT3	m_vNowUp;	// ���݂̏���x�N�g��
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
// �v���g�^�C�v�錾
//*****************************************************************************
TechCamera* GetMainCamera();
