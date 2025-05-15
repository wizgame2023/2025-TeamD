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
		m_Camera = ObjectFactory::Create<FollowCamera>(GetThis<GameStage>());
		//auto PtrCamera = ObjectFactory::Create<FollowCamera>(GetThis<GameStage>());
		PtrView->SetCamera(m_Camera);
		m_Camera->SetEye(eye);
		m_Camera->SetAt(at);
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
		wstring effectPath = mediaPath + L"Effekt/";
		app->RegisterTexture(L"GROUND", texPath + L"Ground.png");

		app->RegisterTexture(L"POSE_TITLE", uiPath + L"ResultToTitle.png");
		//app->RegisterTexture(L"POSE_TITLE_SELECTED", uiPath + L"BackToTitle_Selected.png");
		app->RegisterTexture(L"POSE_SELECT", uiPath + L"ResultNextStage.png");
		//app->RegisterTexture(L"POSE_ENDGAME_SELECTED", uiPath + L"NextStage_Selected.png");
		app->RegisterTexture(L"POSE_START", uiPath + L"BackGame.png");
		app->RegisterTexture(L"POSE_SOUND", uiPath + L"Sound_Menu.png");
		app->RegisterTexture(L"SE_VOLUME", uiPath + L"SE_Menu.png");
		app->RegisterTexture(L"BGM_VOLUME", uiPath + L"BGM_Menu.png");
		app->RegisterTexture(L"01", texPath + L"Black0.1.png");
		app->RegisterTexture(L"NUMBER", uiPath + L"Number.png");
		app->RegisterTexture(L"ACTION_PANCH", uiPath + L"UI_Panch.png");
		app->RegisterTexture(L"ACTION_DASH", uiPath + L"UI_Dash.png");
		app->RegisterTexture(L"ACTION_ULT", uiPath + L"UI_Ult.png");
		app->RegisterTexture(L"ACTION_ULT_FRAME", uiPath + L"UI_Ult_Waku.png");

		app->RegisterTexture(L"HP_BAR", uiPath + L"HpBar.png");
		app->RegisterTexture(L"TARGET", uiPath + L"Target.png");
		app->RegisterTexture(L"BOSS_TEXT", uiPath + L"BossText.png");
		app->RegisterTexture(L"BOSS_APPEAR", uiPath + L"BossAppear.png");

		app->RegisterTexture(L"SEARCH_RANGE", texPath + L"SearchRange.png");
		app->RegisterTexture(L"RESULT_MENU", uiPath + L"Result_Menu2.png");
		app->RegisterTexture(L"RESULT_TEXT", uiPath + L"ResultTexts.png");
		app->RegisterTexture(L"RESULT_TEXT2", uiPath + L"ResultTexts2.png");
		app->RegisterTexture(L"RESULT_SCORE2", uiPath + L"ResultScore.png");
		app->RegisterTexture(L"RESULT_SCORE", uiPath + L"ResultScoreText.png");
		app->RegisterTexture(L"GAMEOVER_TEXT", uiPath + L"GameOver.png");
		
		app->RegisterTexture(L"POSE_SETTING", uiPath + L"Setting_Menu.png");
		app->RegisterTexture(L"POSE_BACK", uiPath + L"Menu_Back.png");
		app->RegisterTexture(L"POSE_CIRCLE", uiPath + L"SelectCircle_Menu.png");

		app->RegisterTexture(L"NEXT_WAVE", uiPath + L"NextWave.png");

		m_Effect = ObjectFactory::Create<EffectManeger>();
		m_Effect->RegisterResource(L"Test", effectPath + L"Laser01.efk");
		m_Effect->RegisterResource(L"Flash", effectPath + L"flash.efk");
		m_Effect->RegisterResource(L"Parry", effectPath + L"parry.efk");
		m_Effect->RegisterResource(L"Laser", effectPath + L"Laser.efk");
		m_Effect->RegisterResource(L"EnemyEye", effectPath + L"EnemyEye.efk");
		m_Effect->RegisterResource(L"MissileFlash", effectPath + L"MissileFlash.efk");
		m_Effect->RegisterResource(L"Trampling", effectPath + L"Trampling.efk");
		m_Effect->RegisterResource(L"Smoke", effectPath + L"Smoke.efk");
		m_Effect->RegisterResource(L"boost", effectPath + L"boost.efk");

	}

	shared_ptr <EffectManeger> GameStage::GetCreateEffect()
	{
		return m_Effect;
	}
	shared_ptr <FollowCamera>GameStage::SetCameraPause()
	{
		return m_Camera;
	}

	/// <summary>
	/// リソースの作成
	/// </summary>
	void GameStage::RegisterObjects() {
		auto& builder = AddGameObject<StageBuilder>(m_MapFileName, 1.0f);
		builder->Register<FixedBox>(L"cube");
		builder->Register<Player>(L"player");
		builder->Register<Legion>(L"wave");
		//builder->Register<Mob>(L"enemy");
		builder->Register<BossEnemy>(L"boss");
		builder->Register<Ground>(L"Ground");
		builder->Register<flyobject>(L"flyobject");
		builder->LoadCsv();

	}
	/// </summary>
	/// ポーズメニューの作成
	/// <summary>
	void GameStage::CreatePose() {
		m_PauseMenu = AddGameObject<PauseMenu>(L"PAUSE", m_SoundTestMenu);
		m_PauseMenu->SetIsPouse(true);
	}
	/// <summary>
	/// サウンドテストメニューの作成
	/// </summary>
	void GameStage::CreateSoundTest() {
		m_SoundTestMenu = AddGameObject<SoundTestMenu>(L"SOUND_TEST");
		m_SoundTestMenu->SetIsPouse(true);
	}
	/// <summary>
	/// リザルトメニューの作成
	/// </summary>
	void GameStage::CreateResult() {
		m_ResultMenu = AddGameObject<ResultMenu>(L"RESULT");
	}

	void GameStage::CreateGameOverMenu() {
		m_GameOverMenu = AddGameObject<GameOverMenu>(L"GAMEOVER");
		m_GameOverMenu->SetIsPouse(true);
	}

	void GameStage::CreateUI() {
		auto icon = AddGameObject<NormalIcon>(L"ACTION_PANCH", Vec3(423.0f, -297.0f, 0.0f), Col4(1, 1, 1, 0.5f), Col4(1, 1, 1, 1.0f), 0.5f);
		icon->SetInput(XINPUT_GAMEPAD_A);
		icon = AddGameObject<NormalIcon>(L"ACTION_DASH", Vec3(347.0f, -228.0f, 0.0f), Col4(1, 1, 1, 0.5f), Col4(1, 1, 1, 1.0f), 0.5f);
		icon->SetInput(XINPUT_GAMEPAD_X);
		m_UltIcon = AddGameObject<UltIcon>();

		m_PlayerHpBarBackGround = AddGameObject<Sprite>(L"HP_BAR", Vec3(-631.0f, 393.0f, 0.0f), Vec2(400.0f, 65.5f));
		m_PlayerHpBarBackGround->SetDiffuse(Col4(0, 0, 0, 1));

		m_PlayerHpBar = AddGameObject<Sprite>(L"HP_BAR", Vec3(-631.0f, 393.0f, 0.0f), Vec2(400.0f, 65.5f));
		m_PlayerHpBar->SetDiffuse(Col4(0, 1, 0, 1));

		m_BossHpBarBackGround = AddGameObject<Sprite>(L"HP_BAR", Vec3(-300.0f, -353.0f, 0.0f), Vec2(600.0f, 24.0f));
		m_BossHpBarBackGround->SetDiffuse(Col4(0, 0, 0, 1));

		m_BossHpBar = AddGameObject<Sprite>(L"HP_BAR", Vec3(-300.0f, -353.0f, 0.0f), Vec2(600.0f, 24.0f));
		m_BossHpBar->SetDiffuse(Col4(1, 0, 0, 1));

		m_BossText = AddGameObject<Sprite>(L"BOSS_TEXT", Vec3(-385.0f, -353.0f, 0.0f), Vec2(100.0f, 24.0f));
		m_BossText->SetDiffuse(Col4(0, 0, 0, 1));
	}
	/// <summary>
	/// オブジェクトの描画をONOFF
	/// </summary>
	/// <param name="flag">描画ONOFF</param>
	void GameStage::SetAllGameObjectActive(bool flag) {
		for (auto& obj : GetGameObjectVec()) {
			if (!obj->FindTag(L"Button") && !obj->FindTag(L"Manager") && !obj->FindTag(L"Menu")) {
				obj->SetUpdateActive(flag);
			}
		}
	}
	void GameStage::GameClear() {
		m_ResultMenu->Open();
		auto camera = GetView()->GetTargetCamera();
		auto player = GetSharedGameObject<Player>(L"Player", false);
		if (player != nullptr && camera != nullptr) {
			player->SetIsGaol(true);
			auto newCamera = ObjectFactory::Create<ResultCamera>(camera->GetEye(), camera->GetAt(), player);
			auto view = static_pointer_cast<SingleView>(GetView());
			view->SetCamera(newCamera);
		}
	}

	void GameStage::GameOver() {
		m_GameOverMenu->Open();
		auto player = GetSharedGameObject<Player>(L"Player", false);
		if (player != nullptr ) {
			player->SetIsGaol(true);
			auto enemygruop = GetSharedObjectGroup(L"EnemyGroup");
			auto enemys = enemygruop->GetGroupVectors();
			for (auto& enemy : enemys)
			{
				auto shEnemy = enemy.lock();
				if (shEnemy->GetUpdateActive() == true)
				{
					shEnemy->SetUpdateActive(false);
				}
			}
		}

	}
	void GameStage::OnCreate() {
		try {
			m_TotalTime = 0;
			ScoreManager::Instance()->Init();
			CreateSharedObjectGroup(L"BulletGroup");
			CreateSharedObjectGroup(L"EnemyGroup");
			CreateSharedObjectGroup(L"PointerGroup");
			CreateSharedObjectGroup(L"Legion");
			CreateSharedObjectGroup(L"Citizen");

			//ビューとライトの作成
			CreateViewLight();
			CreateResource();
			RegisterObjects();
			AddGameObject<ButtonManager>();
			ButtonManager::instance->SetSound(L"SE_ACCEPT");

			CreateSoundTest();
			CreatePose();
			CreateResult();
			CreateGameOverMenu();
			ButtonManager::instance->CloseAll();
			CreateUI();
			auto player = GetSharedGameObject<Player>(L"Player", false);
			if (player != nullptr) {
				auto camera = static_pointer_cast<FollowCamera>(GetView()->GetTargetCamera());
				if (camera != nullptr) {
					camera->SetTarget(player->GetComponent<Transform>());
				}
			}
			SoundManager::Instance().PlayBGM(L"BGM_TITLE");
			
			auto score = ScoreBorder<float>({ 10.0f,20.0f,30.0f,40.0f }, JudgeMode::UpperOrder);
			int rank = score.CalcRank(12.0f);
		}
		catch (...) {
			throw;
		}
	}

	void GameStage::OnUpdate() {
		auto& app = App::GetApp();
		m_Effect->OnUpdate();
		float elapsed = app->GetElapsedTime();
		auto& device = app->GetInputDevice().GetControlerVec()[0];
		if (device.bConnected) {
			if (device.wPressedButtons & XINPUT_GAMEPAD_START) {
				m_SoundTestMenu->Close();
				m_PauseMenu->Open();
				m_Effect->SetEffectPause(true);
				m_Camera->SetCameraPause(true);
			}
		}

		if (m_PauseMenu->IsOpen() || m_SoundTestMenu->IsOpen()||m_GameOverMenu->IsOpen()) {
			//SetAllGameObjectActive(false);
		}
		else {
			//SetAllGameObjectActive(true);
			auto player = GetSharedGameObject<Player>(L"Player", false);
			if (player != nullptr) {
				m_UltIcon->SetCharge(player->GetEnergy());

				float currentHp = player->GetHP();
				float maxHp = player->GetMaxHP();
				m_PlayerHpBar->UpdateSize(Vec3(currentHp / maxHp, 1, 1));
			}
			auto boss = GetSharedGameObject<BossEnemy>(L"BOSS", false);
			if (boss != nullptr) {
				ScoreManager::Instance()->UpdateTime(elapsed);
				bool isBossDraw = boss->GetDrawActive();
				if (isBossDraw) {
					float currentHp = boss->GetHP();
					float maxHp = boss->GetMaxHP();
					m_BossHpBar->UpdateSize(Vec3(currentHp / maxHp, 1, 1));
				}
				m_BossHpBar->SetDrawActive(isBossDraw);
				m_BossHpBarBackGround->SetDrawActive(isBossDraw);
				m_BossText->SetDrawActive(isBossDraw);
			}
			else {
				m_BossHpBar->SetDrawActive(false);
				m_BossHpBarBackGround->SetDrawActive(false);
				m_BossText->SetDrawActive(false);
			}

		}
	}


	void GameStage::OnDraw()
	{
		auto& camera = GetView()->GetTargetCamera();

		m_Effect->SetViewProj(camera->GetViewMatrix(), camera->GetProjMatrix());
		m_Effect->OnDraw();

	}

	void GameStage::OnDestroy() {
		SoundManager::Instance().StopAll();
	}

	void GameStage::OnEvent(const shared_ptr<Event>& event) {
		auto& msg = event->m_MsgStr;

		if (msg == L"DefeatBoss") {
			GameClear();
		}
		else if (msg == L"AppaerBoss") {
			AddGameObject<BossAppearText>(Vec3(-150, 300, 0.0f), Vec3(300, 100, 1.0f));
		}
		else if (msg == L"DeadPlayer") {
			GameOver();
		}
		else if (msg == L"PinchPlayer") {
			SoundManager::Instance().PlayBGM(L"BGM_GAME_PINCH");
		}
		else if(msg == L"StartBoss") {

		}
		else if (msg == L"EndBoss") {
			SoundManager::Instance().PlayBGM(L"BGM_GAME_BOSS");
		}
	}
}
//end basecross
