// 爆発エフェクト クラス [Explosion.h]
#pragma once
#include "main.h"
#include "FbxModel.h"

class CExplosion;

// 破片
class CDebris
{
private:
	CExplosion* m_pParent;	// 爆発エフェクト
	CDebris* m_pBack;		// 前の破片
	CDebris* m_pNext;		// 次の破片
	XMFLOAT3 Scale;
	ID3D11Buffer* m_pVertexBuffer;	// 頂点バッファ
	XMFLOAT4X4 m_world;		// ワールド変換
	XMFLOAT3 m_velocity;	// 速度
	XMFLOAT3 m_accel;		// 加速度
	XMFLOAT3 m_axis;		// 回転軸
	float m_rotate;			// 回転速度
	float m_alpha;			// 不透明度
public:
	CDebris(CExplosion* pExp);	// コンストラクタ
	~CDebris();					// デストラクタ

	CDebris* GetTop();			// リスト先頭を取得
	void SetTop(CDebris* pTop);	// リスト先頭を設定
	CDebris* GetNext() { return m_pNext; }

	HRESULT Init(XMFLOAT3& v0, XMFLOAT3& v1, XMFLOAT3& v2,
		float fRadius, XMFLOAT4X4* pWorld, XMFLOAT3* pVelocity);	// 初期化
	void Uninit();	// 終了処理
	void Update();	// 更新
	void Draw();	// 描画
	void SetScale(XMFLOAT3 Scale);
};

// 爆発エフェクト
class CExplosion
{
private:
	CDebris* m_pDebris;	// 破片
	XMFLOAT3 Scale;
	static ID3D11Buffer* m_pConstantBuffer[2];		// 定数バッファ
	static ID3D11VertexShader* m_pVertexShader;		// 頂点シェーダ
	static ID3D11InputLayout* m_pInputLayout;		// 頂点フォーマット
	static ID3D11PixelShader* m_pPixelShader;		// ピクセルシェーダ
	static int m_nRef;	// 参照カウンタ
public:
	CExplosion();
	~CExplosion();

	CDebris* GetTop() { return m_pDebris; }
	void SetTop(CDebris* pTop) { m_pDebris = pTop; }

	HRESULT Init();	// 初期化
	void Uninit();	// 終了処理
	void Update();	// 更新
	void Draw();	// 描画
	void SetScale(XMFLOAT3 Scale);
	// 爆発開始
	HRESULT Start(CFbxModel* pModel, XMFLOAT4X4* pWorld,
		XMFLOAT3* pVelocity = nullptr);

	// シェーダの定数バッファを更新
	void UpdateBuffer(int nShader, void* pData);
};
