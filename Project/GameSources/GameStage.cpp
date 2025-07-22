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

		File::Load(L"Load", L"Textures/");
		File::Load(L"Load", L"UI/");
		m_Effect = AddGameObject<EffectManager>();
		File::LoadEfk(L"Load", L"Effekt/", m_Effect);
	}

	shared_ptr <EffectManager> GameStage::GetCreateEffect()
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
		auto player = GetSharedGameObject<Player>(L"Player", false);


		Vec3 bossHpPosition = Vec3(-400.0f, 400.0f - 40.0f, 0.0f);
		Vec3 playerHpPosition = Vec3(-270.0f, -353.0f, 0.0f);

		m_NormalIcon = AddGameObject<NormalIcon>(L"ACTION_PANCH", Vec3(410.0f, -257.0f, 0.0f), Col4(1, 1, 1, 0.5f), Col4(1, 1, 1, 1.0f), 0.5f);
		m_NormalIcon->SetCheck(bind(&Player::IsAttack, player));
		m_Icon = AddGameObject<NormalIcon>(L"ACTION_DASH", Vec3(287.0f, -158.0f, 0.0f), Col4(1, 1, 1, 0.5f), Col4(1, 1, 1, 1.0f), 0.5f);
		m_Icon->SetCheck(bind(&Player::IsDash,player));

		m_NormalIconKey = AddGameObject<NormalIcon>(L"ACTION_PANCH_KEY", Vec3(410.0f, -257.0f, 0.0f), Col4(1, 1, 1, 0.5f), Col4(1, 1, 1, 1.0f), 0.5f);
		m_NormalIconKey->SetCheck(bind(&Player::IsAttack, player));

		m_IconKey = AddGameObject<NormalIcon>(L"ACTION_DASH_KEY", Vec3(287.0f, -158.0f, 0.0f), Col4(1, 1, 1, 0.5f), Col4(1, 1, 1, 1.0f), 0.5f);
		m_IconKey->SetCheck(bind(&Player::IsDash, player));

		m_UltIcon = AddGameObject<UltIcon>();

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
		m_BossText->SetDiffuse(Col4(1, 1, 1, 1));

		fadeSprite = AddGameObject<Sprite>(L"FADE", Vec3(0.0f, 0.0f, 0.0f), Vec2(1480.0f, 880.0f), Vec2(0.5f));
		fadeSprite->SetDrawLayer(1);
		m_Fade = fadeSprite->AddComponent<SpriteFade>(1.0f);
		m_Fade->FadeOut();
		m_Fade->Stop();
		auto& device = App::GetApp()->GetInputDevice().GetControlerVec()[0];
		if (device.bConnected) {
			m_NormalIconKey->SetDraw(false);
			m_IconKey->SetDraw(false);
			m_NormalIcon->SetDraw(true);
			m_Icon->SetDraw(true);
			//m_MenuIconKey->SetDraw(false);
		}
		else {
			m_NormalIconKey->SetDraw(true);
			m_IconKey->SetDraw(true);
			m_NormalIcon->SetDraw(false);
			m_Icon->SetDraw(false);
			//m_MenuIconKey->SetDraw(true);
		}
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
				camera->ResetCursorPosition();
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
			Vec3 AtEndPos = Playpos + Vec3(0.0f, 0.0f, 0.0f);
			Vec3 CameraPos = Playpos + Vec3(0.0f, -0.5f, -dire/ 1.5f);
			Vec3 CameraEndPos = Playpos + Vec3(0.0f, 1.0f, -dire * 1.25f);
			m_cameraState = CameraState::OPENINGCAMERA;
			auto ptrOpeningCameraman = AddGameObject<ProductionCameraman>();
			ptrOpeningCameraman->SetReverse(false);
			ptrOpeningCameraman->SetMoveType(ProductionCameraman::MoveType::Orbit);
			ptrOpeningCameraman->StartOpeningAnimation(
				CameraPos, CameraEndPos, 
				AtPos, AtEndPos, 
				-CameraPos, AtEndPos, 
				4.0f, 0.0f,true);

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
		//m_NormalIcon->SetDraw(false);
		//m_Icon->SetDraw(false);
		//m_UltIcon->SetDraw(false);
		m_UltEnege = 0.0f;
		m_UltIcon->SetCharge(m_UltEnege);
		//m_PlayerHpBar->SetDrawActive(false);
		//m_BossStunBar->SetDrawActive(false);
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
				cameraStartPos,	animEndPos1+ side, 
				cameraStartAt,	animAtPos - side,  
				animEndPos2, animAtPos,
				3.0f,0.0f,false);

			auto ptrOpeningCamera = static_pointer_cast<ProductionCamera>(m_ProductionCameraView->GetCamera());
			if (ptrOpeningCamera) {
				SetView(m_ProductionCameraView);
				ptrOpeningCamera->SetCameraObject(productionCamera);
			}		
			m_cameraState = CameraState::RESULTCAMERA;
		}
	}

	void GameStage::UIDraw(){
		if (m_cameraState == CameraState::FOLLOWCAMERA){
			auto& device = App::GetApp()->GetInputDevice().GetControlerVec()[0];
			if (device.bConnected){
				m_NormalIconKey->SetDraw(false);
				m_IconKey->SetDraw(false);
				m_NormalIcon->SetDraw(true);
				m_Icon->SetDraw(true);
			}
			else {
				m_NormalIconKey->SetDraw(true);
				m_IconKey->SetDraw(true);
				m_NormalIcon->SetDraw(false);
				m_Icon->SetDraw(false);

			}
			m_UltIcon->SetDraw(true);
			m_PlayerHpBar->SetDrawActive(true);
			m_BossStunBar->SetDrawActive(true);
		}
		else {
			m_NormalIcon->SetDraw(false);
			m_Icon->SetDraw(false);
			m_NormalIconKey->SetDraw(false);
			m_IconKey->SetDraw(false);
			m_UltIcon->SetDraw(false);
			m_PlayerHpBar->SetDrawActive(false);
			m_BossStunBar->SetDrawActive(false);
			m_BossHpBar->SetDrawActive(false);
			m_BossText->SetDrawActive(false);
		}
	}

	void GameStage::GameOver() {
		m_UltEnege = 0.0f;
		m_UltIcon->SetCharge(m_UltEnege);
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
				CameraPos,CameraEndPos,
				Playpos, Playpos + (-Playrot / 2),
				-CameraPos,Playpos,
				3.0f,0.0f,false);

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
			Vec3 bossPos = boss->GetPosition();
			Vec3 bossRot = boss->GetForward() * 7;
			Vec3 CameraEndPos = bossPos + Vec3(0.0f, -bossPos.y + 2.0f, 0.0f);

			auto productionCamera = GetSharedGameObject<ProductionCameraman>(L"ProductionCamera", false);
			productionCamera->SetReverse(true);
			productionCamera->SetMoveType(ProductionCameraman::MoveType::Linear);

			productionCamera->StartOpeningAnimation(
				camera->GetEye(),CameraEndPos - bossRot,
				player->GetPosition(),CameraEndPos,
				-camera->GetEye(),bossPos,
				4.5f,0.0f,true);

			auto ptrOpeningCamera = static_pointer_cast<ProductionCamera>(m_ProductionCameraView->GetCamera());
			if (ptrOpeningCamera) {
				SetView(m_ProductionCameraView);
				ptrOpeningCamera->SetCameraObject(productionCamera);
			}
			m_cameraState = CameraState::OPENINGCAMERA;
		}

	}

	void GameStage::ResetClearStage() {
		App::GetApp()->GetScene<Scene>()->ResetStage();
		PostEvent(0.0f, nullptr, App::GetApp()->GetScene<Scene>(), L"ToTitleStage");

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
			GameManager::Instance()->SetZoneRate(0.5f);

			RECTANGLE r = { 0,0,5,4 };
			CIRCLE c = { 5.0f,36,360.0f };
			AddGameObject<GroundEffect>(L"SEARCH_RANGE",r);
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
		auto keyState = App::GetApp()->GetInputDevice().GetKeyState();
		auto productionCamera = GetSharedGameObject<ProductionCameraman>(L"ProductionCamera", false);
		auto player = GetSharedGameObject<Player>(L"Player", false);
		UIDraw();
		if ((device.wPressedButtons & XINPUT_GAMEPAD_START && device.wPressedButtons & XINPUT_GAMEPAD_BACK)) {
			ResetClearStage();
		}
		if ((device.wPressedButtons & XINPUT_GAMEPAD_START || keyState.m_bPushKeyTbl[VK_TAB]) && m_cameraState == CameraState::FOLLOWCAMERA) {
			m_Camera->SetCameraPause(true);
			m_SoundTestMenu->Close();
			m_PauseMenu->Open();
			m_Effect->SetEffectPause(true);
			player->SetIsGaol(true);
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
				if (player != nullptr) {
					m_UltIcon ->SetCharge(player->GetEnergy());
				}
			}
			auto boss = GetSharedGameObject<BossEnemy>(L"BOSS", false);
			if (boss != nullptr) {
				ScoreManager::Instance()->UpdateTime(elapsed);
				bool isBossDraw = boss->IsArive();
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
		else if (msg == L"ContorStop") {
			GameManager::Instance()->SetGameSpeed(0.1f);
			XINPUT_VIBRATION vibration;
			vibration.wLeftMotorSpeed = 65535;
			vibration.wRightMotorSpeed = 65535;
			XInputSetState(0, &vibration);
			if (m_cameraState == CameraState::FOLLOWCAMERA)
			{
				auto camera = GetView()->GetTargetCamera();;
				static_pointer_cast<FollowCamera>(camera)->LookAtNearestEnemy();
			}
			PostEvent(0.0f, nullptr, GetThis<Stage>(), L"HitStopVibration");

		}
	}
}
//end basecross
