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
		Vec3 pos = enemy->GetPosition();
		float elapsedTime = App::GetApp()->GetElapsedTime();
		shared_ptr<Object> obj;
		auto group = m_Stage->GetSharedObjectGroup(L"Citizen");
		auto groups = group->GetGroupVector();

		Vec3 objDirection = Vec3();
		float objRenge = 0;
		if (groups.size() != 0)
		{
			for (auto citizen : groups)
			{
				auto shObj = citizen.lock();
				Vec3 direction = shObj->GetComponent<Transform>()->GetPosition() - pos;
				float renge = direction.length();
				if (objRenge == 0)
				{
					objRenge = renge;
					objDirection = direction;
					obj = dynamic_pointer_cast<Object>(shObj);
				}
				else if (objRenge > renge)
				{
					objRenge = renge;
					objDirection = direction;
					obj = dynamic_pointer_cast<Object>(shObj);
				}
			}
		}
		//auto obj = m_Stage->GetSharedGameObject<Object>(L"Citizen");
		if (obj != nullptr)
		{
			Vec3 objDirection = obj->GetComponent<Transform>()->GetPosition() - pos;
			if (enemy->m_kariState == Mob::kariState::musi)
			{
				m_IntruderAlert = m_Enemy->GetIntruderAlert();
				if (m_IntruderAlert)
				{
					m_Enemy->ChangeState<MobAlert>();
				}
				else
				{
					float objRotate = atan2f(objDirection.x, objDirection.z);
					m_Transform->SetRotation(Vec3(0, objRotate, 0));
					float objRenge = objDirection.length();
					pos += objDirection.normalize() * 1.0f * elapsedTime * m_Enemy->m_ZoneElapsedTime;
					enemy->SetPosition(pos);
				}
			}
			else if (enemy->m_kariState == Mob::kariState::hakai)
			{
				m_IntruderAlert = m_Enemy->GetIntruderAlert();
				if (m_IntruderAlert)
				{
					m_Enemy->ChangeState<MobAlert>();
				}
				else
				{
					float objRotate = atan2f(objDirection.x, objDirection.z);
					m_Transform->SetRotation(Vec3(0, objRotate, 0));
					float objRenge = objDirection.length();
					if (objRenge > enemy->m_BalletRange / 2)
					{
						pos += objDirection.normalize() * 1.0f * elapsedTime * m_Enemy->m_ZoneElapsedTime;
						enemy->SetPosition(pos);
					}
					else {
						m_Enemy->ChangeState<MobAlert>();
					}
				}

			}
		}
		else
		{
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
				}
			}
		}
	}
	void MobSearch::Exit()
	{}

	void MobAlert::Enter()
	{
		EnemyState::Enter();
		auto mob = dynamic_pointer_cast<Mob>(m_Enemy);
		m_BulletRemain = 0;
		auto stage = static_pointer_cast<GameStage>(m_Stage);
		if (stage != nullptr) {
			m_Effect = stage->GetCreateEffect();
		}
		else {
			m_Effect = nullptr;
		}

		Execute();
	}

	void MobAlert::Execute()
	{

		float elapsedTime = App::GetApp()->GetElapsedTime();
		auto mob = dynamic_pointer_cast<Mob>(m_Enemy);
		Vec3 direction = Vec3();
		m_IntruderAlert = m_Enemy->GetIntruderAlert();
		shared_ptr<Object> obj;
		auto group = m_Stage->GetSharedObjectGroup(L"Citizen");
		auto groups = group->GetGroupVector();
		Vec3 forward = mob->GetForward();
		Vec3 position = mob->GetPosition();
		float rotate = atan2f(forward.x, forward.z);

		Vec3 objDirection = Vec3();
		float objRenge = 0;
		if (groups.size() != 0)
		{

			for (auto citizen : groups)
			{
				auto shObj = citizen.lock();
				Vec3 direction = shObj->GetComponent<Transform>()->GetPosition() - m_Enemy->GetPosition();
				float renge = direction.length();
				if (objRenge == 0)
				{
					objRenge = renge;
					objDirection = direction;
					obj = dynamic_pointer_cast<Object>(shObj);
				}
				else if (objRenge > renge)
				{
					objRenge = renge;
					objDirection = direction;
					obj = dynamic_pointer_cast<Object>(shObj);
				}
			}
		}
		if (m_IntruderAlert)
		{
			mob->AlartMove(m_Player);
			direction = m_Enemy->GetDirectionToIntruderObject(m_Player);
		}
		else if (obj != nullptr)
		{

			mob->AlartMove(obj);
			direction = m_Enemy->GetDirectionToIntruderObject(obj);

			float objRotate = atan2f(objDirection.x, objDirection.z);
			m_Transform->SetRotation(Vec3(0, objRotate, 0));
			if (objRenge > mob->m_BalletRange / 2)
			{
				m_Enemy->ChangeState<MobSearch>();
				return;
			}
		}
		else {
			m_Enemy->ChangeState<MobSearch>();
			return;
		}

		if(m_BulletRemain > 0)
		{
			if (m_BulletEffect)
			{
				m_Effect->PlayEffect(L"EnemyEye", Vec3(position.x + forward.x / 2, position.y + 0.5f, position.z + forward.z / 2), 0.0f);
				m_Effect->SetRotation(Vec3(0.0f, 1.0f, 0.0f), rotate);
				m_Effect->SetScale(Vec3(0.1f, 0.1f, 0.1f));
				m_Effect->SetEffectSpeed(1.5f);
				m_BulletEffect = false;
			}

			if (mob->m_BalletInterval <= 0 && mob->m_ShotRandomInterval <= 0) {
				m_Effect->PlayEffect(L"Flash", Vec3(position.x + forward.x / 2, position.y + 0.25f, position.z + forward.z / 2), 8.0f);
				m_Effect->SetRotation(Vec3(0.0f, 1.0f, 0.0f), rotate);
				m_Effect->SetScale(Vec3(0.1f, 0.1f, 0.1f));


				auto ballet = m_Stage->AddGameObject<Bullet>(m_Transform->GetPosition() + direction * mob->m_MuzzleOffset, mob->m_BalletSpeed, direction, mob->m_BalletRange);
					
				mob->m_BalletInterval = mob->MAX_BALLET_INTERVAL;

				mob->m_ShotRandomInterval = 1.0f /*Util::RandZeroToOne() * (mob->MAX_BALLET_INTERVAL * 0.5f)*/;
				m_BulletRemain--;
				SoundManager::Instance().PlaySE(L"SE_SHOT");
			}
			else if(mob->m_BalletInterval < 0.3f && mob->m_ShotRandomInterval <= 0.3f)
			{
				m_BulletEffect = true;
			}
		}
		else {
			//リロード
			m_BulletRelord -= elapsedTime;
			if (m_BulletRelord < 0.0f)
			{
				m_BulletRemain = mob->m_BulletRemain;
				m_BulletEffect = true;
				m_BulletRelord = 3.0f;
			}
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
