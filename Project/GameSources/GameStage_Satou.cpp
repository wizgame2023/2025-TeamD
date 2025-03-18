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
		shared_ptr<GameObject> player;
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
			player = AddGameObject<Player>(v[0], v[1], v[2]);
		}

		SetSharedGameObject(L"Player", player);

	}

	void GameStageS::CreateResource() {
		auto& app = App::GetApp();
		auto mediaPath = app->GetDataDirWString();
		wstring uiPath = mediaPath + L"UI/";
		wstring texPath = mediaPath + L"Textures/";

		app->RegisterTexture(L"POSE_TITLE", uiPath + L"BackToTitle.png");
		app->RegisterTexture(L"POSE_TITLE_SELECTED", uiPath + L"BackToTitle_Selected.png");
		app->RegisterTexture(L"POSE_ENDGAME", uiPath + L"NextStage.png");
		app->RegisterTexture(L"POSE_ENDGAME_SELECTED", uiPath + L"NextStage_Selected.png");
		app->RegisterTexture(L"POSE_START", uiPath + L"Restart.png");
		app->RegisterTexture(L"POSE_START_SELECTED", uiPath + L"Restart_Selected.png");
		app->RegisterTexture(L"POSE_SOUND", uiPath + L"Select.png");
		app->RegisterTexture(L"POSE_SOUND_SELECTED", uiPath + L"Select_Selected.png");

		app->RegisterTexture(L"SEARCH_RANGE", texPath + L"SearchRange.png");

	}

	void GameStageS::CreateEnemy()
	{
		//配列の初期化
		vector< vector<Vec3> > vec = {
			{
				Vec3(-5.0f, 1.0f, 0.0f),
				Vec3(0.0f, 0.0f, 0.0f),
				Vec3(1.0f, 1.0f, 1.0f)
			},
		};
		auto& player = GetSharedGameObject<Player>(L"Player", false);
		//オブジェクトの作成
		for (auto v : vec) {
			auto mob = AddGameObject<Mob>(v[0], v[2]);
			mob->SetIntruder(player);
		}

	}
	void GameStageS::RegisterObjects() {
		auto& builder = AddGameObject<StageBuilder>(L"level.csv", 1.0f);
		builder->Register<FixedBox>(L"cube");
		builder->Register<Player>(L"player");
		builder->Register<Mob>(L"mob");

		builder->LoadCsv();
	}


	void GameStageS::OnCreate() {
		try {
			//ビューとライトの作成
			CreateViewLight();
			CreateSharedObjectGroup(L"BulletGroup");
			CreateResource();
			//CreatePlayer();
			//CreateEnemy();
			RegisterObjects();
			//auto builder = AddGameObject<StageBuilder>(L"levelMap.csv");
			//builder->Register<FixedBox>(L"cube");

			//builder->LoadCsv();
			auto player = GetSharedGameObject<Player>(L"Player", false);
			if (player != nullptr) {
				auto camera = static_pointer_cast<FollowCamera>(GetView()->GetTargetCamera());
				if (camera != nullptr) {
					camera->SetTarget(player->GetComponent<Transform>());
				}
			}

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
