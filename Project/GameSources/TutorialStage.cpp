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
		auto& app = App::GetApp();
		auto mediaPath = app->GetDataDirWString();
		wstring uiPath = mediaPath + L"UI/";
		wstring texPath = mediaPath + L"Textures/";
		wstring modelPath = mediaPath + L"Models/";

		app->RegisterTexture(L"SELECT_SRAGE", uiPath + L"NextStageBack.png");
		app->RegisterTexture(L"RESULT_TITLE", uiPath + L"ResultToTitle.png");
	}

	void TutorialStage::CreateSelect() {
		backGround = AddGameObject<Sprite>(L"RESULT_TITLE", Vec3(0.0f, 0.0f, 0.0f), Vec2(50.0f, 80.0f), true);
		//backStage = AddGameObject<Sprite>(L"SELECT_SRAGE", Vec3(10.0f, 100.0f, 0.0f), Vec2(100.0f, 50.0f), true);
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

	void TutorialStage::UpdateStop(bool ClearSprite) {
		for (auto& obj : GetGameObjectVec()) {
			//GatUpdateActive(ClearSprite);
			//if (!obj->FindTag(L"")) {
			//	obj->SetUpdateActive(ClearSprite);
			//}
		}

	}


	void TutorialStage::OnCreate() {
		try {
			CreateResource();
			//CreateSelect();
			GameStage::OnCreate();

		}
		catch (...) {
			throw;
		}
	}

	void TutorialStage::OnUpdate() {
		auto& app = App::GetApp();
		auto& device = app->GetInputDevice().GetControlerVec()[0];
		GameStage::OnUpdate();
		//auto enemy = GetSharedGameObject<Mob>(L"mob", true);
		//auto enemyClear = GetSharedGameObject<Mob>(L"enemy", false);
		auto boss = GetSharedGameObject<BossEnemy>(L"BOSS", true);
		auto bossClear = GetSharedGameObject<BossEnemy>(L"BOSS", false);
		//backGround->SetDrawActive(true);

		//if (enemy != nullptr) {}
		//if (enemyClear != nullptr) {}
		//if (boss != nullptr) {}
		//if (bossClear != nullptr) {}
	}

	void TutorialStage::OnEvent(const shared_ptr<Event>& event) {
		auto& msg = event->m_MsgStr;
		GameStage::OnEvent(event);

		if (msg == L"DefeatBoss") {
			backGround->SetDrawActive(false);
		}
		else if (msg == L"AppaerBoss") {
			backGround->SetDrawActive(true);
		}

	}

}
//end basecross
