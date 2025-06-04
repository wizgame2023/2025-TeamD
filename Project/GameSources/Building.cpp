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

		ptrDraw->SetOwnShadowActive(true);

		auto shadowPtr = AddComponent<Shadowmap>();
		shadowPtr->SetLightHeight(150.0f);
		shadowPtr->SetViewWidth(64.0f);
		shadowPtr->SetViewHeight(64.0f);


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
		/*ptrDraw->SetMeshResource(L"DEFAULT_CUBE");
		ptrDraw->SetTextureResource(L"BUILDING");*/
		ptrDraw->SetMeshResource(L"OBJECT");
		ptrDraw->SetTextureResource(L"BUILD_TEX");
		Mat4x4 meshMat;
		meshMat.affineTransformation(
			Vec3(0.03f, 0.05f, 0.027f), //(.1f, .1f, .1f),
			Vec3(0.0f, 0.0f, 0.0f),
			Vec3(0.0f, XM_PI, 0.0f),
			Vec3(0.0f, -0.5f, 0.0f)
		);
		ptrDraw->SetMeshToTransformMatrix(meshMat);

		ptrDraw->SetOwnShadowActive(true);
	}
}
//end basecross
