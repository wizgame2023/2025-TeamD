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
	void GameStageS::CreateViewLight() {
		const Vec3 eye(0.0f, 5.0f, -10.0f);
		const Vec3 at(0.0f);
		auto PtrView = CreateView<SingleView>();
		//ビューのカメラの設定
		auto PtrCamera = ObjectFactory::Create<FollowCamera>();
		PtrView->SetCamera(PtrCamera);
		PtrCamera->SetEye(eye);
		PtrCamera->SetAt(at);
		//マルチライトの作成
		auto PtrMultiLight = CreateLight<MultiLight>();
		//デフォルトのライティングを指定
		PtrMultiLight->SetDefaultLighting();
	}

	void GameStageS::CreatePlayer()
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
			auto player = AddGameObject<Player>(v[0], v[1], v[2]);
			auto camera = static_pointer_cast<FollowCamera>(GetView()->GetTargetCamera());
			//見るもの
			camera->SetTarget(player->GetComponent<Transform>());
		}


	}
	void GameStageS::CreateCamera()
	{
	}		

	void GameStageS::CreateEnemy()
	{
	}

	void GameStageS::OnCreate() {
		try {
			//ビューとライトの作成
			CreateViewLight();
			//AddGameObject<FixedBox>();
			//CreateWall();
			CreatePlayer();
			auto builder = AddGameObject<StageBuilder>(L"levelMap.csv");
			builder->Register<FixedBox>(L"cube");

			builder->LoadCsv();
		}
		catch (...) {
			throw;
		}
	}
	void GameStageS::CreateWall() {
		AddGameObject<Wall>();
	}

}
//end basecross
