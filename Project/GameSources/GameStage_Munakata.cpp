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
	void GameStageM::CreateViewLight() {
		const Vec3 eye(0.0f, 30.0f, -30.0f);
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
	/// <summary>
	/// リソースの作成
	/// </summary>
	void GameStageM::CreateResource() {
		auto& app = App::GetApp();
		auto mediaPath = app->GetDataDirWString();
		wstring uiPath = mediaPath + L"UI/";
		wstring texPath = mediaPath + L"Textures/";
		wstring modelPath = mediaPath + L"Models/";

		app->RegisterTexture(L"HP_BAR", uiPath + L"Hp.png");
		app->RegisterTexture(L"HP_BAR_E", uiPath + L"EnemyHp.png");
		app->RegisterTexture(L"RESULT_TEXT", uiPath + L"ResultTexts.png");
		app->RegisterTexture(L"RESULT_SCORE", uiPath + L"ResultScoreText.png");
	}
	void GameStageM::RegisterObjects() {
		auto& builder = AddGameObject<StageBuilder>(L"level.csv", 1.0f);
		builder->Register<FixedBox>(L"cube");
		builder->Register<Player>(L"player");
		builder->Register<Mob>(L"mob");
		builder->Register<RootPointer>(L"pointer");

		builder->LoadCsv();
	}
	void GameStageM::OnCreate() {
		try {
			CreateResource();
			GameStage::OnCreate();
			m_PlayerHpBarBackGround = AddGameObject<Sprite>(L"HP_BAR", Vec3(-631.0f, 393.0f, 0.0f), Vec2(400.0f, 24.0f));
			m_PlayerHpBarBackGround->SetDiffuse(Col4(0, 0, 0, 1));

			m_PlayerHpBar = AddGameObject<Sprite>(L"HP_BAR", Vec3(-631.0f, 393.0f, 0.0f), Vec2(400.0f, 24.0f));
			m_PlayerHpBar->SetDiffuse(Col4(0, 1, 0, 1));

			m_PlayerEnergyBarBackGround = AddGameObject<Sprite>(L"HP_BAR", Vec3(-631.0f, 364.0f, 0.0f), Vec2(300.0f, 12.0f));
			m_PlayerEnergyBarBackGround->SetDiffuse(Col4(0, 0, 0, 1));

			m_PlayerEnergyBar = AddGameObject<Sprite>(L"HP_BAR", Vec3(-631.0f, 364.0f, 0.0f), Vec2(300.0f, 12.0f));
			m_PlayerEnergyBar->SetDiffuse(Col4(1, 1, 0, 1));

			m_Fps = AddGameObject<NumberSprite>(L"NUMBER", Vec3(530.0f, 350.0f, 0.0f), Vec2(109.0f, 96.0f), 3);
			SoundManager::Instance().PlayBGM(L"BGM_GAME_PINCH");
		}
		catch (...) {
			throw;
		}
	}

	void GameStageM::OnUpdate() {
		auto& app = App::GetApp();

		auto& device = app->GetInputDevice().GetControlerVec()[0];
		GameStage::OnUpdate();

		auto player = GetSharedGameObject<Player>(L"Player", false);
		if (player != nullptr) {
			float currentHp = player->GetHP();
			float maxHp = player->GetMaxHP();
			m_PlayerHpBar->UpdateSize(Vec3(currentHp / maxHp, 1, 1));

			float currentEnergy = player->GetEnergy();
			m_PlayerEnergyBar->UpdateSize(Vec3(currentEnergy, 1, 1));

			/*RayCastHit hit;
			RayCast::HitTestVec(hit, Line(Vec3(0,1.0f,0), player->GetPosition()), GetGameObjectVec(), { L"Bullet",L"Line",L"Enemy" });*/
		}
		float elpased = app->GetStepTimer().GetFramesPerSecond();
		m_Fps->UpdateNumber(elpased);

		
	}

}
//end basecross
