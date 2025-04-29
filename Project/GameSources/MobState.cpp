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
		Vec3 currntPosition = enemy->GetPosition();
		float elapsedTime = App::GetApp()->GetElapsedTime();
		//if (navi) {
		//	if (m_Path.size() == 0)
		//	{
		//		auto cellpoint = enemy->RootNaviGate();
		//		auto point = navi->GetNearPointer(enemy->GetPosition());
		//		m_Path = navi->FindPathWithWaypoints(point, cellpoint);
		//	}
		//	else {
		//		m_Path[0].y = pos.y;
		//		Vec3 direction = m_Path[0] - pos;
		//		if (direction.length() < 0.5f) {
		//			m_Path.erase(m_Path.begin());
		//		}
		//		else {
		Vec3 pos = enemy->GetPosition();
		Vec3 direction = m_Player->GetPosition() - pos;
		float rotate = atan2f(direction.x, direction.z);
		m_Transform->SetRotation(Vec3(0, rotate, 0));
		float renge = direction.length();
		if (renge > enemy->m_BalletRange / 2)
		{
			pos += direction.normalize() * 1.0f * elapsedTime * m_Enemy->m_ZoneElapsedTime;
			enemy->SetPosition(pos);
		}
		else {
			m_IntruderAlert = m_Enemy->GetIntruderAlert();
			if (m_IntruderAlert)
			{
				m_Enemy->ChangeState<MobAlert>();
				return;
			}
		}
	}
	void MobSearch::Exit()
	{}

	void MobAlert::Enter()
	{
		EnemyState::Enter();
		auto mob = dynamic_pointer_cast<Mob>(m_Enemy);
		m_BulletRemain = mob->m_BulletRemain;
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

			mob->m_ShotRandomInterval = 1.0f /*Util::RandZeroToOne() * (mob->MAX_BALLET_INTERVAL * 0.5f)*/;
			m_BulletRemain--;
			SoundManager::Instance().PlaySE(L"SE_SHOT");
		}
		if (mob->GetIntruderAlert() == false)
		{
			m_Enemy->ChangeState<MobSearch>();
			return;
		}
		else {
			return;
		}
	}

	void MobAlert::Exit()
	{
	}

	//void MobJoinAlert::Enter()
	//{
	//	EnemyState::Enter();
	//	auto enemy = dynamic_pointer_cast<Mob>(m_Enemy);
	//	auto navi = enemy->GetComponent<Navigate>();
	//	m_AlertTime = 5.0f;
	//	Execute();
	//}

	//void MobJoinAlert::Execute()
	//{
	//	auto navi = m_Enemy->GetComponent<Navigate>(false);
	//	auto enemy = dynamic_pointer_cast<Mob>(m_Enemy);
	//	Vec3 currntPosition = enemy->GetPosition();
	//	Vec3 playerPosition = m_Player->GetPosition();
	//	float elapsedTime = App::GetApp()->GetElapsedTime();
	//	if (navi) {
	//		Vec3 dire = (playerPosition - currntPosition);
	//		dire.normalize();
	//		float rotate = atan2f(dire.x, dire.z);
	//		enemy->SetRotation(Vec3(0, rotate, 0));

	//		if (enemy->GetIntruderAlert() == false)
	//		{
	//			m_AlertTime -= elapsedTime;
	//			if (m_Path.size() == 0)
	//			{
	//				auto point = navi->GetNearPointer(enemy->GetPosition());
	//				auto target = navi->GetNearPointer(m_Player->GetPosition());
	//				m_Path = navi->FindPathWithWaypoints(point, m_Player->GetPosition());
	//			}
	//			else {
	//				m_Path[0].y = currntPosition.y;
	//				Vec3 direction = m_Path[0] - currntPosition;
	//				if (direction.length() < 1.5f) {
	//					m_Path.erase(m_Path.begin());
	//				}
	//				else {
	//					direction = direction.normalize();
	//					float rotate = atan2f(direction.x, direction.z);
	//					m_Transform->SetRotation(Vec3(0, rotate, 0));
	//					currntPosition += direction * 1.5f * elapsedTime * enemy->m_ZoneElapsedTime;
	//					enemy->SetPosition(currntPosition);
	//					if (m_AlertTime < 0.0f)
	//					{
	//						enemy->ChangeState<MobSearch>();
	//						m_AlertTime = 5.0f;
	//						return;
	//					}
	//				}
	//			}
	//		}
	//		else {
	//			if ((currntPosition - playerPosition).length() > 5.0f)
	//			{
	//				currntPosition += dire * 1.5f * elapsedTime * enemy->m_ZoneElapsedTime;
	//				enemy->SetPosition(currntPosition);
	//			}
	//			else {
	//				enemy->ChangeState<MobAlert>();
	//				m_AlertTime = 10.0f;
	//				return;
	//			}
	//		}

	//	}
	//}

	//void MobJoinAlert::Exit()
	//{

	//}
}

//end basecross
