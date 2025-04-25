/*!
@file Character.cpp
@brief キャラクターなど実体
*/

#include "stdafx.h"
#include "Project.h"

namespace basecross {
	Spawner::Spawner(const shared_ptr<Stage>& stage) : Object(stage),m_Wave(0),m_SpawnTimer(Timer(1.0f,false)) {}

	void Spawner::OnCreate() {

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

	void Spawner::AddLegion(const shared_ptr<Legion>& legion) {
		m_Legions.push_back(legion);
	}
	void Spawner::AddEnemy(int wave,const shared_ptr<Enemy>& enemy) {
		enemy->SetDrawActive(false);
		enemy->SetUpdateActive(false);
		m_Legions[wave - 1]->IntoEnemyGruop(enemy);
	}
	void Spawner::SpawnEnemy() {
		if (m_Legions[m_Wave]->GetEnemyLegionGruop().size() == 0) {
			if (m_EnemyCount == 0) {
				m_Wave++;
				if (m_Legions.size() <= m_Wave) {
					SpawnBoss();
					m_Wave = -1;
					return;
				}
			}
			else {
				return;
			}
		}
		auto enemy = m_Legions[m_Wave]->GetEnemy();
		enemy->SetDrawActive(true);
		enemy->SetUpdateActive(true);
		m_EnemyCount++;
	}
	void Spawner::SpawnBoss() {
		m_Boss->SetDrawActive(true);
		m_Boss->SetUpdateActive(true);
	}

	void Spawner::OnEvent(const shared_ptr<Event>& event) {
		if (event->m_MsgStr == L"EnemyDead") {
			m_EnemyCount--;
		}
	}
}
//end basecross
