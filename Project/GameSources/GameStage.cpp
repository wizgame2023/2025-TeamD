/*!
@file GameStage.cpp
@brief
*/

#include "stdafx.h"
#include "Project.h"

namespace basecross {

	//--------------------------------------------------------------------------------------
	//	ゲームステージクラス実体
	//--------------------------------------------------------------------------------------
	void GameStage::CreateViewLight() {
		const Vec3 eye(0.0f, 5.0f, -5.0f);
		const Vec3 at(0.0f);
		auto PtrView = CreateView<SingleView>();

		//ビューのカメラの設定
		auto PtrCamera = ObjectFactory::Create<FollowCamera>(GetThis<GameStage>());
		PtrView->SetCamera(PtrCamera);
		PtrCamera->SetEye(eye);
		PtrCamera->SetAt(at);
		//マルチライトの作成
		auto PtrMultiLight = CreateLight<MultiLight>();
		//デフォルトのライティングを指定
		PtrMultiLight->SetDefaultLighting();
	}
	void GameStage::CreateResource() {
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
		app->RegisterTexture(L"SE_VOLUME", uiPath + L"SEVolume.png");
		app->RegisterTexture(L"SE_VOLUME_SELECTED", uiPath + L"SEVolume_Selected.png");
		app->RegisterTexture(L"BGM_VOLUME", uiPath + L"BGMVolume.png");
		app->RegisterTexture(L"BGM_VOLUME_SELECTED", uiPath + L"BGMVolume_Selected.png");
		app->RegisterTexture(L"01", texPath + L"Black0.1.png");
		app->RegisterTexture(L"NUMBER", uiPath + L"TimerNum.png");
		app->RegisterTexture(L"ACTION", uiPath + L"ActionButton.png");

		app->RegisterTexture(L"SEARCH_RANGE", texPath + L"SearchRange.png");
	}
	/// <summary>
	/// リソースの作成
	/// </summary>
	void GameStage::RegisterObjects() {
		auto& builder = AddGameObject<StageBuilder>(L"level.csv", 1.0f);
		builder->Register<FixedBox>(L"cube");
		builder->Register<Player>(L"player");
		builder->Register<RootPointer>(L"pointer");
		builder->Register<Mob>(L"mob");

		builder->LoadCsv();
		
	}
	/// </summary>
	/// ポーズメニューの作成
	/// <summary>
	void GameStage::CreatePose() {
		//タイトル
		ButtonManager::Create(GetThis<Stage>(), L"POSE", L"POSE_TITLE", L"POSE_TITLE_SELECTED", Vec3(0.0f, 150.0f, 0.0f), Vec2(200, 50),
			[](shared_ptr<Stage> stage) {
				stage->PostEvent(0.0f, stage, App::GetApp()->GetScene<Scene>(), L"ToTitleStage");
			});
		//やめる
		ButtonManager::Create(GetThis<Stage>(), L"POSE", L"POSE_ENDGAME", L"POSE_ENDGAME_SELECTED", Vec3(0.0f, 50.0f, 0.0f), Vec2(200, 50),
			[](shared_ptr<Stage> stage) {
				stage->PostEvent(0.0f, stage, App::GetApp()->GetScene<Scene>(), L"ToTitleStage");
			});
		//再開
		ButtonManager::Create(GetThis<Stage>(), L"POSE", L"POSE_START", L"POSE_START_SELECTED", Vec3(0.0f, -50.0f, 0.0f), Vec2(200, 50),
			[](shared_ptr<Stage> stage) {
				auto currentStage = static_pointer_cast<GameStage>(stage);
				currentStage->ClosePose();
			});
		//サウンドテスト
		ButtonManager::Create(GetThis<Stage>(), L"POSE", L"POSE_SOUND", L"POSE_SOUND_SELECTED", Vec3(0.0f, -150.0f, 0.0f), Vec2(200, 50),
			[](shared_ptr<Stage> stage) {
				auto currentStage = static_pointer_cast<GameStage>(stage);
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
	void GameStage::CreateSoundTest() {
		//SE
		ButtonManager::Create(GetThis<Stage>(), L"SOUND_TEST", L"SE_VOLUME", L"SE_VOLUME_SELECTED", Vec3(0.0f, 0.0f, 0.0f), Vec2(200, 50),
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
		ButtonManager::Create(GetThis<Stage>(), L"SOUND_TEST", L"BGM_VOLUME", L"BGM_VOLUME_SELECTED", Vec3(0.0f, -50.0f, 0.0f), Vec2(200, 50),
			[](shared_ptr<Stage> stage) {
				WORD press = ButtonManager::instance->GetPressedAccept(L"SOUND_TEST");
				if (press & XINPUT_GAMEPAD_DPAD_UP) {
					SoundManager::Instance().BGMVolumeUp(0.1f);
				}
				else if (press & XINPUT_GAMEPAD_DPAD_DOWN) {
					SoundManager::Instance().BGMVolumeDown(0.1f);
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
	void GameStage::ClosePose() {
		m_IsPose = false;
		ButtonManager::instance->Close(L"POSE");
	}
	/// <summary>
	/// ポーズ画面を開く
	/// </summary>
	void GameStage::OpenPose() {
		m_IsPose = true;
		ButtonManager::instance->Close(L"SOUND_TEST");
		ButtonManager::instance->OpenAndUse(L"POSE");
	}
	/// <summary>
	/// オブジェクトの描画をONOFF
	/// </summary>
	/// <param name="flag">描画ONOFF</param>
	void GameStage::SetAllGameObjectActive(bool flag) {
		for (auto& obj : GetGameObjectVec()) {
			if (!obj->FindTag(L"Button") && !obj->FindTag(L"Manager")) {
				obj->SetUpdateActive(flag);
			}
		}
	}
	void GameStage::CreateBossEnemy()
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
	void GameStage::OnCreate() {
		try {
			CreateSharedObjectGroup(L"BulletGroup");
			CreateSharedObjectGroup(L"EnemyGroup");
			CreateSharedObjectGroup(L"PointerGroup");

			//ビューとライトの作成
			CreateViewLight();
			CreateResource();
			RegisterObjects();
			AddGameObject<ButtonManager>();

			CreatePose();
			CreateSoundTest();
			ButtonManager::instance->CloseAll();

			auto player = GetSharedGameObject<Player>(L"Player", false);
			if (player != nullptr) {
				auto camera = static_pointer_cast<FollowCamera>(GetView()->GetTargetCamera());
				if (camera != nullptr) {
					camera->SetTarget(player->GetComponent<Transform>());
				}
			}

			m_ProtoHpNumber = AddGameObject<NumberSprite>(L"NUMBER", Vec3(-631.0f, 393.0f, 0.0f), Vec2(109.0f, 96.0f), 3);
			m_ProtoScoreNumber = AddGameObject<NumberSprite>(L"NUMBER", Vec3(-631.0f, 297.0f, 0.0f), Vec2(109.0f, 96.0f), 3);
			auto sprite = AddGameObject<Sprite>(L"ACTION", Vec3(423.0f, -297.0f, 0.0f), Vec2(72.0f));
			sprite->SetDiffuse(Col4(1, 0, 0, 1));
			sprite = AddGameObject<Sprite>(L"ACTION", Vec3(347.0f, -228.0f, 0.0f), Vec2(72.0f));
			sprite->SetDiffuse(Col4(1, 0, 0, 1));
			sprite = AddGameObject<Sprite>(L"ACTION", Vec3(499.0f, -228.0f, 0.0f), Vec2(72.0f));
			sprite->SetDiffuse(Col4(1, 0, 0, 1));

			SoundManager::Instance().PlayBGM(L"BGM");
		}
		catch (...) {
			throw;
		}
	}
	void GameStage::OnUpdate() {
		auto& app = App::GetApp();

		auto& device = app->GetInputDevice().GetControlerVec()[0];
		if (device.bConnected) {
			if (device.wPressedButtons & XINPUT_GAMEPAD_START) {
				OpenPose();
			}
			if (device.wPressedButtons & XINPUT_GAMEPAD_Y) {

			}
		}

		SetAllGameObjectActive(!m_IsPose);

		auto player = GetSharedGameObject<Player>(L"Player", false);
		if (player != nullptr) {
			m_ProtoHpNumber->UpdateNumber(player->GetPlayerHP());
		}

		m_ProtoScoreNumber->UpdateNumber(static_cast<int>(GetClearRate()));
	}

	void GameStage::OnDestroy() {
		SoundManager::Instance().StopAll();
	}
}
//end basecross
