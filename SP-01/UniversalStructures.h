//*****************************************************************************
// UniversalStructures.h
//*****************************************************************************
#pragma once
#include <windows.h>
#include <tchar.h>
#include <mmsystem.h>
#include <d3d11.h>
#include <DirectXMath.h>

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#ifndef SAFE_RELEASE
#define SAFE_RELEASE(x) {if(x){(x)->Release();x=nullptr;}}
#endif
#ifndef SAFE_DELETE
#define SAFE_DELETE(x) {if(x){delete(x);x=nullptr;}}
#endif
#ifndef SAFE_DELETE_ARRAY
#define SAFE_DELETE_ARRAY(x) {if(x){delete[](x);x=nullptr;}}
#endif
#define SHOW_HITBOX true
#define SCREEN_WIDTH	(1280)				// ウインドウの幅
#define SCREEN_HEIGHT	(720)				// ウインドウの高さ
#define SCREEN_CENTER_X	(SCREEN_WIDTH/2)	// ウインドウの中心Ｘ座標
#define SCREEN_CENTER_Y	(SCREEN_HEIGHT/2)	// ウインドウの中心Ｙ座標
#define NUM_VERTEX 4
#define CULLMODE_NONE	0					// カリングしない
#define CULLMODE_CW		1					// 前面カリング
#define CULLMODE_CCW	2					// 背面カリング
#define USE_CONSOLE true
#define NO_LIGHT_DEFAULT true
#define MAX_DECORATIONS 127-1//DEL
using namespace DirectX;

//*****************************************************************************
// 構造体定義
//*****************************************************************************
// 頂点フォーマット( 頂点座標[2D] / 反射光 / テクスチャ座標 )
typedef struct {
	XMFLOAT3 vtx;		// 頂点座標
	XMFLOAT4 diffuse;	// 拡散反射光
	XMFLOAT2 tex;		// テクスチャ座標
} VERTEX_2D;

typedef struct UV 
{
	float U, V;
};

typedef struct Box
{
	float PositionX = 0; 
	float PositionY = 0; 
	float PositionZ = 0;
	float SizeX=0, SizeY=0, SizeZ=0;
};

// 頂点フォーマット( 頂点座標[3D] / 法線ベクトル / 反射光 / テクスチャ座標 )
typedef struct {
	XMFLOAT3 vtx;		// 頂点座標
	XMFLOAT3 nor;		// 法線ベクトル
	XMFLOAT4 diffuse;	// 拡散反射光
	XMFLOAT2 tex;		// テクスチャ座標
} VERTEX_3D;
//*****************************************************************************
// エナム
//*****************************************************************************
enum eMODES
{
	MODE_SCORE_ATTACK = 0,
	MAX_MODES
};

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
bool IsInCollision3D(Box a, Box b);
bool CompareXmfloat3(XMFLOAT3 a, XMFLOAT3 b);
float GetAngle(XMFLOAT3, XMFLOAT3);
float DotProduct(XMFLOAT3, XMFLOAT3);
XMFLOAT3 MultiplyVector(XMFLOAT3, XMFLOAT3);
XMFLOAT3 SumVector(XMFLOAT3, XMFLOAT3);
XMFLOAT3 GetForwardVector(XMFLOAT3 Rot);
XMFLOAT3 GetVectorDifference(XMFLOAT3 a, XMFLOAT3 b);
XMFLOAT3 NormalizeVector(XMFLOAT3 v);
XMFLOAT3 MultiplyVectorByFloat(XMFLOAT3 a, float flot);
float GetDistance(XMFLOAT3 obj1, XMFLOAT3 obj2);