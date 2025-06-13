/*!
@file Skybox.cpp
@brief キャラクターなど実体
*/

#include "stdafx.h"
#include "Project.h"
#include <string>

namespace basecross {
	// スタティックメンバ変数として初期化することで、GameStage.cppでのテクスチャ読み込みの際に流用している
	const std::map<std::wstring, Vec3> SkyBackGround::pairs = {
		{L"000_000_000", Vec3(0   ,  0,   +0.5f)},
		{L"000_090_000", Vec3(+0.5f, 0,    0)},
		{L"000_180_000", Vec3(0   ,  0,   -0.5f)},
		{L"000_270_000", Vec3(-0.5f, 0,    0)},
		{L"090_000_000", Vec3(0   , -0.5f, 0)},
		{L"270_000_000", Vec3(0   , +0.5f, 0)},
	};

	void SkyBackGround::OnCreate()
	{
		for (const auto& pair : pairs)
		{
			// プレーンクラスを元にスカイキューブを作成する
			auto plane = ObjectFactory::Create<Plane>(GetStage());

			auto planeDrawComp = plane->GetComponent<PTStaticDraw>(); // ライティングは掛けたくないので、PNTは使わない（ライティングで影がつくとキューブ形状がばれる）
			planeDrawComp->SetTextureResource(pair.first);
			planeDrawComp->SetDepthStencilState(DepthStencilState::None); // Zバッファの影響を受けなくする（最初に描画すれば、他のオブジェクトを遮らない）
			planeDrawComp->SetSamplerState(SamplerState::LinearClamp); // テクスチャの切れ目を目立たなくする（これをやらないとプレーンの境目に線が入ったように見えてしまう）

			auto planeTransComp = plane->GetComponent<Transform>();
			// テクスチャキーの文字列を角度に変換する
			float rotX = XMConvertToRadians(std::stof(pair.first.substr(0, 3))); // 0文字目から3文字切り出す
			float rotY = XMConvertToRadians(std::stof(pair.first.substr(4, 3))); // 4文字目から3文字切り出す
			float rotZ = XMConvertToRadians(std::stof(pair.first.substr(8, 3))); // 8文字目から3文字切り出す
			// 視野範囲の限界まで拡大する（Zバッファをオフにしているので不必要かと思うが、念のため）
			const float scale = 707.0f;
			planeTransComp->SetPosition(pair.second * scale);
			planeTransComp->SetRotation(rotX, rotY, rotZ);
			planeTransComp->SetScale(Vec3(scale));
			planeTransComp->SetParent(GetThis<GameObject>()); // スカイキューブを構成するプレーンをSkybox2オブジェクトの子にすることで座標の管理を簡略化できる

			m_planes.push_back(plane);
		}
	}


	void SkyBackGround::OnDraw()
	{
		// 管理しているすべての面を描画する
		for (auto& plane : m_planes)
		{
			plane->OnDraw();
		}
	}
}
//end basecross
