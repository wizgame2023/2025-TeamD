/*!
@file Enemy.cpp
@brief “G‚È‚ÇŽÀ‘Ì
*/

#include "stdafx.h"
#include "Project.h"
#include "MobState.h"

namespace basecross {

	void EnemyState::Enter()
	{
		m_Stage = m_Enemy->GetStage();
		m_Transform = m_Enemy->GetTransfrom();
		m_Player = m_Enemy->m_Intruder;
	}


	void MobSearch::Enter()
	{
		EnemyState::Enter();
		Execute();
	}
	void MobSearch::Execute()
	{
		m_IntruderAlert = m_Enemy->GetIntruderAlert();
		if (m_IntruderAlert == true)
		{
			m_Enemy->ChangeState<MobAlert>();
		}
	}
	void MobSearch::Exit()
	{
	}
	void MobAlert::Enter()
	{
		EnemyState::Enter();
		Execute();
	}
	void MobAlert::Execute()
	{
		m_IntruderAlert = m_Enemy->GetIntruderAlert();
		if (m_IntruderAlert == true)
		{
			Vec3 target = m_Player->GetComponent<Transform>()->GetPosition();
			Vec3 position = m_Transform->GetPosition();

			Vec3 rot = position - target;
			rot.normalize();
			float rotate = atan2f(-rot.x, -rot.z);
			m_Transform->SetRotation(Vec3(0, rotate, 0));
			if (m_Enemy->m_BalletInterval <= 0 && m_Enemy->m_ShotRandomInterval <= 0) {
				Vec3 direction = m_Enemy->GetDirectionToIntruder();

				auto ballet = m_Stage->AddGameObject<Bullet>(m_Transform->GetPosition() + direction * m_Enemy->m_MuzzleOffset, m_Enemy->m_BalletSpeed, direction, m_Enemy->m_BalletRange);
				m_Enemy->m_BalletInterval = m_Enemy->MAX_BALLET_INTERVAL;

				m_Enemy->m_Line->SetBallet(ballet);
				m_Enemy->m_Line->SetDrawActive(true);
				m_Enemy->m_Line = m_Stage->AddGameObject<ForecastLine>(m_Enemy);

				m_Enemy->m_ShotRandomInterval = Util::RandZeroToOne() * (m_Enemy->MAX_BALLET_INTERVAL * 0.5f);
			}
		}
		else {
			m_Enemy->ChangeState<MobSearch>();
		}
	}
	void MobAlert::Exit()
	{
	}
}
//end basecross
