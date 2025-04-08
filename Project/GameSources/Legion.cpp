/*!
@file Legion.cpp
@brief Enemyの管理
*/

#include "stdafx.h"
#include "Project.h"

namespace basecross {
	void Legion::OnCreate()
	{
	}
	void Legion::OnUpdate()
	{
		for (int i = 0; i < m_GruopEnemy.size(); i++)
		{
			auto enemy = m_GruopEnemy[i].lock();
			ReportAlert(enemy->GetIntruderAlert());
			if (m_IntruderAlert)
			{
				break;
			}
		}
		if (m_IntruderAlert)
		{
			for (int i = 0; i < m_GruopEnemy.size(); i++)
			{
				auto enemy = m_GruopEnemy[i].lock();
				ChangeEnemyMove(enemy);
			}
		}
	}

	void Legion::ChangeEnemyMove(const shared_ptr<Enemy>& enemy)
	{
		enemy->ChangeState<MobJoinAlert>();
	}

}
//end basecross