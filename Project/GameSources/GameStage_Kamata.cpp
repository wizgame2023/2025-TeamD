/*!
@file GameStage.cpp
@brief ゲームステージ実体
*/

#include "stdafx.h"
#include "Project.h"

namespace basecross {

	//--------------------------------------------------------------------------------------
	//	ゲームステージクラス実体
	//--------------------------------------------------------------------------------------
	void GameStageK::CreateViewLight() {
		const Vec3 eye(0.0f, 5.0f, -5.0f);
		const Vec3 at(0.0f);
		auto PtrView = CreateView<SingleView>();
		//ビューのカメラの設定
		auto PtrCamera = ObjectFactory::Create<Camera>();
		PtrView->SetCamera(PtrCamera);
		PtrCamera->SetEye(eye);
		PtrCamera->SetAt(at);
		//マルチライトの作成
		auto PtrMultiLight = CreateLight<MultiLight>();
		//デフォルトのライティングを指定
		PtrMultiLight->SetDefaultLighting();
	}

	void GameStageK::CreatePlayer()
	{
		//配列の初期化
		vector< vector<Vec3> > vec = {
			{
				Vec3(0.0f, 1.0f, 0.0f),
				Vec3(0.0f, 0.0f, 0.0f),
				Vec3(1.0f, 1.0f, 1.0f)
			},
		};
		//オブジェクトの作成
		for (auto v : vec) {
			AddGameObject<Player>(v[0], v[1], v[2]);
		}

	}

	void GameStageK::CreateEnemy()
	{
	}

	void GameStageK::OnCreate() {
		try {
			//ビューとライトの作成
			CreateViewLight();
			AddGameObject<FixedBox>();
			CreatePlayer();
		}
		catch (...) {
			throw;
		}
	}

}
//end basecross
