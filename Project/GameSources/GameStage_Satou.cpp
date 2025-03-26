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
		auto PtrCamera = ObjectFactory::Create<FollowCamera>(GetThis<GameStageS>());
		PtrView->SetCamera(PtrCamera);
		PtrCamera->SetEye(eye);
		PtrCamera->SetAt(at);
		//マルチライトの作成
		auto PtrMultiLight = CreateLight<MultiLight>();
		//デフォルトのライティングを指定
		PtrMultiLight->SetDefaultLighting();
	}

	/// <summary>
	/// リソースの作成
	/// </summary>
	void GameStageS::CreateResource() {
		auto& app = App::GetApp();
		auto mediaPath = app->GetDataDirWString();
		wstring uiPath = mediaPath + L"UI/";
		wstring texPath = mediaPath + L"Textures/";
		wstring modelPath = mediaPath + L"Models/";

		app->RegisterTexture(L"POSE_TITLE", uiPath + L"BackToTitle.png");
		app->RegisterTexture(L"POSE_TITLE_SELECTED", uiPath + L"BackToTitle_Selected.png");
		app->RegisterTexture(L"POSE_ENDGAME", uiPath + L"NextStage.png");
		app->RegisterTexture(L"POSE_ENDGAME_SELECTED", uiPath + L"NextStage_Selected.png");
		app->RegisterTexture(L"POSE_START", uiPath + L"Restart.png");
		app->RegisterTexture(L"POSE_START_SELECTED", uiPath + L"Restart_Selected.png");
		app->RegisterTexture(L"POSE_SOUND", uiPath + L"Select.png");
		app->RegisterTexture(L"POSE_SOUND_SELECTED", uiPath + L"Select_Selected.png");
		app->RegisterTexture(L"01", texPath + L"Black0.1.png");

		app->RegisterTexture(L"SEARCH_RANGE", texPath + L"SearchRange.png");

		auto modelMesh = MeshResource::CreateBoneModelMesh(modelPath, L"HR.bmf");
		app->RegisterResource(L"PLAYER", modelMesh);
	}
	/// <summary>
	/// ポーズメニューの作成
	/// </summary>
	void GameStageS::CreatePose() {
		//タイトル
		ButtonManager::Create(GetThis<Stage>(), L"POSE", L"POSE_TITLE", L"POSE_TITLE_SELECTED", Vec3(0.0f, 150.0f, 0.0f), Vec2(200, 50),
			[](shared_ptr<Stage> stage) {
				stage->PostEvent(0.0f, stage, App::GetApp()->GetScene<Scene>(), L"ToTitleStage");
			});
		//やめる
		ButtonManager::Create(GetThis<Stage>(), L"POSE", L"POSE_ENDGAME", L"POSE_ENDGAME_SELECTED", Vec3(0.0f, 50.0f, 0.0f), Vec2(200, 50),
			[](shared_ptr<Stage> stage) {
				stage->PostEvent(0.0f, stage, App::GetApp()->GetScene<Scene>(), L"ToGameStage");
			});
		//再開
		ButtonManager::Create(GetThis<Stage>(), L"POSE", L"POSE_START", L"POSE_START_SELECTED", Vec3(0.0f, -50.0f, 0.0f), Vec2(200, 50),
			[](shared_ptr<Stage> stage) {
				auto currentStage = static_pointer_cast<GameStageM>(stage);
				currentStage->ClosePose();
			});
		//サウンド
		ButtonManager::Create(GetThis<Stage>(), L"POSE", L"POSE_SOUND", L"POSE_SOUND_SELECTED", Vec3(0.0f, -150.0f, 0.0f), Vec2(200, 50),
			[](shared_ptr<Stage> stage) {
				auto currentStage = static_pointer_cast<GameStageM>(stage);
				ButtonManager::instance->Close(L"POSE");
				ButtonManager::instance->OpenAndUse(L"SOUND_TEST");
			});

		ButtonManager::instance->SetInput(L"POSE", InputData(StickMode::LY, 1, 0.1f));
		ButtonManager::instance->AddAcceptButton(L"POSE", XINPUT_GAMEPAD_A);
		ClosePose();
	}
	/// <summary>
	/// サウンドテストメニューの作成
	/// </summary>
	void GameStageS::CreateSoundTest() {
		//SE
		ButtonManager::Create(GetThis<Stage>(), L"SOUND_TEST", L"POSE_TITLE", L"POSE_TITLE_SELECTED", Vec3(0.0f, 0.0f, 0.0f), Vec2(200, 50),
			[](shared_ptr<Stage> stage) {
				WORD press = ButtonManager::instance->GetPressedAccept(L"SOUND_TEST");
				if (press & XINPUT_GAMEPAD_DPAD_UP) {
					SoundManager::Instance().SEVolumeUp(0.1f);
				}
				else if (press & XINPUT_GAMEPAD_DPAD_DOWN) {
					SoundManager::Instance().SEVolumeDown(0.1f);
				}
			});
		//BGM
		ButtonManager::Create(GetThis<Stage>(), L"SOUND_TEST", L"POSE_ENDGAME", L"POSE_ENDGAME_SELECTED", Vec3(0.0f, -50.0f, 0.0f), Vec2(200, 50),
			[](shared_ptr<Stage> stage) {
				WORD press = ButtonManager::instance->GetPressedAccept(L"SOUND_TEST");
				if (press & XINPUT_GAMEPAD_DPAD_UP) {
					SoundManager::Instance().SEVolumeUp(0.1f);
				}
				else if (press & XINPUT_GAMEPAD_DPAD_DOWN) {
					SoundManager::Instance().SEVolumeDown(0.1f);
				}
			});

		ButtonManager::instance->SetInput(L"SOUND_TEST", InputData(StickMode::LY, 1, 0.1f));
		ButtonManager::instance->AddAcceptButton(L"SOUND_TEST", XINPUT_GAMEPAD_DPAD_UP);
		ButtonManager::instance->AddAcceptButton(L"SOUND_TEST", XINPUT_GAMEPAD_DPAD_DOWN);

		ButtonManager::instance->Close(L"SOUND_TEST");
	}
	/// <summary>
	/// ポーズ画面を閉じる
	/// </summary>
	void GameStageS::ClosePose() {
		m_IsPose = false;
		ButtonManager::instance->Close(L"POSE");
	}
	/// <summary>
	/// ポーズ画面を開く
	/// </summary>
	void GameStageS::OpenPose() {
		m_IsPose = true;
		ButtonManager::instance->Close(L"SOUND_TEST");
		ButtonManager::instance->OpenAndUse(L"POSE");
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

	/// <summary>
	/// 敵作成
	/// </summary>
	void GameStageS::CreateEnemy()
	{
		//配列の初期化
		vector< vector<Vec3> > vec = {
			{
				Vec3(20.0f, 1.0f, 0.0f),
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

	void GameStageS::CreateBossEnemy()
	{
		vector< vector<Vec3> > vec = {
			{
				Vec3(0.0f,3.5f, 0.0f),
				Vec3(0.0f, 0.0f, 0.0f),
				Vec3(1.5f, 1.5f, 1.5f)
			},
		};
		auto& player = GetSharedGameObject<Player>(L"Player", false);
		//オブジェクトの作成
		for (auto v : vec) {
			auto bossEnemy = AddGameObject<BossEnemy>(v[0], v[2]);
			SetSharedGameObject(L"BossBody", bossEnemy);
			bossEnemy->SetIntruder(player);
		}
	}

	void GameStageS::RegisterObjects() {
		auto& builder = AddGameObject<StageBuilder>(L"levelMap.csv");
		builder->Register<FixedBox>(L"cube");
		builder->Register<Player>(L"player");
		builder->Register<Mob>(L"mob");

		builder->LoadCsv();
	}

	void GameStageS::SetAllGameObjectActive(bool flag) {
		for (auto& obj : GetGameObjectVec()) {
			if (!obj->FindTag(L"Button") && !obj->FindTag(L"Manager")) {
				obj->SetUpdateActive(flag);
			}
		}
	}


	void GameStageS::OnCreate() {
		try {
			GameStage::OnCreate();
		}
		catch (...) {
			throw;
		}
	}

	void GameStageS::OnUpdate() {
		auto& app = App::GetApp();

		auto& device = app->GetInputDevice().GetControlerVec()[0];
		GameStage::OnUpdate();
	}
}
//end basecross
