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
		Execute();
	}
	void MobSearch::Execute()
	{
		auto navi = m_Enemy->GetComponent<Navigate>(false);
		auto enemy = dynamic_pointer_cast<Mob>(m_Enemy);
		Vec3 currntPosition = m_Enemy->GetPosition();
		float elapsedTime = App::GetApp()->GetElapsedTime();
		if (navi) {
			Vec3 halfPos = navi->GetAStarForword(currntPosition);
			if (halfPos == Vec3(1, 0, 0))  m_Enemy->SetRotation(Vec3(0, 90, 0));
			if (halfPos == Vec3(-1, 0, 0)) m_Enemy->SetRotation(Vec3(0, 270, 0));
			if (halfPos == Vec3(0, 0, 1))  m_Enemy->SetRotation(Vec3(0, 0, 0));
			if (halfPos == Vec3(0, 0, -1)) m_Enemy->SetRotation(Vec3(0, 180, 0));


			if (halfPos != Vec3(0))
			{
				currntPosition += halfPos * 6.0f * elapsedTime * m_Enemy->m_ZoneElapsedTime;
				m_Enemy->SetPosition(currntPosition);
			}
			else {

				Vec3 pos = enemy->RootNaviGate();
				navi->AvoidBlock(m_Enemy->GetPosition(), pos);
			}
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



	void BossSearch::Enter()
	{
		EnemyState::Enter();
		auto boss = dynamic_pointer_cast<BossEnemy>(m_Enemy);
		auto navi = boss->GetComponent<Navigate>();
		auto player = boss->m_Intruder;
		Vec3 playerPos = player->GetPosition();
		Execute();
	}
	void BossSearch::Execute()
	{
		auto navi = m_Enemy->GetComponent<Navigate>();
		auto boss = dynamic_pointer_cast<BossEnemy>(m_Enemy);
		auto player = boss->m_Intruder;
		Vec3 playerPos = player->GetPosition();

		Vec3 target = m_Player->GetComponent<Transform>()->GetPosition();
		Vec3 position = m_Transform->GetPosition();
		Vec3 rot = target - position;
		rot.normalize();
		float rotate = atan2f(rot.x, rot.z);
		m_Transform->SetRotation(Vec3(0, rotate, 0));

		if ((boss->GetPosition() - playerPos).length() < 2.0f)
		{
			m_Enemy->ChangeState<BossAttack>();
		}
		else {
			//navi->SetTargetPosition(m_Enemy->GetPosition(), m_Enemy->m_Intruder->GetPosition());
		}

	}
	void BossSearch::Exit()
	{

	}
	void BossAttack::Enter()
	{
		EnemyState::Enter();
		m_Stage->AddGameObject<AttackCollision>(m_Transform->GetPosition() + m_Transform->GetForward(), Vec3(3.0f, 1.5f, 3.0f), 3.0f, 1.0f);
		Execute();
	}
	void BossAttack::Execute()
	{
		float elapsedTime = App::GetApp()->GetElapsedTime();
		auto navi = m_Enemy->GetComponent<Navigate>();
		m_ChangeTime -= elapsedTime;

		if (m_ChangeTime < 0.0f)
		{
			//navi->SetTargetPosition(m_Enemy->GetPosition(), m_Enemy->m_Intruder->GetPosition());
			m_Enemy->ChangeState<BossSearch>();
			m_ChangeTime = 5.0f;
		}

	}
	void BossAttack::Exit()
	{
	}

}

//end basecross
