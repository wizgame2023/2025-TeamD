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
		m_CurrentCamera = CreateView<SingleView>();
		//m_OpeningCameraView = ObjectFactory::Create<SingleView>(GetThis<GameStage>());
		//auto ptrOpeningCamera = ObjectFactory::Create<OpeningCamera>();

		//ビューのカメラの設定
		m_Camera = ObjectFactory::Create<FollowCamera>(GetThis<GameStage>());
		//ToOpeningCamera();
		m_CurrentCamera->SetCamera(m_Camera);
		m_Camera->SetEye(eye);
		m_Camera->SetAt(at);
		//auto PtrCamera = ObjectFactory::Create<FollowCamera>(GetThis<GameStage>());
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

		app->RegisterTexture(L"SELECT_SRAGE", uiPath + L"NextStageBack.png");
		app->RegisterTexture(L"RESULT_TITLE", uiPath + L"ResultToTitle.png");
		//app->RegisterTexture(L"POSE_TITLE_SELECTED", uiPath + L"BackToTitle_Selected.png");
		app->RegisterTexture(L"RESULT_NEXT_STAGE", uiPath + L"ResultNextStage.png");
		app->RegisterTexture(L"RESULT_SELECT_BACK", uiPath + L"ResultSelectBackUI.png");
		app->RegisterTexture(L"RESULT_START__BACK", uiPath + L"ResultBackGameBack.png");
		app->RegisterTexture(L"RESULT_TITLE_BACK", uiPath + L"ResultToTitleBack.png");
		app->RegisterTexture(L"RESULT_NEXT_STAGE_BACK", uiPath + L"ResultNextStageBack.png");
		//app->RegisterTexture(L"POSE_ENDGAME_SELECTED", uiPath + L"NextStage_Selected.png");
		app->RegisterTexture(L"BGM_BAR", uiPath + L"BGM_MenuBar.png");
		app->RegisterTexture(L"BGM_BACKBAR", uiPath + L"BGM_MenuBackBar.png");
		app->RegisterTexture(L"BGM_SLIDEBAR", uiPath + L"BGM_MenuSlideBar.png");
		app->RegisterTexture(L"POSE_START", uiPath + L"BackGame.png");
		app->RegisterTexture(L"POSE_SOUND", uiPath + L"Sound_Menu.png");
		app->RegisterTexture(L"SE_VOLUME", uiPath + L"SE_Menu.png");
		app->RegisterTexture(L"BGM_VOLUME", uiPath + L"BGM_Menu.png");
		app->RegisterTexture(L"01", texPath + L"Black0.1.png");
		app->RegisterTexture(L"NUMBER", uiPath + L"Number.png");
		app->RegisterTexture(L"ACTION_PANCH", uiPath + L"UI_Panch.png");
		app->RegisterTexture(L"ACTION_DASH", uiPath + L"UI_Dash.png");
		app->RegisterTexture(L"ACTION_ULT", uiPath + L"UI_Ult.png");
		app->RegisterTexture(L"ACTION_ULT_EFFECT", uiPath + L"UI_UltEffect.png");
		app->RegisterTexture(L"ACTION_ULT_FRAME", uiPath + L"UI_Ult_Waku.png");

		app->RegisterTexture(L"HP_BAR_EDGE", uiPath + L"HpEdge.png");
		app->RegisterTexture(L"HP_BAR", uiPath + L"HpBar.png");
		app->RegisterTexture(L"HP_BAR2D", uiPath + L"HpBar2D.png");
		app->RegisterTexture(L"TARGET", uiPath + L"Target.png");
		app->RegisterTexture(L"BOSS_TEXT", uiPath + L"BossText.png");
		app->RegisterTexture(L"BOSS_APPEAR", uiPath + L"BossAppear.png");

		app->RegisterTexture(L"RESULT_BACK", uiPath + L"Result_Back.png");
		app->RegisterTexture(L"SEARCH_RANGE", texPath + L"SearchRange.png");
		app->RegisterTexture(L"AOF", texPath + L"AoF.png");
		app->RegisterTexture(L"BUILDING", texPath + L"Building.png");


		app->RegisterTexture(L"RESULT_MENU", uiPath + L"Result_Menu_Score.png");
		app->RegisterTexture(L"RESULT_TEXT2", uiPath + L"Result_Menu_Texts.png");
		app->RegisterTexture(L"RESULT_SCORE2", uiPath + L"Result_Score.png");
		app->RegisterTexture(L"RESULT_SCORE", uiPath + L"ResultScoreText.png");
		app->RegisterTexture(L"GAMEOVER_TEXT", uiPath + L"GameOver.png");
		
		app->RegisterTexture(L"POSE_SETTING", uiPath + L"Setting_Menu.png");
		app->RegisterTexture(L"POSE_BACK", uiPath + L"Menu_Back.png");
		app->RegisterTexture(L"POSE_CIRCLE", uiPath + L"SelectCircle_Menu.png");

		app->RegisterTexture(L"NEXT_WAVE", uiPath + L"NextWave.png");

		m_Effect = AddGameObject<EffectManeger>();
		m_Effect->RegisterResource(L"Test", effectPath + L"Laser01.efk");
		m_Effect->RegisterResource(L"Flash", effectPath + L"flash.efk");
		m_Effect->RegisterResource(L"Parry", effectPath + L"parry.efk");
		m_Effect->RegisterResource(L"Laser", effectPath + L"Laser.efk");
		m_Effect->RegisterResource(L"EnemyEye", effectPath + L"EnemyEye.efk");
		m_Effect->RegisterResource(L"MissileFlash", effectPath + L"MissileFlash.efk");
		m_Effect->RegisterResource(L"Trampling", effectPath + L"Trampling.efk");
		m_Effect->RegisterResource(L"Smoke", effectPath + L"Smoke.efk");
		m_Effect->RegisterResource(L"boost", effectPath + L"boost.efk");
		m_Effect->RegisterResource(L"ShockWave", effectPath + L"ShockWave.efk");
		m_Effect->RegisterResource(L"Panchi", effectPath + L"panchi.efk");
		m_Effect->RegisterResource(L"Brick", effectPath + L"Brick.efk");
		m_Effect->RegisterResource(L"HitEffect", effectPath + L"HitEffect.efk");
		m_Effect->RegisterResource(L"Bomb", effectPath + L"bomb.efk");
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
		builder->Register<LimitArea>(L"area");
		builder->Register<Building>(L"building");
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
		m_ResultMenu->SetIsPouse(true);
	}

	void GameStage::CreateGameOverMenu() {
		m_GameOverMenu = AddGameObject<GameOverMenu>(L"GAMEOVER");
		m_GameOverMenu->SetIsPouse(true);
	}

	void GameStage::CreateUI() {
		

		Vec3 bossHpPosition = Vec3(-400.0f, 400.0f - 20.0f, 0.0f); 
		Vec3 playerHpPosition = Vec3(-210.0f, -353.0f, 0.0f);
		m_NormalIcon = AddGameObject<NormalIcon>(L"ACTION_PANCH", Vec3(423.0f, -297.0f, 0.0f), Col4(1, 1, 1, 0.5f), Col4(1, 1, 1, 1.0f), 0.5f);
		m_NormalIcon->SetInput(XINPUT_GAMEPAD_A);
		m_Icon = AddGameObject<NormalIcon>(L"ACTION_DASH", Vec3(347.0f, -228.0f, 0.0f), Col4(1, 1, 1, 0.5f), Col4(1, 1, 1, 1.0f), 0.5f);
		m_Icon->SetInput(XINPUT_GAMEPAD_X);
		m_UltIcon = AddGameObject<UltIcon>();

		auto player = GetSharedGameObject<Player>(L"Player", false);
		m_PlayerHpBar = AddGameObject<HpSprite>(static_pointer_cast<Character>(player), playerHpPosition, Vec3(400.0f, 45.5f, 0.0f), Col4(0, 1, 0, 1));

		auto boss = GetSharedGameObject<BossEnemy>(L"BOSS", false);
		m_BossHpBar = AddGameObject<HpSprite>(static_pointer_cast<Character>(boss), bossHpPosition, Vec3(800.0f, 12.0f, 0.0f), Col4(1, 0, 0, 1));
		

		m_BossText = AddGameObject<Sprite>(L"BOSS_TEXT", Vec3(-400.0f, bossHpPosition.y + 20.0f, bossHpPosition.z), Vec2(100.0f, 24.0f));
		m_BossText->SetDiffuse(Col4(0, 0, 0, 1));
	}
	/// <summary>
	/// オブジェクトの描画をONOFF
	/// </summary>
	/// <param name="flag">描画ONOFF</param>
	void GameStage::SetAllGameObjectActive(bool flag) {
		for (auto& obj : GetGameObjectVec()) {
			if (!obj->FindTag(L"Button") && !obj->FindTag(L"Manager") && !obj->FindTag(L"Menu") ) {
				obj->SetUpdateActive(flag);
			}
		}
	}

	void GameStage::ToMainCamera()
	{
		const Vec3 eye(0.0f, 5.0f, -5.0f);
		const Vec3 at(0.0f);
		m_CurrentCamera->SetCamera(m_Camera);
		m_Camera->SetEye(eye);
		m_Camera->SetAt(at);
		auto player = GetSharedGameObject<Player>(L"Player", false);
		if (player != nullptr) {
			auto camera = static_pointer_cast<FollowCamera>(GetView()->GetTargetCamera());
			if (camera != nullptr) {
				camera->SetTarget(player->GetComponent<Transform>());
			}
		}
	}

	void GameStage::ToOpeningCamera()
	{
		Vec3 CameraPos = Vec3(10.0f, 5.0f, 0);
		Vec3 CameraStartEndPos = Vec3(5.0f,  5.0f, 0);
		Vec3 CameraEndPos = Vec3(15.0f, 10.0f, 0);
		Vec3 PlayEndpos = Vec3(0.0f, 5.0f, -5.0);
		Vec3 PlayStartpos = Vec3(0, 1.0f, 0);
		auto view = CreateView<SingleView>();
		//カメラのオープニングの移動(最初のカメラの位置、最後のカメラの位置、
// 　　　　　　　　　　　　　最初に見てる所、最後に見てる所、後半最初に見る位置、
// 　　　　　　　　　　　　　かかる時間(多分)、後半最後にいる位置、後半最後に見てる所)
		auto ptrOpeningCameraman = AddGameObject<OpeningCameraman>(CameraPos, CameraStartEndPos,
			PlayStartpos, Vec3(0), PlayEndpos,
			0.0f, CameraEndPos, PlayEndpos);
		auto ptrOpeningCamera = dynamic_pointer_cast<OpeningCamera>(m_OpeningCameraView->GetCamera());
		if (ptrOpeningCamera) {
			ptrOpeningCamera->SetCameraObject(ptrOpeningCameraman);
			SetView(m_OpeningCameraView);
		}
	}

	void GameStage::GameClear() {
		m_NormalIcon->SetDraw(false);
		m_Icon->SetDraw(false);
		m_UltIcon->SetDraw(false);
		//m_PlayerHpBarBackGround->SetDrawActive(false);
		m_PlayerHpBar->SetDrawActive(false);
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
		m_NormalIcon->SetDraw(false);
		m_Icon->SetDraw(false);
		m_UltIcon->SetDraw(false);
		//m_PlayerHpBarBackGround->SetDrawActive(false);
		m_PlayerHpBar->SetDrawActive(false);
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
			SoundManager::Instance().PlayBGM(L"BGM_GAME");
			auto player = GetSharedGameObject<Player>(L"Player", false);
			if (player != nullptr) {
				auto camera = static_pointer_cast<FollowCamera>(GetView()->GetTargetCamera());
				if (camera != nullptr) {
					camera->SetTarget(player->GetComponent<Transform>());
				}
			}
			GameManager::Instance()->SetZoneRate(0.5f);
			//GameManager::Instance()->StartZone(20.0f);
			//m_ResultMenu->Open();
			//m_GameOverMenu->Open();

		}
		catch (...) {
			throw;
		}
	}

	void GameStage::OnUpdate() {
		auto& app = App::GetApp();
		GameManager::Instance()->Update();
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

		if (m_ResultMenu->IsOpen())
		{
			auto player = GetSharedGameObject<Player>(L"Player", false);
			player->SetAnim(L"Clear");
			player->UpdateAnim();
		}
		if (m_GameOverMenu->IsOpen())
		{
			auto player = GetSharedGameObject<Player>(L"Player", false);
			player->SetAnim(L"Died");
			player->UpdateAnim();
		}
		if (m_PauseMenu->IsOpen() || m_SoundTestMenu->IsOpen()||m_GameOverMenu->IsOpen() || m_ResultMenu->IsOpen()) {
			//SetAllGameObjectActive(false);
			m_NormalIcon->SetDrawActive(false);
			m_Icon->SetDrawActive(false);
			m_UltIcon->SetDrawActive(false);

			m_BossHpBar->SetDrawActive(false);
			m_BossText->SetDrawActive(false);

		}
		else {
			//SetAllGameObjectActive(true);
			auto player = GetSharedGameObject<Player>(L"Player", false);
			if (player != nullptr) {
				m_UltIcon->SetCharge(player->GetEnergy());

			}
			auto boss = GetSharedGameObject<BossEnemy>(L"BOSS", false);
			if (boss != nullptr) {
				ScoreManager::Instance()->UpdateTime(elapsed);
				bool isBossDraw = boss->GetDrawActive();
				m_BossText->SetDrawActive(isBossDraw);
			}
			else {
				m_BossText->SetDrawActive(false);
			}

		}
	}


	void GameStage::OnDraw()
	{
		auto& camera = GetView()->GetTargetCamera();
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
		else if (msg == L"StopVibration") {
			XINPUT_VIBRATION vibration;
			vibration.wLeftMotorSpeed = 0;
			vibration.wRightMotorSpeed = 0;
			XInputSetState(0, &vibration);

			GameManager::Instance()->SetGameSpeed(1.0f);
		}
		else if (msg == L"HitStop") {
			GameManager::Instance()->SetGameSpeed(1.0f);

			XINPUT_VIBRATION vibration;
			vibration.wLeftMotorSpeed = 65535;
			vibration.wRightMotorSpeed = 65535;
			XInputSetState(0, &vibration);

			PostEvent(0.5f, nullptr, GetThis<Stage>(), L"StopVibration");
		}
	}
}
//end basecross
