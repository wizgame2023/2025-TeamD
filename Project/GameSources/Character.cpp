/*!
@file Character.cpp
@brief キャラクターなど実体
*/

#include "stdafx.h"
#include "Project.h"

namespace basecross{

	void Character::OnCreate()
	{
		GameObject::OnCreate();
	}
	void Character::OnUpdate()
	{
		GameObject::OnUpdate();
	}
	void Character::OnDraw()
	{
		GameObject::OnDraw();
	}


	FixedBox::FixedBox(const shared_ptr<Stage>& stage):
		Object(stage)
	{
	}
	FixedBox::~FixedBox(){}

	void FixedBox::OnCreate()
	{
		//ここでm_Transformの中身取得してくれる
		Object::OnCreate();
		//操作系
		SetPosition(Vec3());
		SetScale(Vec3(1.0f));
		SetRotation(Vec3());


		Wicth_FixedBox = true;
		
		//CollisionSphere衝突判定を付ける
		auto ptrColl = AddComponent<CollisionObb>();
		ptrColl->SetDrawActive(true);//debug
		ptrColl->SetFixed(Wicth_FixedBox);

		//描画設定
		auto ptrDraw = AddComponent<BcPNTStaticDraw>();
		ptrDraw->SetMeshResource(L"DEFAULT_CUBE");
	}
	shared_ptr<Object> FixedBox::Create() {
		return GetStage()->AddGameObject<FixedBox>();
	}

	Wall::Wall(const shared_ptr<Stage>& stage) :
		GameObject(stage)
	{
	}
	Wall::~Wall() {}

	void Wall::OnCreate()
	{
		Wicth_Wall = true;
		//初期位置の設定
		auto ptr = AddComponent<Transform>();
		ptr->SetPosition(Vec3(0.0f, 0.0f,5.0f));
		ptr->SetRotation(Vec3(0));
		ptr->SetScale(Vec3(5.0f, 3.0f, 0.5f));

		//CollisionSphere衝突判定を付ける
		auto ptrColl = AddComponent<CollisionObb>();
		ptrColl->SetDrawActive(true);//debug
		ptrColl->SetFixed(Wicth_Wall);
		//描画設定
		auto ptrDraw = AddComponent<BcPNTStaticDraw>();
		ptrDraw->SetMeshResource(L"DEFAULT_CUBE");


	}

}
//end basecross
