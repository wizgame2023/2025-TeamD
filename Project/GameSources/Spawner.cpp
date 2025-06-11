/*!
@file Character.cpp
@brief キャラクターなど実体
*/

#include "stdafx.h"
#include "Project.h"

namespace basecross {
	Spawner::Spawner(const shared_ptr<Stage>& stage) : Object(stage),m_Wave(0), m_EnemyCount(0), m_SpawnTimer(Timer(1.0f, false)), m_IsSpawn(true) {}

	void Spawner::OnCreate() {
		Object::OnCreate();
	}
	void Spawner::OnUpdate() {
		if (!dynamic_pointer_cast<FollowCamera>(OnGetDrawCamera())) {
			return;
		}
		if (m_Wave == -1 || !m_IsSpawn) return;
		for (auto& wEnemy : m_Legions[m_Wave]->GetEnemyLegionGruop()) {
			auto enemy = wEnemy.lock();
			if (enemy && enemy->GetDrawActive()) {
				auto draw = enemy->GetComponent<BcPNTBoneModelDraw>();
				float color = draw->GetAlpha();
				if (color < 1.0f) {
					color += 1.0f * GetGameElapsed();
				}
				draw->SetAlpha(color);
			}
		}
		m_Boss->SetUpdateActive(false);
		if (m_EnemyCount < m_Legions[m_Wave]->GetMaxCount()) {
			if (m_SpawnTimer.UpdateTimer()) {
				SpawnEnemy();
				m_SpawnTimer.SetTime(1.0f, true);
			}
		}
	}
	void Spawner::OnAfterCreate() {
		if (m_Legions.size() == 0) {
			PostEvent(0.0f, nullptr, GetThis<Spawner>(), L"WaveClear");
			return;
		}
		for (const auto& legion : m_Legions) {
			if (legion->GetEnemyLegionGruop().size() == 0) {
				PostEvent(0.0f, nullptr, GetThis<Spawner>(), L"WaveClear");
			}
		}

		m_Stage->AddGameObject<NextWaveText>(Vec3(-250, 0, 0), m_Wave + 1, m_Legions.size() + 1);
	}

	void Spawner::AddLegion(const shared_ptr<Legion>& legion) {
		m_Legions.push_back(legion);
	}
	void Spawner::AddEnemy(int wave,const shared_ptr<Enemy>& enemy) {
		enemy->SetDrawActive(false);
		enemy->SetUpdateActive(false);
		enemy->GetComponent<BcPNTBoneModelDraw>()->SetAlpha(0.0f);
		m_Legions[wave - 1]->IntoEnemyGruop(enemy);
	}
	void Spawner::SpawnEnemy() {
		if (m_Wave == -1) return;

		auto enemy = m_Legions[m_Wave]->GetEnemy();
		if (enemy != nullptr) {
			enemy->SetDrawActive(true);
			enemy->SetUpdateActive(true);
			enemy->OnSpawn();
			m_EnemyCount++;
		}
	}
	void Spawner::SpawnBoss() {
		m_Boss->SetDrawActive(true);
		m_Boss->SetUpdateActive(true);
		m_Boss->OnSpawn();
	}

	void Spawner::OnEvent(const shared_ptr<Event>& event) {
		if (event->m_MsgStr == L"EnemyDead") {
			m_EnemyCount--;
			if (m_EnemyCount == 0 && m_Legions[m_Wave]->GetEnemyLegionGruop().size() == 0) {
				PostEvent(5.0f, nullptr, GetThis<Spawner>(), L"WaveClear");
				SoundManager::Instance().PlaySE(L"SE_WAVE");
				m_Stage->AddGameObject<NextWaveText>(Vec3(-250, 0, 0), m_Wave + 2, m_Legions.size() + 1);
			}
		}
		else if (event->m_MsgStr == L"WaveClear" && m_Wave != -1) {
			m_Wave++;
			auto player = m_Stage->GetSharedGameObject<Player>(L"Player", false);
			if (player != nullptr)
			{
				player->HealHP(player->GetMaxHP() / 4.0f);
			}
			if (m_Legions.size() <= m_Wave) {

				//カメラ移動
				PostEvent(0.0f, GetThis<ObjectInterface>(), m_Stage, L"AppaerBoss");
				PostEvent(2.5, GetThis<ObjectInterface>(), GetThis<Spawner>(), L"SpawnBoss");
				m_Wave = -1;
				return;
			}
			else {

			}
		}
		else if (event->m_MsgStr == L"SpawnBoss")
		{
			SpawnBoss();
		}
	}
}
//end basecross
