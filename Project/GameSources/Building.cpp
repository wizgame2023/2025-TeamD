/*!
@file Character.cpp
@brief キャラクターなど実体
*/

#include "stdafx.h"
#include "Project.h"

namespace basecross {

	Ground::Ground(const shared_ptr<Stage>& stage) : Object(stage){}
	Ground::~Ground(){}

	void Ground::OnCreate() {
		Object::OnCreate();
		auto ptrColl = AddComponent<CollisionObb>();
		ptrColl->SetDrawActive(GameManager::Instance()->IsDebug());//debug
		ptrColl->SetFixed(true);

		//描画設定
		auto ptrDraw = AddComponent<BcPNTStaticDraw>();
		ptrDraw->SetMeshResource(L"DEFAULT_CUBE");
		ptrDraw->SetTextureResource(L"GROUND");
	}

	LimitArea::LimitArea(const shared_ptr<Stage>& stage) : Object(stage) {}
	LimitArea::~LimitArea() {}

	void LimitArea::OnCreate() {
		Object::OnCreate();
		auto ptrColl = AddComponent<CollisionObb>();
		ptrColl->SetDrawActive(GameManager::Instance()->IsDebug());//debug
		ptrColl->SetFixed(true);

	}

	Building::Building(const shared_ptr<Stage>& stage) : Object(stage) {}
	Building::~Building() {}

	void Building::OnCreate() {
		Object::OnCreate();
		auto ptrColl = AddComponent<CollisionObb>();
		ptrColl->SetDrawActive(GameManager::Instance()->IsDebug());//debug
		ptrColl->SetFixed(true);

		//描画設定
		auto ptrDraw = AddComponent<BcPNTStaticDraw>();
		ptrDraw->SetMeshResource(L"DEFAULT_CUBE");
		ptrDraw->SetTextureResource(L"BUILDING");
	}
}
//end basecross
