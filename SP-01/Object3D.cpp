//*****************************************************************************
// GameObject.cpp
// 一般的なゲームオブジェクトの管理
// Author : Mane
//*****************************************************************************
#include "Object3D.h"
#include "Cube3D.h"
#include "S_InGame3D.h"
#include "Player3D.h"
#include "Enemy.h"
#include "Field.h"
#include "Sound.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define BULLET_SPEED 30
#define PRINT_HITBOX false

//*****************************************************************************
// 構造体定義
//*****************************************************************************
int nInitedGameObjects = 0;

//*****************************************************************************
// コンストラクタ関数
//*****************************************************************************
Object3D::Object3D()
{
	nObjType = GO_MAX;
	Model = nullptr;
	p_goParent = nullptr;
	Init();
}

Object3D::Object3D(int Type)
{
	nObjType = Type;
	p_goParent = nullptr;
	Init();
	switch (Type)
	{
	case GO_TITLE_LOGO:
		Model = new Model3D(this, "data/model/LogoV3.fbx");
		break;
	case GO_BUNBUN:
		Model = new Model3D(this, BUNBUN_MODEL);
		Model->SwitchAnimation(1, 0, 1.0f);
		break;
	case GO_CHU:
		Model = new Model3D(this, CHU_MODEL);
		Model->SwitchAnimation(1, 0, 4.05f);
		Model->SetLoop(false);
		Model->SetFrameOfAnimation(250);
		break;
	case GO_ANGRY:
		Model = new Model3D(this, ANGRY_MODEL);
		Model->SwitchAnimation(0, 0, 1.0f);
		break;
	default:
		break;
	}
	nInitedGameObjects++;
}

Object3D::Object3D(const char * ModelPath, int Type)
{
	Init();
	Model = new Model3D(this, ModelPath);
	nInitedGameObjects++;
	nObjType = Type;
}

Object3D::Object3D(int ModelPath, int Type)
{
	Init();
	Model = new Model3D(this, ModelPath);
	nInitedGameObjects++;
	nObjType = Type;
}

Object3D::Object3D(Light3D* light, const char * ModelPath, int Type)
{
	Init();
	Model = new Model3D(this, ModelPath);
	Model->SetLight(light);
	nInitedGameObjects++;
	nObjType = Type;
	pLight = light;
}

Object3D::Object3D(Light3D * light, int ModelPath, int Type)
{
	Init();
	Model = new Model3D(this, ModelPath);
	Model->SetLight(light);
	nInitedGameObjects++;
	nObjType = Type;
	pLight = light;
}


Object3D::~Object3D()
{
	nInitedGameObjects--;
	End();
}

//*****************************************************************************
//Init関数
//初期化関数
//引数：void
//戻：void
//*****************************************************************************
void Object3D::Init()
{
	Rotation = { 0,0,0 };
	InitialPosition = Position = { 0,0,0 };
	Scale = { 1,1,1 };
	pMainCamera = GetMainCamera();
	bUse = true;
	ScaleUp = true;
	bIsUnlit = false;
	nUseCounterFrame = 0;
	fAcceleration = 0;
	p_goParent = nullptr;
	Model = nullptr;
	pExp = nullptr;
	pVisualHitbox = nullptr;
	pLight = nullptr;
	Hitbox = { 0,0,0,0,0,0 };
#if PRINT_HITBOX
	if (nType != GO_CUBE) {
		pVisualHitbox = new Cube3D();
		((Cube3D*)pVisualHitbox)->Init("data/texture/hbox.tga");
		((Cube3D*)pVisualHitbox)->SetScale({ Hitbox.SizeX, Hitbox.SizeY, Hitbox.SizeZ });
		((Cube3D*)pVisualHitbox)->SetPosition({ Hitbox.PositionX,Hitbox.PositionY,Hitbox.PositionZ });
	}
#endif
}

//*****************************************************************************
//Update関数
//変更関数
//引数：void
//戻：void
//*****************************************************************************
void Object3D::Update()
{
	if (!bUse)
		return;
	if (Model)
		Model->UpdateModel();
	XMFLOAT3* ModelScale;
	switch (nObjType)
	{
	case GO_BUNBUN:
		if (!p_goParent)
			return;
		BunBunControl();
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
}

void Object3D::BunBunControl()
{
	Model->SetRotation(p_goParent->GetModel()->GetRotation());
	Player3D* Player = (Player3D*)p_goParent;

	switch (Model->GetCurrentAnimation())
	{
	case BUN_BUN_APPEARS:
		if (Model->GetCurrentFrame() >= 4057) {
			SetAnimation(BUN_BUN_IDLE, 1.0f);
			Model->SetLoop(true);
		}
		break;
	case BUN_BUN_IDLE:
		if (Player->GetState() == PLAYER_BUNBUN_FALLING) {
			SetAnimation(BUN_BUN_DISAPPEARS, 1.0f);
			Model->SetLoop(false);
		}
		break;
	default:
		break;
	}

}


//*****************************************************************************
//Draw関数
//レンダリング関数
//引数：void
//戻：void
//*****************************************************************************
void Object3D::Draw()
{
#if PRINT_HITBOX
	if (pVisualHitbox) {
		Box ThisHitbox = GetHitbox();
		((Cube3D*)pVisualHitbox)->SetScale({ ThisHitbox.SizeX, ThisHitbox.SizeY, ThisHitbox.SizeZ });
		((Cube3D*)pVisualHitbox)->SetPosition({ ThisHitbox.PositionX,ThisHitbox.PositionY,ThisHitbox.PositionZ });
		((Cube3D*)pVisualHitbox)->Draw();
	}
#endif
	GetDeviceContext()->RSSetState(GetMainWindow()->GetRasterizerState(1));
	if (!bUse)
		return;
	if (nObjType == GO_CHU && Model && Model->GetCurrentFrame() >= 249)
		return;
	if (bIsUnlit || NO_LIGHT_DEFAULT)
		GetMainLight()->SetLightEnable(false);
	if (Model && !p_goParent)
		Model->DrawModel();
	else if(Model && p_goParent)
		Model->DrawModel(p_goParent->GetPositionAddr(), p_goParent->GetScaleAddr(), p_goParent->GetRotationAddr());
	GetMainLight()->SetLightEnable(true);
	

}

//*****************************************************************************
//End関数
//終了関数
//引数：void
//戻：void
//*****************************************************************************
void Object3D::End()
{
	SAFE_DELETE(Model);
	SAFE_DELETE(pExp);
#if PRINT_HITBOX
	SAFE_DELETE(pVisualHitbox);
#endif
}

//*****************************************************************************
//GetPosition関数
//拠点を戻す
//引数：void
//戻：XMFLOAT3
//*****************************************************************************
XMFLOAT3 Object3D::GetPosition()
{
	return Position;
}

//*****************************************************************************
//GetRotation関数
//回転を戻す
//引数：void
//戻：XMFLOAT3
//*****************************************************************************
XMFLOAT3 Object3D::GetRotation()
{
	return Rotation;
}

//*****************************************************************************
//GetScale関数
//大きさを戻す
//引数：void
//戻：XMFLOAT3
//*****************************************************************************
XMFLOAT3 Object3D::GetScale()
{
	return Scale;
}

XMFLOAT3 * Object3D::GetPositionAddr()
{
	return &Position;
}

XMFLOAT3 * Object3D::GetRotationAddr()
{
	return &Rotation;
}

XMFLOAT3 * Object3D::GetScaleAddr()
{
	return &Scale;
}

//*****************************************************************************
//GetModel関数
//モデルのアドレスを戻す
//引数：void
//戻：Model3D*
//*****************************************************************************
Model3D * Object3D::GetModel()
{
	return Model;
}

//*****************************************************************************
//SetScale関数
//大きさを設定する
//引数：float
//戻：void
//*****************************************************************************
void Object3D::SetScale(float fScale)
{
	if (fScale < 0) 
	{
		Scale = { 0,0,0 };
		return;
	}
	Scale = { fScale, fScale, fScale };
}

//*****************************************************************************
//SetScale関数
//大きさを設定する
//引数：XMFLOAT3
//戻：void
//*****************************************************************************
void Object3D::SetScale(XMFLOAT3 fScale)
{
	Scale = fScale;
}

void Object3D::SetScaling(float fScale)
{
	Scale = { Scale.x + fScale, Scale.y + fScale, Scale.z + fScale };
}

//*****************************************************************************
//SetRotation関数
//回転を設定する
//引数：XMFLOAT3
//戻：void
//*****************************************************************************
void Object3D::SetRotation(XMFLOAT3 fRot)
{
	Rotation = fRot;
}

//*****************************************************************************
//GetModelWorld関数
//ワールドマトリックスのアドレスを戻す
//引数：void
//戻：XMFLOAT4X4*
//*****************************************************************************
XMFLOAT4X4* Object3D::GetModelWorld()
{
	XMFLOAT3 ParPosition, ParRotation, ParScale;
	ParPosition = ParRotation = {0,0,0};
	ParScale = { 1,1,1 };
	if (p_goParent)
	{
		ParPosition = p_goParent->GetPosition();
		ParRotation = p_goParent->GetRotation();
		ParScale = p_goParent->GetScale();
	}
	XMMATRIX mtxWorld, mtxRot, mtxTranslate, mtxScale;
	// ワールドマトリックスの初期化
	mtxWorld = XMMatrixIdentity();

	mtxScale = XMMatrixScaling(Scale.x*ParScale.x, Scale.y*ParScale.y, Scale.z*ParScale.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxScale);

	mtxRot = XMMatrixRotationRollPitchYaw(Rotation.x + ParRotation.x, Rotation.y + ParRotation.y, Rotation.z + ParRotation.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

	mtxTranslate = XMMatrixTranslation(Position.x + ParPosition.x, Position.y + ParPosition.y, Position.z + ParPosition.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);
	
	XMStoreFloat4x4(&WorldMatrix, mtxWorld);
	
	// ワールドマトリックスの設定
	return &WorldMatrix;

}

//*****************************************************************************
//SetPosition関数
//拠点を設定する
//引数：XMFLOAT3
//戻：void
//*****************************************************************************
void Object3D::SetPosition(XMFLOAT3 newPos)
{
	Position = newPos;
}

//*****************************************************************************
//SetPosition関数
//拠点を設定する
//引数：XMFLOAT3, bool
//戻：void
//*****************************************************************************
void Object3D::SetPosition(XMFLOAT3 newPos, bool bInitial)
{
	Position = newPos;
	if (bInitial)
		InitialPosition = Position;
}

//*****************************************************************************
//SetRotation関数
//モデルの回転を設定する
//引数：XMFLOAT3
//戻：void
//*****************************************************************************
void Object3D::SetModelRotation(XMFLOAT3 rot)
{
	if(Model)
		Model->SetRotation(rot);
}

//*****************************************************************************
//RotateAroundX関数
//X座標で回転する
//引数：float
//戻：void
//*****************************************************************************
void Object3D::RotateAroundX(float x)
{
	Rotation.x -= x;
	if (Rotation.x > XM_2PI) {
		Rotation.x -= XM_2PI;
	}
}

//*****************************************************************************
//RotateAroundY関数
//Y座標で回転する
//引数：float
//戻：void
//*****************************************************************************
void Object3D::RotateAroundY(float y)
{
	Rotation.y -= y;

}

//*****************************************************************************
//RotateAroundZ関数
//Z座標で回転する
//引数：float
//戻：void
//*****************************************************************************
void Object3D::RotateAroundZ(float z)
{
	Rotation.z -= z;
	if (Rotation.z > XM_2PI) {
		Rotation.z -= XM_2PI;
	}
}

//*****************************************************************************
//SetUse関数
//使ってることを設定する
//引数：bool
//戻：void
//*****************************************************************************
void Object3D::SetUse(bool newUse)
{
	bUse = newUse;
}

//*****************************************************************************
//InitModel関数
//モデルの初期化
//引数：const char*
//戻：void
//*****************************************************************************
void Object3D::InitModel(const char * szPath)
{
	if (Model)
		SAFE_DELETE(Model);
	Model = new Model3D(this, szPath);
}

//*****************************************************************************
//InitModel関数
//モデルの初期化
//引数：int
//戻：void
//*****************************************************************************
void Object3D::InitModel(int szPath)
{
	if (Model)
		SAFE_DELETE(Model);
	Model = new Model3D(this, szPath);
}

//*****************************************************************************
//SetParent関数
//親オブジェクトを設定する
//引数：GameObject3D*
//戻：void
//*****************************************************************************
void Object3D::SetParent(Object3D * pNewParent)
{
	p_goParent = pNewParent;
}

void Object3D::Translate(XMFLOAT3 translation)
{
	TranslateX(translation.x);
	TranslateY(translation.y);
	TranslateZ(translation.z);
}

//*****************************************************************************
//TranslateX関数
//プレイヤーに動かせる（X座標）
//引数：float
//戻：void
//*****************************************************************************
void Object3D::TranslateX(float translation)
{
	Position.x += translation;
}

//*****************************************************************************
//TranslateY関数
//プレイヤーに動かせる（Y座標）
//引数：float
//戻：void
//*****************************************************************************
void Object3D::TranslateY(float translation)
{
	Position.y += translation;
}

//*****************************************************************************
//TranslateZ関数
//プレイヤーに動かせる（Z座標）
//引数：float
//戻：void
//*****************************************************************************
void Object3D::TranslateZ(float translation)
{
	Position.z += translation;
}

//*****************************************************************************
//IsInUse関数
//使っていることを確認する
//引数：void
//戻：bool
//*****************************************************************************
bool Object3D::IsInUse()
{
	return bUse;
}

//*****************************************************************************
//GetType関数
//種類を戻す
//引数：void
//戻：int
//*****************************************************************************
int Object3D::GetType()
{
	return nObjType;
}

//*****************************************************************************
//GetHitBox関数
//ヒットエフェクトを戻す
//引数：void
//戻：Box
//*****************************************************************************
Box Object3D::GetHitbox()
{
	return { Hitbox.PositionX + Position.x,Hitbox.PositionY + Position.y,Hitbox.PositionZ + Position.z, Hitbox.SizeX, Hitbox.SizeY, Hitbox.SizeZ };
}

//*****************************************************************************
//SetHitbox関数
//ヒットエフェクトを設定する
//引数：Box
//戻：void
//*****************************************************************************
void Object3D::SetHitbox(Box newHitbox)
{
	Hitbox = newHitbox;
}

//*****************************************************************************
//SetAnimation関数
//オブジェクトのモデル
//のアニメーションを設定する
//引数：Box
//戻：void
//*****************************************************************************
void Object3D::SetAnimation(int anim)
{
	Model->SwitchAnimation(anim, 1,1.0f);
}

//*****************************************************************************
//SetAnimation関数
//オブジェクトのモデル
//のアニメーションを設定する
//引数：Box
//戻：void
//*****************************************************************************
void Object3D::SetAnimation(int anim, float fSpeed)
{
	Model->SwitchAnimation(anim, 1, fSpeed);
}

//*****************************************************************************
//MoveToPos関数
//自動で動くことです
//引数：void
//戻：void
//*****************************************************************************
bool Object3D::MoveToPos(float f_Speed, XMFLOAT3 vDestination)
{
	if (Position.x < vDestination.x)
	{
		Position.x += f_Speed;
		if (Position.x > vDestination.x)
			Position.x = vDestination.x;
	}
	else if (Position.x > vDestination.x) {
		Position.x -= f_Speed;
		if (Position.x < vDestination.x)
			Position.x = vDestination.x;
	}

	if (Position.y < vDestination.y)
	{
		Position.y += f_Speed;
		if (Position.y > vDestination.y)
			Position.y = vDestination.y;
	}
	else if (Position.y > vDestination.y) {
		Position.y -= f_Speed;
		if (Position.y < vDestination.y)
			Position.y = vDestination.y;
	}

	if (Position.z < vDestination.z)
	{
		Position.z += f_Speed;
		if (Position.z > vDestination.z)
			Position.z = vDestination.z;
	}
	else if (Position.z > vDestination.z) {
		Position.z -= f_Speed;
		if (Position.z < vDestination.z)
			Position.z = vDestination.z;
	}
	return CompareXmfloat3(Position, vDestination);
}

//*****************************************************************************
//SetMovement関数
//自動の動き方を設定する
//引数：XMFLOAT3, XMFLOAT3
//戻：void
//*****************************************************************************
void Object3D::SetMovement(XMFLOAT3 Start, XMFLOAT3 End, float Speed, int delay)
{
	x3MoveStartPos = Start;
	x3MoveEndPos = End;
	bGoToStartPos = true;
	fAutoSpeed = Speed;
	nDelayFramesBetweenStops = delay;
	bMoveable = true;
}

//*****************************************************************************
//IsMoveableObject関数
//自動に動けるオブジェクトを設定する
//引数：void
//戻：bool
//*****************************************************************************
bool Object3D::IsMoveableObject()
{
	return bMoveable;
}

//*****************************************************************************
//GetMoveStartPosition関数
//最初の拠点を戻す
//引数：void
//戻：XMFLOAT3
//*****************************************************************************
XMFLOAT3 Object3D::GetMoveStartPosition()
{
	return x3MoveStartPos;
}

//*****************************************************************************
//GetMoveEndPosition関数
//自動に動けるオブジェクトの最後の拠点を戻す
//引数：void
//戻：XMFLOAT3
//*****************************************************************************
XMFLOAT3 Object3D::GetMoveEndPosition()
{
	return x3MoveEndPos;
}

//*****************************************************************************
//GetForward関数
//フォーワードベクトルを戻す
//引数：void
//戻：XMFLOAT3
//*****************************************************************************
XMFLOAT3 Object3D::GetForward()
{
	return GetForwardVector(Rotation);
}

//*****************************************************************************
//GetModelForward関数
//モデルのフォーワードベクトルを戻す
//引数：void
//戻：XMFLOAT3
//*****************************************************************************
XMFLOAT3 Object3D::GetModelForward()
{
	if (!Model)
		return{ 0,0,0 };
	return GetForwardVector(Model->GetRotation());
}

//*****************************************************************************
//MoveTowardPos関数
//終点に動くこと
//引数：XMFLOAT3, float
//戻：bool
//*****************************************************************************
bool Object3D::MoveTowardPos(XMFLOAT3 Destination, float Speed)
{
	if (Position.x < Destination.x)
	{
		Position.x += Speed;
		if (Position.x > Destination.x)
			Position.x = Destination.x;
	}

	if (Position.x > Destination.x)
	{
		Position.x -= Speed;
		if (Position.x < Destination.x)
			Position.x = Destination.x;
	}

	if (Position.y < Destination.y)
	{
		Position.y += Speed;
		if (Position.y > Destination.y)
			Position.y = Destination.y;
	}

	if (Position.y > Destination.y)
	{
		Position.y -= Speed;
		if (Position.y < Destination.y)
			Position.y = Destination.y;
	}

	if (Position.z < Destination.z)
	{
		Position.z += Speed;
		if (Position.z > Destination.z)
			Position.z = Destination.z;
	}

	if (Position.z > Destination.z)
	{
		Position.z -= Speed;
		if (Position.z < Destination.z)
			Position.z = Destination.z;
	}
	return Position.z == Destination.z && Position.y == Destination.y && Position.x == Destination.x;
}

bool Object3D::MoveTowardPosXY(XMFLOAT2 Destination, float Speed)
{
	if (Position.x < Destination.x)
	{
		Position.x += Speed;
		if (Position.x > Destination.x)
			Position.x = Destination.x;
	}

	if (Position.x > Destination.x)
	{
		Position.x -= Speed;
		if (Position.x < Destination.x)
			Position.x = Destination.x;
	}

	if (Position.y < Destination.y)
	{
		Position.y += Speed;
		if (Position.y > Destination.y)
			Position.y = Destination.y;
	}

	if (Position.y > Destination.y)
	{
		Position.y -= Speed;
		if (Position.y < Destination.y)
			Position.y = Destination.y;
	}

	return Position.y == Destination.y && Position.x == Destination.x;
}

bool Object3D::MoveTowardPosXZ(XMFLOAT2 Destination, float Speed)
{
	if (Position.x < Destination.x)
	{
		Position.x += Speed;
		if (Position.x > Destination.x)
			Position.x = Destination.x;
	}

	if (Position.x > Destination.x)
	{
		Position.x -= Speed;
		if (Position.x < Destination.x)
			Position.x = Destination.x;
	}

	if (Position.z < Destination.y)
	{
		Position.z += Speed;
		if (Position.z > Destination.y)
			Position.z = Destination.y;
	}

	if (Position.z > Destination.y)
	{
		Position.z -= Speed;
		if (Position.z < Destination.y)
			Position.z = Destination.y;
	}
	return Position.z == Destination.y && Position.x == Destination.x;
}

//*****************************************************************************
//PauseObject関数
//オブジェクトをポースする
//引数：int
//戻：void
//*****************************************************************************
void Object3D::PauseObject(int pauseFrames)
{
	nPauseFrames = pauseFrames;
}

//*****************************************************************************
//GetMoveSpeed関数
//作れるオブジェクトの速さを戻す
//引数：void
//戻：int
//*****************************************************************************
float Object3D::GetMoveSpeed()
{
	return fAutoSpeed;
}

//*****************************************************************************
//GetDelayFrames関数
//作れるオブジェクトのディレイを戻す
//引数：void
//戻：int
//*****************************************************************************
int Object3D::GetDelayFrames()
{
	return nDelayFramesBetweenStops;
}

///////////////
//リスト管理//
/////////////

//*****************************************************************************
// コンストラクタ関数
//*****************************************************************************
Go_List::Go_List()
{
	HeadNode = nullptr;
	nObjectCount = 0;
}

Go_List::~Go_List()
{
	End();
}

//*****************************************************************************
//GetNumberOfObjects関数
//リストのオブジェクトの数を戻す
//引数：void
//戻：int
//*****************************************************************************
int Go_List::GetNumberOfObjects()
{
	return nObjectCount;
}

//*****************************************************************************
//CheckCollision関数
//当たり判定を確認して、オブジェクトのアドレスを戻す
//引数：Box
//戻：GameObject3D*
//*****************************************************************************
Object3D * Go_List::CheckCollision(Box hb)
{
	if (HeadNode == nullptr)
		return nullptr;
	go_node* pPositionList = HeadNode;
	while (true) {
		if (pPositionList == nullptr)
			break;
		if (pPositionList->Object != nullptr)
		{
			if (IsInCollision3D(pPositionList->Object->GetHitbox(), hb))
				return pPositionList->Object;
		}
		pPositionList = pPositionList->next;
	}
	return nullptr;
}

//*****************************************************************************
//CheckCollision関数
//当たり判定を確認して、オブジェクトのアドレスを戻す
//引数：Box, bool
//戻：GameObject3D*
//*****************************************************************************
Object3D * Go_List::CheckCollision(Box hb, bool bIgnoreUnused)
{
	if (HeadNode == nullptr)
		return nullptr;
	go_node* pPositionList = HeadNode;
	while (true) {
		if (pPositionList == nullptr)
			break;
		if (pPositionList->Object != nullptr)
		{
			if (bIgnoreUnused && pPositionList->Object->IsInUse()) {
				if (IsInCollision3D(pPositionList->Object->GetHitbox(), hb))
					return pPositionList->Object;
			}
		}
		pPositionList = pPositionList->next;
	}
	return nullptr;
}

Object3D * Go_List::AddEnemy(XMFLOAT3 newPosition)
{
	return  AddEnemy(newPosition, false, { 0,0,0 }, { 0,0,0 }, 0, 0);
}

Object3D * Go_List::AddEnemy(XMFLOAT3 newPosition, bool Moveable, XMFLOAT3 Start, XMFLOAT3 End, float Speed, int DelayFrames)
{
	go_node* pPositionList = HeadNode;
	if (HeadNode != nullptr) {
		while (pPositionList->next != nullptr) {
			pPositionList = pPositionList->next;
		}
		go_node* pWorkList = new go_node();
		pWorkList->Object = new Enemy();
		Enemy* enemy = (Enemy*)(pWorkList->Object);
		enemy->SetPosition(newPosition, true);
		if (Moveable)
			enemy->SetMovement(Start, End, Speed, DelayFrames);
		pWorkList->next = nullptr;
		pPositionList->next = pWorkList;
		nObjectCount++;
		return pWorkList->Object;
	}
	else {
		HeadNode = new go_node();
		HeadNode->Object = new Enemy();
		Enemy* enemy = (Enemy*)(HeadNode->Object);
		enemy->SetPosition(newPosition, true);
		if (Moveable)
			enemy->SetMovement(Start, End, Speed, DelayFrames);
		HeadNode->next = nullptr;
		nObjectCount++;
		return HeadNode->Object;
	}
	return nullptr;
}

Object3D * Go_List::AddField(XMFLOAT3 newPosition, XMFLOAT3 newScale, int TexturePath)
{
	return AddField(newPosition, newScale, TexturePath, false, { 0,0,0 }, { 0,0,0 }, 0, 0);
}

Object3D * Go_List::AddField(XMFLOAT3 newPosition, XMFLOAT3 newScale, int TexturePath, bool Moveable, XMFLOAT3 Start, XMFLOAT3 End, float Speed, int DelayFrames)
{
	go_node* pPositionList = HeadNode;
	if (HeadNode != nullptr) {
		while (pPositionList->next != nullptr) {
			pPositionList = pPositionList->next;
		}
		go_node* pWorkList = new go_node();
		pWorkList->Object = new Field3D(TexturePath);
		Field3D* field = (Field3D*)(pWorkList->Object);
		field->SetScale(newScale);
		field->SetPosition(newPosition);
		if (Moveable)
			field->SetMovement(Start, End, Speed, DelayFrames);
		pWorkList->next = nullptr;
		pPositionList->next = pWorkList;
		nObjectCount++;
		return pWorkList->Object;
	}
	else {
		HeadNode = new go_node();
		HeadNode->Object = new Field3D(TexturePath);
		Field3D* field = (Field3D*)(HeadNode->Object);
		field->SetScale(newScale);
		field->SetPosition(newPosition);
		if (Moveable)
			field->SetMovement(Start, End, Speed, DelayFrames);
		HeadNode->next = nullptr;
		nObjectCount++;
		return HeadNode->Object;
	}
}

//*****************************************************************************
//DeleteLastPosObject関数
//最後のオブジェクトを消す
//引数：void
//戻：void
//*****************************************************************************
void Go_List::DeleteLastPosObject()
{
	if (HeadNode == nullptr)
		return;
	go_node* pPositionList = HeadNode->next;
	go_node* PreviousPos = HeadNode;
	if (pPositionList == nullptr)
	{
		if (PreviousPos->Object) {
			delete(PreviousPos->Object);
			PreviousPos->Object = nullptr;
			delete(PreviousPos);
			HeadNode = PreviousPos = nullptr;
			return;
		}
	}
	while (true) {
		if (pPositionList->next == nullptr)
			break;
		pPositionList = pPositionList->next;
		PreviousPos = PreviousPos->next;
	}
	if (pPositionList->Object) {
		delete(pPositionList->Object);
		pPositionList->Object = nullptr;
		delete(pPositionList);
		pPositionList = nullptr;
		PreviousPos->next = nullptr;
	}
	return;
}

//*****************************************************************************
//DeleteObject関数
//オブジェクトを消す
//引数：GameObject3D*
//戻：void
//*****************************************************************************
void Go_List::DeleteObject(Object3D * pSearch)
{
	if (HeadNode == nullptr)
		return;
	if (HeadNode->Object == pSearch)
	{
		go_node* pPositionList = HeadNode;
		HeadNode = HeadNode->next;
		delete(pPositionList->Object);
		pPositionList->Object = nullptr;
		delete(pPositionList);
		pPositionList = nullptr;
		return;
	}
	if (HeadNode->next->Object == pSearch)
	{
		go_node* pPositionList = HeadNode->next;
		HeadNode->next = HeadNode->next->next;
		delete(pPositionList->Object);
		pPositionList->Object = nullptr;
		delete(pPositionList);
		pPositionList = nullptr;
		return;
	}
	go_node* pPositionList = HeadNode->next;
	go_node* PreviousPos = HeadNode;
	if (pPositionList == nullptr)
		return;
	while (true) {
		if (pPositionList->next == nullptr)
			return;
		if (pPositionList->Object == pSearch)
			break;
		pPositionList = pPositionList->next;
		PreviousPos = PreviousPos->next;
	}
	if (pPositionList->Object) {
		PreviousPos->next = pPositionList->next;
		delete(pPositionList->Object);
		pPositionList->Object = nullptr;
		delete(pPositionList);
		pPositionList = nullptr;
	}
	return;
}

//*****************************************************************************
//Update関数
//変更関数
//引数：void
//戻：void
//*****************************************************************************
void Go_List::Update()
{
	if (HeadNode == nullptr)
		return;
	go_node* pPositionList = HeadNode;
	while (true) {
		if (pPositionList == nullptr)
			break;
		if (pPositionList->Object != nullptr)
			pPositionList->Object->Update();
		pPositionList = pPositionList->next;
	}
}

//*****************************************************************************
//Draw関数
//レンダリング関数
//引数：void
//戻：void
//*****************************************************************************
void Go_List::Draw()
{
	if (HeadNode == nullptr)
		return;
	go_node* pPositionList = HeadNode;
	while (true) {

		if (pPositionList == nullptr)
			break;

		if (pPositionList->Object != nullptr)
			pPositionList->Object->Draw();
		pPositionList = pPositionList->next;
	}
}

//*****************************************************************************
//End関数
//終了関数
//引数：void
//戻：void
//*****************************************************************************
void Go_List::End()
{
	if (HeadNode == nullptr)
		return;
	go_node* pPositionList = HeadNode;
	go_node* pWork = nullptr;
	int Count = 0;
	while (true) {
		if (pPositionList == nullptr)
			break;
		if (pPositionList->Object != nullptr) {
			Count++;
			delete(pPositionList->Object);
			pPositionList->Object = nullptr;
		}
		pWork = pPositionList;
		pPositionList = pPositionList->next;
		if (pWork) {
			delete(pWork);
			pWork = nullptr;
		}
	}
	printf("GameObjects Deleted: %d\n", Count);
	nObjectCount = 0;
	HeadNode = nullptr;
}

