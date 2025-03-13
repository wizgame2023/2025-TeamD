/*!
@file Character.cpp
@brief キャラクターなど実体
*/

#include "stdafx.h"
#include "Project.h"
#include "Character.h"

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
		GameObject(stage)
	{
	}
	FixedBox::~FixedBox(){}

	void FixedBox::OnCreate()
	{
		//初期位置の設定
		auto ptr = AddComponent<Transform>();
		ptr->SetPosition(Vec3(0));
		ptr->SetRotation(Vec3(0));
		ptr->SetScale(Vec3(50.0f,0.5f,50.0f));

		//CollisionSphere衝突判定を付ける
		auto ptrColl = AddComponent<CollisionObb>();
		ptrColl->SetDrawActive(true);//debug
		ptrColl->SetFixed(true);

		//描画設定
		auto ptrDraw = AddComponent<BcPNTStaticDraw>();
		ptrDraw->SetMeshResource(L"DEFAULT_CUBE");


	}

	Wall::Wall(const shared_ptr<Stage>& stage) :
		GameObject(stage)
	{
	}
	Wall::~Wall() {}

	void Wall::OnCreate()
	{
		//初期位置の設定
		auto ptr = AddComponent<Transform>();
		ptr->SetPosition(Vec3(0.0f, 0.0f,5.0f));
		ptr->SetRotation(Vec3(0));
		ptr->SetScale(Vec3(5.0f, 3.0f, 0.5f));

		//CollisionSphere衝突判定を付ける
		auto ptrColl = AddComponent<CollisionObb>();
		ptrColl->SetDrawActive(true);//debug
		ptrColl->SetFixed(true);
		//描画設定
		auto ptrDraw = AddComponent<BcPNTStaticDraw>();
		ptrDraw->SetMeshResource(L"DEFAULT_CUBE");


	}

}
//end basecross
