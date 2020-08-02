// ���E�{�b�N�X �f�o�b�O�\�� [Box.h]
#pragma once
#include "main.h"

class Cube
{
private:
	ID3D11Buffer* m_pVertexBuffer;	// ���_�o�b�t�@
	XMFLOAT4X4 m_world;
	XMFLOAT4 m_color;
	XMFLOAT3 xm3OffsetSize;
	static ID3D11Buffer* m_pConstantBuffer[2];		// �萔�o�b�t�@
	static ID3D11VertexShader* m_pVertexShader;		// ���_�V�F�[�_
	static ID3D11InputLayout* m_pInputLayout;		// ���_�t�H�[�}�b�g
	static ID3D11PixelShader* m_pPixelShader;		// �s�N�Z���V�F�[�_
	static ID3D11Buffer* m_pIndexBuffer;			// �C���f�b�N�X�o�b�t�@				// �C���f�b�N�X�o�b�t�@�C���^�[�t�F�[�X�ւ̃|�C���^
	static int m_nRef;	// �Q�ƃJ�E���^

public:
	Cube();
	~Cube();

	HRESULT Init(XMFLOAT3* pHalfSize);
	void Uninit();
	void Update();
	void Draw();

	XMFLOAT4X4& GetWorld() { return m_world; }
	void SetWorld(XMFLOAT4X4& world) { m_world = world; }
	XMFLOAT4& GetColor() { return m_color; }
	void SetColor(XMFLOAT4* pColor) { m_color = *pColor; }
};

