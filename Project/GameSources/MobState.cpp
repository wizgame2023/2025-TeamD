/*!
@file Enemy.cpp
@brief “G‚È‚ÇŽÀ‘Ì
*/

#include "stdafx.h"
#include "Project.h"
#include "MobState.h"

namespace basecross {



	void MobSearch::Enter()
	{
		EnemyState::Enter();
		auto enemy = dynamic_pointer_cast<Mob>(m_Enemy);
		auto navi = enemy->GetComponent<Navigate>(false);
		m_Path = {};
		Execute();
	}
	void MobSearch::Execute()
	{
		auto navi = m_Enemy->GetComponent<Navigate>(false);
		auto enemy = dynamic_pointer_cast<Mob>(m_Enemy);
		Vec3 currntPosition = m_Enemy->GetPosition();
		float elapsedTime = App::GetApp()->GetElapsedTime();
		if (navi) {
			if (m_Path.size() == 0)
			{
				auto cellpoint = m_Enemy->RootNaviGate();
				auto point = navi->GetNearPinter(m_Enemy->GetPosition());
				m_Path = navi->FindPathWithWaypoints(point, cellpoint);
			}
			else {
				Vec3 pos = m_Enemy->GetPosition();
				m_Path[0].y = pos.y;
				Vec3 direction = m_Path[0] - pos;
				if (direction.length() < 0.1f) {
					m_Path.erase(m_Path.begin());
				}
				else {
					direction = direction.normalize();
					float rotate = atan2f(direction.x, direction.z);
					m_Transform->SetRotation(Vec3(0, rotate, 0));
					pos += direction * 1.0f * elapsedTime * m_Enemy->m_ZoneElapsedTime;
				}
				m_Enemy->SetPosition(pos);
			}
		}

		m_IntruderAlert = m_Enemy->GetIntruderAlert();
		if (m_IntruderAlert)
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
		Vec3 target = m_Player->GetComponent<Transform>()->GetPosition();
		Vec3 position = m_Transform->GetPosition();
		Vec3 rot = target - position;
		rot.normalize();
		float rotate = atan2f(rot.x, rot.z);
		m_Transform->SetRotation(Vec3(0, rotate, 0));
		auto mob = dynamic_pointer_cast<Mob>(m_Enemy);
		if (mob->m_BalletInterval <= 0 && mob->m_ShotRandomInterval <= 0) {
			Vec3 direction = m_Enemy->GetDirectionToIntruder();

			auto ballet = m_Stage->AddGameObject<Bullet>(m_Transform->GetPosition() + direction * mob->m_MuzzleOffset, mob->m_BalletSpeed, direction, mob->m_BalletRange);
			mob->m_BalletInterval = mob->MAX_BALLET_INTERVAL;

			mob->m_ShotRandomInterval = Util::RandZeroToOne() * (mob->MAX_BALLET_INTERVAL * 0.5f);

			SoundManager::Instance().PlaySE(L"SE_SHOT");
		}
		m_IntruderAlert = m_Enemy->GetIntruderAlert();
		if (m_IntruderAlert == false)
		{
			m_Enemy->ChangeState<MobSearch>();
		}

	}
	void MobAlert::Exit()
	{
	}

	void MobJoinAlert::Enter()
	{
		EnemyState::Enter();
		auto enemy = dynamic_pointer_cast<Mob>(m_Enemy);
		auto navi = enemy->GetComponent<Navigate>();
		Execute();
	}

	void MobJoinAlert::Execute()
	{
		auto navi = m_Enemy->GetComponent<Navigate>(false);
		auto enemy = dynamic_pointer_cast<Mob>(m_Enemy);
		Vec3 currntPosition = m_Enemy->GetPosition();
		float elapsedTime = App::GetApp()->GetElapsedTime();
		if (navi) {
			auto point = navi->GetNearPinter(m_Enemy->GetPosition());
			auto target = navi->GetNearPinter(m_Player->GetPosition());
			if (m_Path.size() == 0)
			{
				m_Path = navi->FindPathWithWaypoints(point, m_Player->GetPosition());
			}
			else {
				Vec3 pos = m_Enemy->GetPosition();
				m_Path[0].y = pos.y;
				Vec3 direction = m_Path[0] - pos;
				if (direction.length() < 0.1f) {
					m_Path.erase(m_Path.begin());
				}
				else {
					direction = direction.normalize();
					float rotate = atan2f(direction.x, direction.z);
					m_Transform->SetRotation(Vec3(0, rotate, 0));
					pos += direction * 3.0f * elapsedTime * m_Enemy->m_ZoneElapsedTime;
				}
				m_Enemy->SetPosition(pos);
			}
			if ((point->GetPosition() - target->GetPosition()).length() < 1.0f)
			{
				m_Enemy->ChangeState<MobAlert>();
			}

		}
	}

	void MobJoinAlert::Exit()
	{

	}
}

//end basecross
