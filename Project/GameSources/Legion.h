/*!
@file Enemymaneger.h
@brief Enemyの管理など
*/

#pragma once
#include "stdafx.h"


namespace basecross {
	class Enemy;
	class Mob;
	template<typename> class EnemyState;
	class Legion : public Object
	{
		float m_PopInterval;
		int m_MaxEnemyCount;
		vector<weak_ptr<Enemy>> m_GruopEnemy;
	public:
		Legion(const shared_ptr<Stage>& stage) : Object(stage),m_PopInterval(0),m_MaxEnemyCount(0) {}
		‾Legion() {};

		virtual void OnCreate()override;
		virtual void OnUpdate()override;

		void MobAreaInterval();

		void IntoEnemyGruop(const shared_ptr<Enemy>& enemy)
		{
			m_GruopEnemy.push_back(enemy);
		}
		shared_ptr<Enemy> GetEnemy() {
			if (m_GruopEnemy.size() == 0) return nullptr;
			auto enemy = m_GruopEnemy[0].lock();
			m_GruopEnemy.erase(m_GruopEnemy.begin());
			if (enemy) {
				return enemy;
			}
			return nullptr;
		}
		void SetPopInterval(float interval) {
			m_PopInterval = interval;
		}
		float GetPopInterval() {
			return m_PopInterval;
		}
		void SetMaxCount(int count) {
			m_MaxEnemyCount = count;
		}
		int GetMaxCount() {
			return m_MaxEnemyCount;
		}

		vector<weak_ptr<Enemy>> GetEnemyLegionGruop()
		{
			return m_GruopEnemy;
		}
	};
}
//end basecross
