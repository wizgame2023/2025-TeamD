/*!
@file Character.h
@brief キャラクターなど
*/

#pragma once
#include "stdafx.h"
#include "StageBuilder.h"
namespace basecross {
	class BossEnemy;

	class Spawner : public Object {
		int m_Wave;
		int m_EnemyCount;
		vector<shared_ptr<Legion>> m_Legions;
		shared_ptr<BossEnemy> m_Boss;

		Timer m_SpawnTimer;
	public:
		Spawner(const shared_ptr<Stage>& stage);
		virtual  ~Spawner(){}

		virtual void OnCreate()override;
		virtual void OnAfterCreate()override;
		virtual void OnUpdate()override;
		virtual void OnEvent(const shared_ptr<Event>& event)override;


		void AddLegion(const shared_ptr<Legion>& legion);
		void AddEnemy(int wave, const shared_ptr<Enemy>& enemy);
		void SetBoss(const shared_ptr<BossEnemy>& boss) {
			m_Boss = boss;
			m_Boss->SetDrawActive(false);
			m_Boss->SetUpdateActive(false);
		}
		void SpawnEnemy();
		void SpawnBoss();
	};
}

//end basecross
