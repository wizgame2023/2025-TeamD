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
		MobAreaInterval();
		for (int i = 0; i < m_GruopEnemy.size(); i++)
		{
			auto enemy = m_GruopEnemy[i].lock();
			ReportAlert(enemy->GetIntruderAlert());
			if (m_IntruderAlert == true)
			{
				break;
			}
		}
		if (m_IntruderAlert == true)
		{
			for (int i = 0; i < m_GruopEnemy.size(); i++)
			{
				auto enemy = m_GruopEnemy[i].lock();
				auto mob = dynamic_pointer_cast<Mob>(enemy);
				mob->SetIntruderAlert(true);
				ChangeEnemyMove<MobJoinAlert>(mob);
			}
		}

	}

	void Legion::MobAreaInterval()
	{
		Vec3 nearEnemyPosition = Vec3();
		shared_ptr<Enemy> nearEnemy = nullptr;
		float minDistance = 1.0f; // Mobの一定範囲を定義

		for (int i = 0; i < m_GruopEnemy.size(); i++)
		{
			auto enemy = m_GruopEnemy[i].lock();
			if (!enemy) continue; // 無効なオブジェクトをスキップ

			// 初めのEnemyを基準に設定
			if (nearEnemyPosition == Vec3())
			{
				nearEnemyPosition = enemy->GetPosition();
				nearEnemy = enemy;
			}
			else
			{
				// 他のEnemyが一定範囲内にいるかどうかをチェック
				float distance = (nearEnemyPosition - enemy->GetPosition()).length();
				if (distance < minDistance)
				{
					// Mobを範囲外に移動させる例
					Vec3 direction = (enemy->GetPosition() - nearEnemyPosition);
					direction.normalize();
					enemy->SetPosition(nearEnemyPosition + direction * minDistance);
				}
			}
		}
	}
	template<typename T>
	void Legion::ChangeEnemyMove(const shared_ptr<Mob>& enemy)
	{
		enemy->ChangeState<T>();
	}

}
//end basecross