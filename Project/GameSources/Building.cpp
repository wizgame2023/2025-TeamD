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
		ptrColl->SetDrawActive(true);//debug
		ptrColl->SetFixed(true);

		//描画設定
		auto ptrDraw = AddComponent<BcPNTStaticDraw>();
		ptrDraw->SetMeshResource(L"DEFAULT_CUBE");
		ptrDraw->SetTextureResource(L"GROUND");
		//auto ptrDraw = AddComponent<BcPNTStaticModelDraw>();
		//ptrDraw->SetMeshResource(L"GROUND");
		//Mat4x4 meshMat;
		//meshMat.affineTransformation(
		//	Vec3(0.1f, 0.1f, 0.1f), //サイズ
		//	Vec3(0.0f, 0.0f, 0.0f), //回転軸
		//	Vec3(0.0f, 0.0f, 0.0f), //回転
		//	Vec3(0.01f, 0.45f, -0.04f) //ポジション
		//);
		//ptrDraw->SetMeshToTransformMatrix(meshMat);
	}
}
//end basecross
