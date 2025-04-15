/*!
@file Legion.cpp
@brief Enemyの管理
*/

#include "stdafx.h"
#include "Project.h"

namespace basecross {
	void Legion::OnCreate()
	{
		Object::OnCreate();
		auto group = GetStage()->GetSharedObjectGroup(L"Legion");
		group->IntoGroup(GetThis<Legion>());

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
				auto mob = dynamic_pointer_cast<Mob>(enemy);
				ChangeEnemyMove<MobJoinAlert>(mob);
			}
		}
		//else {
		//	for (int i = 0; i < m_GruopEnemy.size(); i++)
		//	{
		//		auto enemy = m_GruopEnemy[i].lock();
		//		auto mob = dynamic_pointer_cast<Mob>(enemy);
		//		ChangeEnemyMove<MobSearch>(mob);
		//	}
		//}

	}

	template<typename T>
	void Legion::ChangeEnemyMove(const shared_ptr<Mob>& enemy)
	{
		enemy->ChangeState<T>();
	}

}
//end basecross