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
		m_BoneTransform = m_Enemy->GetComponent<Transform>();
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
			Vec3 position = m_BoneTransform->GetPosition();
			Vec3 rot = target - position;
			rot.normalize();
			float rotate = atan2f(rot.x, rot.z);
			m_BoneTransform->SetRotation(Vec3(0, rotate, 0));
			auto mob = dynamic_pointer_cast<Mob>(m_Enemy);
			if (mob->m_BalletInterval <= 0 && mob->m_ShotRandomInterval <= 0) {
				Vec3 direction = m_Enemy->GetDirectionToIntruder();

				auto ballet = m_Stage->AddGameObject<Bullet>(m_BoneTransform->GetPosition() + direction * mob->m_MuzzleOffset, mob->m_BalletSpeed, direction, mob->m_BalletRange);
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
		Execute();
	}
	void BossSearch::Execute()
	{
		auto navi = m_Enemy->GetComponent<Navigate>();
		auto boss = dynamic_pointer_cast<BossEnemy>(m_Enemy);
		auto player = boss->m_Intruder;
		Vec3 playerPos = player->GetPosition();
		if ((boss->GetPosition() - playerPos).length() < 1.0f)
		{
			m_Enemy->ChangeState<BossAttack>();
		}
		else {
			navi->SetTargetPosition(m_Enemy->GetPosition(), m_Enemy->m_Intruder->GetPosition());
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
			navi->SetTargetPosition(m_Enemy->GetPosition(), m_Enemy->m_Intruder->GetPosition());
			m_Enemy->ChangeState<BossSearch>();
			m_ChangeTime = 5.0f;
		}

	}
	void BossAttack::Exit()
	{
	}

}

//end basecross
