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
		m_Transform = m_Enemy->GetComponent<Transform>();
		m_Player = m_Enemy->m_Intruder;
	}


	void MobSearch::Enter()
	{
		EnemyState::Enter();
		auto enemy = dynamic_pointer_cast<Mob>(m_Enemy);
		auto navi = enemy->GetComponent<Navigate>(false);
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
				auto group = m_Stage->GetSharedObjectGroup(L"PointerGroup");
				auto pointers = group->GetGroupVector();
				int rnd = static_cast<int>(Util::RandZeroToOne() * (pointers.size() - 1));
				auto pointer = pointers[rnd].lock();
				if (pointer != nullptr) {
					auto point = navi->GetNearPinter(m_Enemy->GetPosition());
					m_Path = navi->FindPathWithWaypoints(point, pointer->GetComponent<Transform>()->GetPosition());
				}
			}
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

		m_IntruderAlert = m_Enemy->GetIntruderAlert();
		if (m_IntruderAlert)
		{
			m_Enemy->ChangeState<MobJoinAlert>();
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
		navi->AvoidBlock(enemy->GetPosition(), m_Player->GetPosition());
		Execute();
	}

	void MobJoinAlert::Execute()
	{
		float elapsedTime = App::GetApp()->GetElapsedTime();
		auto navi = m_Enemy->GetComponent<Navigate>();
		m_IntruderAlert = m_Enemy->GetIntruderAlert();
		if (m_IntruderAlert == true)
		{
			m_Enemy->ChangeState<MobAlert>();
		}
		else {
			Vec3 currntPosition = m_Enemy->GetPosition();
			Vec3 taregtpoint = navi->GetAStarForword(m_Enemy->GetPosition());
			if (taregtpoint != Vec3(0))
			{
				currntPosition += taregtpoint * 3.0f * elapsedTime * m_Enemy->m_ZoneElapsedTime;
			}
			else {
				m_IntruderAlert = m_Enemy->GetIntruderAlert();
				if (m_IntruderAlert == true)
				{
					m_Enemy->ChangeState<MobAlert>();
				}
				else {
					Enter();
				}
			}
			m_Enemy->SetPosition(currntPosition);
		}
	}

	void MobJoinAlert::Exit()
	{

	}
}

//end basecross
