/*!
@file GameStage.cpp
@brief 繧ｲ繝ｼ繝繧ｹ繝��繧ｸ螳滉ｽ
*/

#include "stdafx.h"
#include "Project.h"

namespace basecross {

	//--------------------------------------------------------------------------------------
	//	繧ｲ繝ｼ繝繧ｹ繝��繧ｸ繧ｯ繝ｩ繧ｹ螳滉ｽ
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
		//繝槭Ν繝√Λ繧､繝医�菴懈�
		auto PtrMultiLight = CreateLight<MultiLight>();
		//繝�ヵ繧ｩ繝ｫ繝医�繝ｩ繧､繝�ぅ繝ｳ繧ｰ繧呈欠螳
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
	/// 繧ｹ繝��繧ｸ隱ｭ縺ｿ霎ｼ縺ｿ險ｭ螳
	/// </summary>
	void GameStage::RegisterObjects() {
		auto& builder = AddGameObject<StageBuilder>(L"level.csv", 1.0f);
		builder->Register<FixedBox>(L"cube");
		builder->Register<Player>(L"player");
		builder->Register<Mob>(L"mob");
		builder->Register<RootPointer>(L"pointer");

		builder->LoadCsv();
	}
	/// <summary>
	/// 繝昴�繧ｺ繝｡繝九Η繝ｼ縺ｮ菴懈�
	/// </summary>
	void GameStage::CreatePose() {
		//繧ｿ繧､繝医Ν
		ButtonManager::Create(GetThis<Stage>(), L"POSE", L"POSE_TITLE", L"POSE_TITLE_SELECTED", Vec3(0.0f, 150.0f, 0.0f), Vec2(200, 50),
			[](shared_ptr<Stage> stage) {
				stage->PostEvent(0.0f, stage, App::GetApp()->GetScene<Scene>(), L"ToTitleStage");
			});
		//繧�ａ繧
		ButtonManager::Create(GetThis<Stage>(), L"POSE", L"POSE_ENDGAME", L"POSE_ENDGAME_SELECTED", Vec3(0.0f, 50.0f, 0.0f), Vec2(200, 50),
			[](shared_ptr<Stage> stage) {
				stage->PostEvent(0.0f, stage, App::GetApp()->GetScene<Scene>(), L"ToTitleStage");
			});
		//蜀埼幕
		ButtonManager::Create(GetThis<Stage>(), L"POSE", L"POSE_START", L"POSE_START_SELECTED", Vec3(0.0f, -50.0f, 0.0f), Vec2(200, 50),
			[](shared_ptr<Stage> stage) {
				auto currentStage = static_pointer_cast<GameStage>(stage);
				currentStage->ClosePose();
			});
		//繧ｵ繧ｦ繝ｳ繝
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
	/// 繧ｵ繧ｦ繝ｳ繝峨ユ繧ｹ繝医Γ繝九Η繝ｼ縺ｮ菴懈�
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
	/// 繝昴�繧ｺ逕ｻ髱｢繧帝哩縺倥ｋ
	/// </summary>
	void GameStage::ClosePose() {
		m_IsPose = false;
		ButtonManager::instance->Close(L"POSE");
	}
	/// <summary>
	/// 繝昴�繧ｺ逕ｻ髱｢繧帝幕縺
	/// </summary>
	void GameStage::OpenPose() {
		m_IsPose = true;
		ButtonManager::instance->Close(L"SOUND_TEST");
		ButtonManager::instance->OpenAndUse(L"POSE");
	}
	/// <summary>
	/// 逋ｻ骭ｲ縺輔ｌ縺ｦ縺�ｋ縺吶∋縺ｦ縺ｮ繧ｪ繝悶ず繧ｧ繧ｯ繝医�陦ｨ遉ｺ謫堺ｽ
	/// </summary>
	/// <param name="flag">陦ｨ遉ｺONOFF</param>
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
		//繧ｪ繝悶ず繧ｧ繧ｯ繝医�菴懈�
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

			//繝薙Η繝ｼ縺ｨ繝ｩ繧､繝医�菴懈�
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
