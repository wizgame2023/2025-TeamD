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
			Vec3 rot = target - position;
			rot.normalize();
			float rotate = atan2f(rot.x, rot.z);
			m_Transform->SetRotation(Vec3(0, rotate, 0));
			auto mob = dynamic_pointer_cast<Mob>(m_Enemy);
			if (mob->m_BalletInterval <= 0 && mob->m_ShotRandomInterval <= 0) {
				Vec3 direction = m_Enemy->GetDirectionToIntruder();

				auto ballet = m_Stage->AddGameObject<Bullet>(m_Transform->GetPosition() + direction * mob->m_MuzzleOffset, mob->m_BalletSpeed, direction, mob->m_BalletRange);
				mob->m_BalletInterval = mob->MAX_BALLET_INTERVAL;

				m_Enemy->m_Line->SetBallet(ballet);
				m_Enemy->m_Line->SetDrawActive(true);
				m_Enemy->m_Line = m_Stage->AddGameObject<ForecastLine>(m_Enemy);

				mob->m_ShotRandomInterval = Util::RandZeroToOne() * (mob->MAX_BALLET_INTERVAL * 0.5f);

				SoundManager::Instance().PlaySE(L"SE_SHOT");
			}
		}
		else {
			m_Enemy->ChangeState<MobSearch>();
		}
	}
	void MobAlert::Exit()
	{
	}


	void BossSearch::Enter()
	{
		EnemyState::Enter();
		auto boss = dynamic_pointer_cast<BossEnemy>(m_Enemy);
		auto navi = boss->GetComponent<Navigate>();
		auto player = boss->m_Intruder;
		Vec3 playerPos = player->GetPosition();
		navi->SetTargetPosition(m_Transform->GetPosition(), playerPos);
		Execute();
	}
	void BossSearch::Execute()
	{
		auto boss = dynamic_pointer_cast<BossEnemy>(m_Enemy);
		auto navi = boss->GetComponent<Navigate>();
		float elapsedTime = App::GetApp()->GetElapsedTime();
		Vec3 currntPosition = boss->GetPosition();
		Vec3 halfPos = navi->GetAStarForword(currntPosition);
		if (halfPos == Vec3(1, 0, 0))  m_Transform->SetRotation(Vec3(0, 90, 0));
		if (halfPos == Vec3(-1, 0, 0)) m_Transform->SetRotation(Vec3(0, 270, 0));
		if (halfPos == Vec3(0, 0, 1))  m_Transform->SetRotation(Vec3(0, 0, 0));
		if (halfPos == Vec3(0, 0, -1)) m_Transform->SetRotation(Vec3(0, 180, 0));

		if (halfPos != Vec3(0))
		{
			currntPosition += halfPos * 6.0f * elapsedTime * m_Enemy->m_ZoneElapsedTime;
			m_Transform->SetPosition(currntPosition);
		}
		else {
			m_Enemy->ChangeState<BossAttack>();
		}


	}
	void BossSearch::Exit()
	{

	}
	void BossAttack::Enter()
	{
		EnemyState::Enter();
		Execute();
	}
	void BossAttack::Execute()
	{
		m_Stage->AddGameObject<AttackCollision>(m_Transform->GetPosition() + m_Transform->GetForward(), Vec3(3.0f, 1.5f, 3.0f), 3.0f, 1.0f);
	}
	void BossAttack::Exit()
	{
	}
}
//end basecross
