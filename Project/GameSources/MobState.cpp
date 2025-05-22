/*!
@file Enemy.cpp
@brief
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
		mob->m_BalletInterval = mob->MAX_BALLET_INTERVAL;
		mob->m_ShotRandomInterval = 1.0f; /*Util::RandZeroToOne() * (mob->MAX_BALLET_INTERVAL * 0.5f)*/
		m_BulletRemain = mob->m_BulletRemain;
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
			if (mob->m_BalletInterval < 0.3f && mob->m_ShotRandomInterval < 0.3f && m_BulletEffect != true)
			{
				m_Effect->PlayEffect(m_Eyehandle, L"EnemyEye", Vec3(position.x, position.y + 0.5f, position.z), 0.0f);
				m_Effect->SetRotation(m_Eyehandle, Vec3(0.0f, 1.0f, 0.0f), rotate);
				m_Effect->SetScale(m_Eyehandle, Vec3(0.1f, 0.1f, 0.1f));
				m_Effect->SetEffectSpeed(m_Eyehandle, 2.0f);
				m_Effect->SetAllColor(m_Eyehandle, Col4(1.0, 0, 0, 1.0f));
				m_BulletEffect = true;
			}
			else if (mob->m_BalletInterval <= 0 && mob->m_ShotRandomInterval <= 0) 
			{
				m_Effect->PlayEffect(m_Handle, L"Flash", Vec3(position.x + forward.x / 2, position.y + 0.25f, position.z + forward.z / 2), 8.0f);
				m_Effect->SetRotation(m_Handle, Vec3(0.0f, 1.0f, 0.0f), rotate);
				m_Effect->SetScale(m_Handle, Vec3(0.1f, 0.1f, 0.1f));


				auto ballet = m_Stage->AddGameObject<Bullet>(m_Transform->GetPosition() + direction * mob->m_MuzzleOffset, mob->m_BalletSpeed, direction, mob->m_BalletRange);
				m_BulletEffect = false;

				mob->m_BalletInterval = mob->MAX_BALLET_INTERVAL;

				mob->m_ShotRandomInterval = 1.0f /*Util::RandZeroToOne() * (mob->MAX_BALLET_INTERVAL * 0.5f)*/;
				m_BulletRemain--;
				SoundManager::Instance().PlaySE(L"SE_SHOT");
			}
		}
		else {
			//ÉäÉçÅ[Éh
			m_BulletRelord -= elapsedTime;
			if (m_BulletRelord < 0.0f)
			{
				mob->m_BalletInterval = mob->MAX_BALLET_INTERVAL;
				mob->m_ShotRandomInterval = 1.0f; /*Util::RandZeroToOne() * (mob->MAX_BALLET_INTERVAL * 0.5f)*/
				m_BulletRemain = mob->m_BulletRemain;
				m_BulletEffect = false;
				m_BulletRelord = 3.0f;
			}
		}
	}

	void MobAlert::Exit()
	{
	}

}
