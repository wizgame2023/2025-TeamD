/*!
@file Character.cpp
@brief キャラクターなど実体
*/

#include "stdafx.h"
#include "Project.h"

namespace basecross {
	Spawner::Spawner(const shared_ptr<Stage>& stage) : Object(stage),m_Wave(0),m_SpawnTimer(Timer(1.0f,false)) {}

	void Spawner::OnCreate() {
		Object::OnCreate();
	}
	void Spawner::OnUpdate() {
		if (m_Wave == -1) return;
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
	}

	void Spawner::AddLegion(const shared_ptr<Legion>& legion) {
		m_Legions.push_back(legion);
	}
	void Spawner::AddEnemy(int wave,const shared_ptr<Enemy>& enemy) {
		enemy->SetDrawActive(false);
		enemy->SetUpdateActive(false);
		m_Legions[wave - 1]->IntoEnemyGruop(enemy);
	}
	void Spawner::SpawnEnemy() {
		if (m_Wave == -1) return;

		auto enemy = m_Legions[m_Wave]->GetEnemy();
		if (enemy != nullptr) {
			enemy->SetDrawActive(true);
			enemy->SetUpdateActive(true);
			m_EnemyCount++;
		}
	}
	void Spawner::SpawnBoss() {
		m_Boss->SetDrawActive(true);
		m_Boss->SetUpdateActive(true);

	}

	void Spawner::OnEvent(const shared_ptr<Event>& event) {
		if (event->m_MsgStr == L"EnemyDead") {
			m_EnemyCount--;
			if (m_EnemyCount == 0 && m_Legions[m_Wave]->GetEnemyLegionGruop().size() == 0) {
				PostEvent(5.0f, nullptr, GetThis<Spawner>(), L"WaveClear");
				SoundManager::Instance().PlaySE(L"SE_WAVE");
			}
		}
		else if (event->m_MsgStr == L"WaveClear") {
			m_Wave++;
			if (m_Legions.size() <= m_Wave) {
				SpawnBoss();
				m_Wave = -1;
				return;
			}
			else {
				
			}
		}
	}
}
//end basecross
