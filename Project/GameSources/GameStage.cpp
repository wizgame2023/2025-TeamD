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
	void GameStage::CreateViewLight() {		//ビューのカメラの設定
		m_ProductionCameraView = ObjectFactory::Create<SingleView>(GetThis<GameStage>());
		auto ptrOpeningCamera = ObjectFactory::Create<ProductionCamera>();
		m_ProductionCameraView->SetCamera(ptrOpeningCamera);

		m_MyCameraView = CreateView<SingleView>();
		auto PtrCamera = ObjectFactory::Create<FollowCamera>(GetThis<GameStage>());
		m_MyCameraView->SetCamera(PtrCamera);
		m_Camera = PtrCamera;
		//マルチライトの作成
		auto PtrMultiLight = CreateLight<MultiLight>();
		//デフォルトのライティングを指定
		PtrMultiLight->SetDefaultLighting2();
	}
	void GameStage::CreateResource() {
		auto& app = App::GetApp();
		auto mediaPath = app->GetDataDirWString();
		wstring uiPath = mediaPath + L"UI/";
		wstring texPath = mediaPath + L"Textures/";
		wstring modelPath = mediaPath + L"Models/";
		wstring effectPath = mediaPath + L"Effekt/";
		wstring skypath = mediaPath + L"BackGround/";
		app->RegisterTexture(L"GROUND", texPath + L"Ground.png");
		app->RegisterTexture(L"WAY", texPath + L"Asphalt2.png");

		app->RegisterTexture(L"SELECT_SRAGE", uiPath + L"NextStageBack.png");
		app->RegisterTexture(L"RESULT_TITLE2", uiPath + L"ResultToTitle.png");
		app->RegisterTexture(L"RESULT_TITLE", uiPath + L"Result_GoTitle.png");
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
		app->RegisterTexture(L"ACTION_PANCH", uiPath + L"UI_Panch_A.png");
		app->RegisterTexture(L"ACTION_DASH", uiPath + L"UI_Dash_X.png");
		app->RegisterTexture(L"ACTION_ULT", uiPath + L"UI_Ult.png");
		app->RegisterTexture(L"ACTION_ULT_EFFECT", uiPath + L"UI_UltEffect.png");
		app->RegisterTexture(L"ACTION_ULT_FRAME", uiPath + L"UI_Ult_Waku_B.png");

		app->RegisterTexture(L"HP_BAR_EDGE", uiPath + L"HpEdge.png");
		app->RegisterTexture(L"HP_BAR", uiPath + L"HpBar.png");
		app->RegisterTexture(L"HP_BAR2D", uiPath + L"HpBar2D.png");
		app->RegisterTexture(L"HP_BAR_FRAME", uiPath + L"HP_kazari.png");

		app->RegisterTexture(L"TARGET", uiPath + L"Target.png");
		app->RegisterTexture(L"BOSS_TEXT", uiPath + L"BossText.png");
		app->RegisterTexture(L"BOSS_APPEAR", uiPath + L"BossAppear.png");

		app->RegisterTexture(L"RESULT_BACK", uiPath + L"Result_Back.png");
		app->RegisterTexture(L"SEARCH_RANGE", texPath + L"SearchRange.png");
		app->RegisterTexture(L"AOF", texPath + L"AoF.png");
		app->RegisterTexture(L"BUILDING", texPath + L"Building.png");


		app->RegisterTexture(L"RESULT_MENU", uiPath + L"Result_Menu_Score.png");
		app->RegisterTexture(L"RESULT_TEXT", uiPath + L"Result_Texts.png");
		app->RegisterTexture(L"RESULT_TEXT2", uiPath + L"Result_Menu_Texts.png");
		app->RegisterTexture(L"RESULT_SCORE2", uiPath + L"Result_Score.png");
		app->RegisterTexture(L"RESULT_SCORE", uiPath + L"ResultScoreText.png");
		app->RegisterTexture(L"GAMEOVER_TEXT", uiPath + L"Game_Over.png");
		
		app->RegisterTexture(L"POSE_SETTING", uiPath + L"Setting_Menu.png");
		app->RegisterTexture(L"POSE_BACK", uiPath + L"Menu_Back.png");
		app->RegisterTexture(L"POSE_CIRCLE", uiPath + L"SelectCircle_Menu.png");
		app->RegisterTexture(L"GO_RESTART", uiPath + L"GoReStart.png");
		app->RegisterTexture(L"SELECT_RESULT", uiPath + L"Result_To_Select.png");

		app->RegisterTexture(L"POSE_START2", uiPath + L"BackGame2.png");
		app->RegisterTexture(L"RESULT_TITLE3", uiPath + L"Result_GoTitle2.png");
		app->RegisterTexture(L"POSE_SOUND2", uiPath + L"Sound_Menu2.png");

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
		builder->Register<Mob>(L"enemy");
		builder->Register<BossEnemy>(L"boss");
		builder->Register<Ground>(L"Ground");
		builder->Register<Way>(L"way");
		builder->Register<LimitArea>(L"area");
		builder->Register<Building>(L"building");
		builder->Register<flyobject>(L"flyobject");
		builder->LoadCsv();

	}
	
	/// </summary>
	/// ポーズメニューの作成
	/// <summary>
	void GameStage::CreatePose() {
		m_PauseMenu = AddGameObject<PauseMenu>(L"PAUSE", static_pointer_cast<Menu>(m_SoundTestMenu));
		m_PauseMenu->SetIsPouse(true);
		m_SoundTestMenu->SetPauseMenu(m_PauseMenu);
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
		

		Vec3 bossHpPosition = Vec3(-400.0f, 400.0f - 40.0f, 0.0f);
		Vec3 playerHpPosition = Vec3(-270.0f, -353.0f, 0.0f);
		m_NormalIcon = AddGameObject<NormalIcon>(L"ACTION_PANCH", Vec3(410.0f, -257.0f, 0.0f), Col4(1, 1, 1, 0.5f), Col4(1, 1, 1, 1.0f), 0.5f);
		m_NormalIcon->SetInput(XINPUT_GAMEPAD_A);
		m_Icon = AddGameObject<NormalIcon>(L"ACTION_DASH", Vec3(287.0f, -158.0f, 0.0f), Col4(1, 1, 1, 0.5f), Col4(1, 1, 1, 1.0f), 0.5f);
		m_Icon->SetInput(XINPUT_GAMEPAD_X);
		m_UltIcon = AddGameObject<UltIcon>();

		auto player = GetSharedGameObject<Player>(L"Player", false);
		m_PlayerHpBar = AddGameObject<HpSprite>(static_pointer_cast<Character>(player), playerHpPosition, Vec3(540.0f, 20.5f, 0.0f), Col4(0.1, 0.8, 0.1, 1));
		auto frame = m_PlayerHpBar->AddSprite(L"HP_BAR_FRAME", Vec3(-50, 10, 0), Vec2(600.0f, 40.0f));
		m_PlayerHpBar->SetBackColor(Col4(0, 0, 0, 1));
		frame->SetDrawLayer(1);

		auto boss = GetSharedGameObject<BossEnemy>(L"BOSS", false);
		m_BossStunBar = AddGameObject<StunSprite>(boss, bossHpPosition - Vec3(0.0f, 12.0f, 0.0f), Vec3(600.0f, 4.5f, 0.0f));
		m_BossHpBar = AddGameObject<HpSprite>(static_pointer_cast<Character>(boss), bossHpPosition, Vec3(800.0f, 12.0f, 0.0f), Col4(1, 0, 0, 1));

		auto bossFrame = m_BossHpBar->AddSprite(L"HP_BAR_FRAME", Vec3(-50, 10, 0) , Vec2(865.0f, 30.0f));
		bossFrame->SetDrawActive(false);
		m_BossHpBar->SetBackColor(Col4(0, 0, 0, 1));


		m_BossText = AddGameObject<Sprite>(L"BOSS_TEXT", Vec3(-400.0f, bossHpPosition.y + 30.0f, bossHpPosition.z), Vec2(100.0f, 24.0f));
		m_BossText->SetDiffuse(Col4(0, 0, 0, 1));

		fadeSprite = AddGameObject<Sprite>(L"FADE", Vec3(0.0f, 0.0f, 0.0f), Vec2(1480.0f, 880.0f), true);
		fadeSprite->SetDrawLayer(1);
		m_Fade = fadeSprite->AddComponent<SpriteFade>(1.0f);
		m_Fade->FadeOut();
		m_Fade->Stop();

	}
	/// <summary>
	/// オブジェクトの更新をONOFF
	/// </summary>
	/// <param name="flag">描画ONOFF</param>
	void GameStage::SetAllGameObjectActive(bool flag) {
		for (auto& obj : GetGameObjectVec()) {
			if (!obj->FindTag(L"Button") && !obj->FindTag(L"Manager") && !obj->FindTag(L"Menu") && !obj->FindTag(L"Camera")) {
				obj->SetUpdateActive(flag);
			}
		}
	}

	void GameStage::ToMainCamera(){
		auto player = GetSharedGameObject<Player>(L"Player", false);

		if (player != nullptr) {
			auto camera = static_pointer_cast<FollowCamera>(m_MyCameraView->GetCamera());
			if (camera != nullptr) {
				SetView(m_MyCameraView);
				camera->SetTarget(player->GetComponent<Transform>());
				m_cameraState = CameraState::FOLLOWCAMERA;
				auto player = GetSharedGameObject<Player>(L"Player", false);
				if (player != nullptr) {
					player->SetIsGaol(false);
				}
			}
		}
	}

	void GameStage::ToOpeningCamera(){
		auto player = GetSharedGameObject<Player>(L"Player", false);
		if (player != nullptr) {
			player->SetIsGaol(true);
			float dire = 3.0f;
			Vec3 Playpos = player->GetPosition(); 
			Vec3 AtPos = Playpos + Vec3(0.0f, 0.0f, 0.0f);
			Vec3 AtEndPos = Playpos + Vec3(0.0f, 1.0f, 0.0f);
			Vec3 CameraPos = Playpos + Vec3(0.0f, -0.5f, -dire/ 1.5f);
			Vec3 CameraEndPos = Playpos + Vec3(0.0f, 2.0f, -dire * 1.5f);
			m_cameraState = CameraState::OPENINGCAMERA;
			// 補間開始時のカメラ位置
			// 補間終了時のカメラ位置（最終位置）
			// 補間開始時にカメラが注視するターゲット位置
			// 補間終了時にカメラが注視するターゲット位置
			// 第二フェーズ用のカメラ最終位置（必要に応じて利用）
			// 第二フェーズ用の注視ターゲット位置（必要に応じて利用）
			// カメラの補間にかける総時間（外部からの参照）

			auto ptrOpeningCameraman = AddGameObject<ProductionCameraman>();
			ptrOpeningCameraman->SetReverse(false);
			ptrOpeningCameraman->StartOpeningAnimation(CameraPos, CameraEndPos, AtPos, AtEndPos, -CameraPos, AtEndPos, 4.0f, 0.0f,true);
			ptrOpeningCameraman->SetMoveType(ProductionCameraman::MoveType::Orbit);
			SetSharedGameObject(L"ProductionCamera", ptrOpeningCameraman);

			auto ptrOpeningCamera = static_pointer_cast<ProductionCamera>(m_ProductionCameraView->GetCamera());
			ptrOpeningCamera->SetEye(ptrOpeningCameraman->GetEyePos());
			ptrOpeningCamera->SetAt(ptrOpeningCameraman->GetAtPos());
			if (ptrOpeningCamera) {
				SetView(m_ProductionCameraView);
				ptrOpeningCamera->SetCameraObject(ptrOpeningCameraman);
			}
		}
	}

	void GameStage::GameClear() {

		App::GetApp()->GetScene<Scene>()->Clear(m_StageData);
		m_NormalIcon->SetDraw(false);
		m_Icon->SetDraw(false);
		m_UltIcon->SetDraw(false);
		m_UltEnege = 0.0f;
		m_UltIcon->SetCharge(m_UltEnege);
		//m_PlayerHpBarBackGround->SetDrawActive(false);
		m_PlayerHpBar->SetDrawActive(false);
		m_BossStunBar->SetDrawActive(false);
		auto player = GetSharedGameObject<Player>(L"Player", false);
		auto camera = static_pointer_cast<FollowCamera>(m_MyCameraView->GetCamera());
		if (player != nullptr && camera != nullptr && m_cameraState == CameraState::FOLLOWCAMERA) {
			player->SetIsGaol(true);
			SoundManager::Instance().PlayBGM(L"BGM_GAMECLEAR", 1.0f);
			Vec3 playerPos = player->GetPosition();
			Vec3 playerForwardOffset = player->GetForward() * 2.0f;
			Vec3 cameraStartPos = camera->GetEye();
			Vec3 cameraStartAt = camera->GetAt();
			Vec3 animAtPos = playerPos + Vec3(0.0f, 0.5f, 0.0f);
			Vec3 side = cross(playerForwardOffset, Vec3(0, 1, 0));
			Vec3 animEndPos1 = animAtPos + playerForwardOffset;
			Vec3 animEndPos2 = animAtPos - playerForwardOffset;

			auto productionCamera = GetSharedGameObject<ProductionCameraman>(L"ProductionCamera", false);
			productionCamera->SetReverse(false);
			productionCamera->SetMoveType(ProductionCameraman::MoveType::Linear);
			productionCamera->StartOpeningAnimation(
				cameraStartPos,	
				animEndPos1+ side, 
				cameraStartAt,	
				animAtPos - side,  
				animEndPos2, 
				animAtPos,
				3.0f,
				0.0f,
				false
			);
			auto ptrOpeningCamera = static_pointer_cast<ProductionCamera>(m_ProductionCameraView->GetCamera());
			if (ptrOpeningCamera) {
				SetView(m_ProductionCameraView);
				ptrOpeningCamera->SetCameraObject(productionCamera);
			}
			
			m_cameraState = CameraState::RESULTCAMERA;
		}
	}

	void GameStage::GameOver() {
		m_NormalIcon->SetDraw(false);
		m_Icon->SetDraw(false);
		m_UltIcon->SetDraw(false);
		m_UltEnege = 0.0f;
		m_UltIcon->SetCharge(m_UltEnege);
		//m_PlayerHpBarBackGround->SetDrawActive(false);
		m_PlayerHpBar->SetDrawActive(false);
		m_BossStunBar->SetDrawActive(false);
		SoundManager::Instance().PlayBGM(L"BGM_GAMEOVER", 1.0f);
		auto player = GetSharedGameObject<Player>(L"Player", false);
		if (player != nullptr ) {

			auto enemygruop = GetSharedObjectGroup(L"EnemyGroup");
			auto enemys = enemygruop->GetGroupVectors();
			for (auto& enemy : enemys){
				auto shEnemy = enemy.lock();
				if (shEnemy->GetUpdateActive() == true){
					shEnemy->SetUpdateActive(false);
				}
			}

		}
	}

	void GameStage::GameOverCamera(){
		auto player = GetSharedGameObject<Player>(L"Player", false);
		if (player != nullptr) {
			player->SetIsGaol(true);
			Vec3 Playpos = player->GetPosition();
			Vec3 Playrot = player->GetForward();
			auto camera = static_pointer_cast<FollowCamera>(m_MyCameraView->GetCamera());
			Vec3 CameraPos = camera->GetEye();

			Vec3 CameraEndPos = Playpos + (Playrot / 2) + Vec3(0.0f, 1.0f, 0.0f);

			auto productionCamera = GetSharedGameObject<ProductionCameraman>(L"ProductionCamera", false);
			productionCamera->SetMoveType(ProductionCameraman::MoveType::Linear);
			productionCamera->SetReverse(false);
			productionCamera->StartOpeningAnimation(
				CameraPos, 
				CameraEndPos, 
				Playpos, 
				Playpos + (-Playrot / 2),
				-CameraPos,
				Playpos,
				3.0f,
				0.0f,
				false
			);

			auto ptrOpeningCamera = static_pointer_cast<ProductionCamera>(m_ProductionCameraView->GetCamera());
			if (ptrOpeningCamera) {

				SetView(m_ProductionCameraView);
				ptrOpeningCamera->SetCameraObject(productionCamera);
			}
			m_cameraState = CameraState::OPENINGCAMERA;
		}
	}

	void GameStage::BossAppaerCamera(){
		auto player = GetSharedGameObject<Player>(L"Player", false);
		auto boss = GetSharedGameObject<BossEnemy>(L"BOSS", false);
		auto camera = static_pointer_cast<FollowCamera>(m_MyCameraView->GetCamera());

		if (player != nullptr) {
			player->SetIsGaol(true);
			Vec3 playerPos = player->GetPosition();
			Vec3 bossPos = boss->GetPosition();
			Vec3 bossRot = boss->GetForward();
			Vec3 CameraPos = camera->GetEye();
			Vec3 CameraEndPos = bossPos + Vec3(0.0f, -bossPos.y + 2.0f, 0.0f);

			auto productionCamera = GetSharedGameObject<ProductionCameraman>(L"ProductionCamera", false);
			productionCamera->SetReverse(true);
			productionCamera->SetMoveType(ProductionCameraman::MoveType::Linear);
			productionCamera->StartOpeningAnimation(
				CameraPos,
				CameraEndPos - bossRot * 7,
				playerPos,
				CameraEndPos,
				-CameraPos,
				bossPos,
				4.5f,
				0.0f,
				true
			);

			auto ptrOpeningCamera = static_pointer_cast<ProductionCamera>(m_ProductionCameraView->GetCamera());
			if (ptrOpeningCamera) {
				SetView(m_ProductionCameraView);
				ptrOpeningCamera->SetCameraObject(productionCamera);
			}
		}

	}

	void GameStage::OnCreate() {
		try {
			m_TotalTime = 0;
			m_IsGameStater = 0;
			ScoreManager::Instance()->Init();
			CreateSharedObjectGroup(L"BulletGroup");
			CreateSharedObjectGroup(L"EnemyGroup");
			CreateSharedObjectGroup(L"PointerGroup");
			CreateSharedObjectGroup(L"Legion");
			CreateSharedObjectGroup(L"Citizen");

			//ビューとライトの作成
			CreateViewLight();
			CreateResource();
			auto& app = App::GetApp();
			auto path = app->GetDataDirWString();
			auto skypath = path + L"BackGround/";
			for (const auto& key : SkyBackGround::pairs) {

				App::GetApp()->RegisterTexture(key.first, skypath + key.first + L".bmp");
			}
			AddGameObject<SkyBackGround>();
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

			//ToMainCamera();
			ToOpeningCamera();

			auto score = ScoreBorder<float>({ 10.0f,20.0f,30.0f,40.0f }, JudgeMode::UpperOrder);
			int rank = score.CalcRank(12.0f);
			GameManager::Instance()->SetZoneRate(0.5f);

		}
		catch (...) {
			throw;
		}
	}

	void GameStage::OnUpdate() {
		auto& app = App::GetApp();
		RayCast::DebugRay(Line(Vec3(0,1,0), Vec3(0,1,10)), Col4(1,0,0,1), GetThis<Stage>());
		GameManager::Instance()->Update();
		float elapsed = app->GetElapsedTime();
		auto& device = app->GetInputDevice().GetControlerVec()[0];
		auto productionCamera = GetSharedGameObject<ProductionCameraman>(L"ProductionCamera", false);

		if (device.bConnected) {
			if (device.wPressedButtons & XINPUT_GAMEPAD_START && m_cameraState == CameraState::FOLLOWCAMERA) {
				m_SoundTestMenu->Close();
				m_PauseMenu->Open();
				m_Effect->SetEffectPause(true);
				m_Camera->SetCameraPause(true);
			}
		}
		if (m_PauseMenu->IsOpen() || m_SoundTestMenu->IsOpen()||m_GameOverMenu->IsOpen() || m_ResultMenu->IsOpen()) {
			m_NormalIcon->SetDrawActive(false);
			m_Icon->SetDrawActive(false);
			m_UltIcon->SetDrawActive(false);

			m_BossHpBar->SetDrawActive(false);
			m_BossText->SetDrawActive(false);
		}
		else {
			//SetAllGameObjectActive(true);
			if (m_cameraState == CameraState::FOLLOWCAMERA) {
				auto player = GetSharedGameObject<Player>(L"Player", false);
				if (player != nullptr) {
					m_UltIcon ->SetCharge(player->GetEnergy());
				}
			}
			auto boss = GetSharedGameObject<BossEnemy>(L"BOSS", false);
			if (boss != nullptr) {
				ScoreManager::Instance()->UpdateTime(elapsed);
				bool isBossDraw = boss->GetDrawActive();
				m_BossHpBar->SetDrawActive(isBossDraw);
				m_BossText->SetDrawActive(isBossDraw);
			}
			else {
				m_BossHpBar->SetDrawActive(false);
				m_BossText->SetDrawActive(false);
			}
		}

		auto spawner = GetSharedGameObject<Spawner>(L"Spawner", false);

		if (productionCamera->GetEndState())
		{
			if (m_IsGameStater == 1)
			{
				if (!m_ResultMenu->IsOpen()) {
					m_ResultMenu->Open();
				}
				auto player = GetSharedGameObject<Player>(L"Player", false);
				player->SetAnim(L"Clear");
				player->UpdateAnim();

			}
			else if (m_IsGameStater == 2)
			{
				if (!m_GameOverMenu->IsOpen()) {
					m_GameOverMenu->Open();
					//m_Fade->Play();
				}
				auto player = GetSharedGameObject<Player>(L"Player", false);
				player->SetAnim(L"Died");
				player->UpdateAnim();

			}
			else {
				spawner->Start();
			}
		}
		else{
			if (spawner) {
				spawner->Stop();
			}
		}

	}


	void GameStage::OnDraw(){
		auto& camera = GetView()->GetTargetCamera();
	}

	void GameStage::OnDestroy() {
		SoundManager::Instance().StopAll();
	}

	void GameStage::OnEvent(const shared_ptr<Event>& event) {
		auto& msg = event->m_MsgStr;

		if (msg == L"DefeatBoss") {
			m_IsGameStater = 1;
			GameClear();
		}
		else if (msg == L"AppaerBoss") {
			BossAppaerCamera();
		}
		else if (msg == L"ShakeBoss") {
			auto camera = GetView()->GetTargetCamera();;
			camera->ShakeStart(0.5f, 0.5f);
			XINPUT_VIBRATION vibration;
			vibration.wLeftMotorSpeed = 65535;
			vibration.wRightMotorSpeed = 65535;
			XInputSetState(0, &vibration);
			SoundManager::Instance().PlaySE(L"SE_CRUSH");
			PostEvent(0.5f, nullptr, GetThis<Stage>(), L"StopVibration");
		}
		else if (msg == L"SpawnBoss") {
			AddGameObject<BossAppearText>(Vec3(-150, 300, 0.0f), Vec3(300, 100, 1.0f));
		}
		else if (msg == L"DeadPlayer") {
			GameOver();
			GameOverCamera();
			m_IsGameStater = 2;
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
		else if (msg == L"HitStopVibration") {
			XINPUT_VIBRATION vibration;
			vibration.wLeftMotorSpeed = 0;
			vibration.wRightMotorSpeed = 0;
			XInputSetState(0, &vibration);

			GameManager::Instance()->SetGameSpeed(0.5f);
			if (m_cameraState == CameraState::FOLLOWCAMERA)
			{
				auto camera = GetView()->GetTargetCamera();;
				camera->ShakeStart(0.3f, 0.3f);
			}
			PostEvent(0.1f, nullptr, GetThis<Stage>(), L"StopVibration");
		}
		else if (msg == L"HitStop") {
			GameManager::Instance()->SetGameSpeed(0.1f);
			XINPUT_VIBRATION vibration;
			vibration.wLeftMotorSpeed = 65535;
			vibration.wRightMotorSpeed = 65535;
			XInputSetState(0, &vibration);

			PostEvent(0.4f, nullptr, GetThis<Stage>(), L"HitStopVibration");
		}
	}
}
//end basecross
