/*!
@file GameStage.cpp
@brief ゲームステージ実体
*/

#include "stdafx.h"
#include "Project.h"

namespace basecross {

	/// <summary>
	/// リソースの作成
	/// </summary>
	void TutorialStage::CreateResource() {
		//auto& app = App::GetApp();
		//auto mediaPath = app->GetDataDirWString();
		//wstring uiPath = mediaPath + L"UI/";
		//wstring texPath = mediaPath + L"Textures/";
		//wstring modelPath = mediaPath + L"Models/";

		//app->RegisterTexture(L"SELECT_SRAGE", uiPath + L"NextStageBack.png");
		//app->RegisterTexture(L"RESULT_TITLE", uiPath + L"ResultToTitle.png");
		////app->RegisterTexture(L"BACKGROUND", texPath + L"TitleBackGround.png");
		GameStage::CreateResource();

	}


	void TutorialStage::ClearSprite() {
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

	void TutorialStage::CreateSelect() {
		m_backGround = AddGameObject<Sprite>(L"RESULT_TITLE", Vec3(-300.0f, -150.0f, 0.0f), Vec2(100.0f, 80.0f), true);
		m_backStage = AddGameObject<Sprite>(L"SELECT_SRAGE", Vec3(-300.0f, -150.0f, 0.0f), Vec2(100.0f, 80.0f), true);
		m_backSprite = AddGameObject<Sprite>(L"RESULT_MENU", Vec3(-300.0f, -150.0f, 0.0f), Vec2(100.0f, 80.0f), true);
		m_backGround->SetDrawActive(false);
		m_backStage->SetDrawActive(false);
		m_backSprite->SetDrawActive(false);

	}


	void TutorialStage::OnCreate() {
		try {
			CreateResource();
			CreateSelect();
			GameStage::OnCreate();
			PostEvent(0.0f, GetThis<ObjectInterface>(), GetThis<GameStage>(), L"AppaerEnemy");

		}
		catch (...) {
			throw;
		}
	}

	void TutorialStage::OnUpdate() {
		auto& app = App::GetApp();
		auto& device = app->GetInputDevice().GetControlerVec()[0];
		GameStage::OnUpdate();
		auto productionCamera = GetSharedGameObject<ProductionCameraman>(L"ProductionCamera", false);

		if (m_BossDead == true && productionCamera->GetEndState()) {
			m_backGround->SetDrawActive(true);
			PostEvent(4.0f, GetThis<ObjectInterface>(), GetThis<GameStage>(), L"DeadBoss");
		}
		if (m_EnemyDead == true && productionCamera->GetEndState()) {
			m_backStage->SetDrawActive(true);
		}


	}

	void TutorialStage::OnEvent(const shared_ptr<Event>& event) {
		auto& msg = event->m_MsgStr;

		if (msg == L"DeadBoss") {
			m_BossDead = false;
			m_backGround->SetDrawActive(false);
		}
		else if (msg == L"AppaerBoss") {
			m_BossDead = true;
		}

		if (msg == L"DeadWave") {
			m_backSprite->SetDrawActive(false);
		}
		else if (msg == L"AppaerWave") {
			m_backSprite->SetDrawActive(true);
		}

		if (msg == L"EnemyDead") {
			m_EnemyDead = false;
			m_backStage->SetDrawActive(false);
		}		
		else if (msg == L"AppaerEnemy") {
			m_EnemyDead = true;
		}

		GameStage::OnEvent(event);

	}

}
//end basecross
