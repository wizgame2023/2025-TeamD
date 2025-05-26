/*!
@file Enemy.cpp
@brief
*/

#include "stdafx.h"
#include "Project.h"

namespace basecross {
	void BossSearch::Enter()
	{
		EnemyState::Enter();
		//m_Line = m_Stage->AddGameObject<LineCube>(0.02f, Col4(0, 1, 0, 1));
	}
	void BossSearch::Execute()
	{
		auto navigate = m_Enemy->GetComponent<Navigate>(false);
		float elapsed = App::GetApp()->GetElapsedTime();
		if (navigate) {
			if (m_Path.size() == 0) {
				m_Line->SetDrawActive(false);
				if (m_OperatorIntarval.UpdateTimer()) {
					float rndOparation = Util::RandZeroToOne() * 100.0f;
					if (rndOparation < 40) {
						auto group = m_Stage->GetSharedObjectGroup(L"PointerGroup");
						auto pointers = group->GetGroupVector();
						int rnd = static_cast<int>(Util::RandZeroToOne() * (pointers.size() - 1));
						auto pointer = pointers[rnd].lock();
						if (pointer != nullptr) {
							m_Path = navigate->FindPathWithWaypoints2(navigate->GetNearPointer(m_Enemy->GetPosition()), pointer->GetComponent<Transform>()->GetPosition());
						}
					}
					else if (rndOparation < 60) {

					}
					else {
						Vec3 target;
						do {
							target = Vec3(Util::RandZeroToOne() * 10.0f, m_Enemy->GetPosition().y, Util::RandZeroToOne() * 10.0f);
						} while (RayCast::HitTestVec(RayCastHit(), Line(m_Enemy->GetPosition(), target), m_Enemy->GetStage()->GetGameObjectVec(), { L"Bullet",L"Line",L"Enemy" }));
						m_Path.push_back(target);
					}
				}
			}
			else {
				m_Line->SetDrawActive(true);
				Vec3 pos = m_Enemy->GetPosition();
				m_Path[0].y = pos.y;
				Vec3 direction = m_Path[0] - pos;
				m_Line->SetLine(Line(pos, m_Path[0]));
				if (direction.length() < 0.1f) {
					m_Path.erase(m_Path.begin());
				}
				else {
					direction = direction.normalize();
					float rotate = atan2f(direction.x, direction.z);
					m_Transform->SetRotation(Vec3(0, rotate, 0));
					m_Enemy->Move(direction);
				}
			}

		}
		m_Enemy->SearchRange();
		if (m_Enemy->GetIntruderAlert()) {
			m_Enemy->ChangeState<BossHostility>();
		}
	}
	void BossSearch::Exit()
	{
		m_Stage->RemoveGameObject<LineCube>(m_Line);
	}

	void BossWarning::Execute()
	{
		BossSearch::Execute();
	}

	void BossHostility::Enter()
	{
		EnemyState::Enter();
		m_Stage->PostEvent(0.0f, nullptr, m_Stage, L"StartBoss");

		m_TargetPosition = m_Enemy->GetPosition() + Vec3(Util::RandZeroToOne() * 10.0f - 5.0f,0.0f, Util::RandZeroToOne() * 10.0f - 5.0f);
		m_LerpStartDirection = m_Enemy->GetForward().normalize();
		m_LerpTargetDirection = m_TargetPosition - m_Enemy->GetPosition();
		m_LerpTargetDirection = m_LerpTargetDirection.normalize();
		m_LerpTime = 0.0f;
	}
	void BossHostility::Execute()
	{
		float elapsedTime = App::GetApp()->GetElapsedTime() * m_Enemy->m_ZoneElapsedTime;
		auto navi = m_Enemy->GetComponent<Navigate>();
		Vec3 position = m_Enemy->GetPosition();
		Vec3 intruderPosition = m_Enemy->m_Intruder->GetPosition();
		Vec3 direction = intruderPosition - position;
		if (m_CooldownTimer.UpdateTimer()) {
			float rnd = Util::RandZeroToOne() * 100.0f;
			if (rnd < 90) {
				m_Enemy->ChangeState<BossGun>();
			}
			else {
				m_Enemy->ChangeState<BossCrush>();
			}
		}
		else {
			Vec3 newDirection = Lerp::CalculateLerp(m_LerpStartDirection, m_LerpTargetDirection, 0.0f, 1.0f, m_LerpTime, Lerp::rate::Linear);
			float rotationY = atan2f(newDirection.x, newDirection.z);
			m_Enemy->SetRotation(Vec3(0, rotationY, 0));
			
			if (m_LerpTargetDirection == newDirection) {
				m_Enemy->Move(m_LerpTargetDirection);
			}
			else {
				m_LerpTime += elapsedTime;
			}
		}
	}
	void BossHostility::Exit()
	{
		//m_Stage->PostEvent(0.0f, nullptr, m_Stage, L"EndBoss");
	}

	void BossCrush::Enter()
	{
		EnemyState::Enter();
		m_Attack = m_Enemy->m_Cruch;
		//m_Enemy->SetAnimation()
	}
	void BossCrush::Execute()
	{
		float elapsed = App::GetApp()->GetElapsedTime();
		Vec3 position = m_Enemy->GetPosition();
		Vec3 intruderPosition = m_Enemy->m_Intruder->GetPosition();

		Vec3 direction = intruderPosition - position;
		float distance = direction.length();
		direction = direction.normalize();

		if (!m_IsFinish) {
			if (m_Attack->IsInRange(distance) && !m_IsReady) {
				Ready(1.0f);
				m_AttackPosition = m_Enemy->GetPosition() + direction.normalize() * 1.0f + cross(Vec3(0, 1, 0), direction) * 0.5f;
				m_Stage->AddGameObject<AreaOfEffect>(m_AttackPosition, m_Attack->GetScale().x, 36, 1.0f);

				m_Enemy->SetAnimation(L"Crush");
			}
			if (m_IsReady) {
				if (m_ReadyTimer.UpdateTimer()) {
					SoundManager::Instance().PlaySE(L"SE_CRUSH");
					m_Attack->Play(m_AttackPosition);
					m_CooldownTimer.SetTime(m_Attack->GetCharaCooldown(), true);
					m_IsFinish = true;
					m_FinishedForward = m_Enemy->GetForward();

					Effekseer::Handle handle;
					m_Enemy->m_Effect->PlayEffect(handle,L"Trampling", m_AttackPosition, 0.0f);
					m_Enemy->m_Effect->SetScale(handle,m_Attack->GetSize() / 8.0f);
				}
			}
			else {
				float rotationY = atan2f(direction.x, direction.z);
				m_Enemy->SetRotation(Vec3(0, rotationY, 0));
				m_Enemy->Move(direction);
			}
		}
		else {
			if (m_CooldownTimer.UpdateTimer()) {
				if (LerpRotatePlayer(direction)) {
					m_Enemy->ChangeState<BossHostility>();
				}
			}
		}
	}
	void BossCrush::Exit()
	{

	}
	void BossCrush::Ready(float time) {
		AttackState::Ready(time);
	}

	void BossGun::Enter()
	{
		EnemyState::Enter();
		m_Attack = m_Enemy->m_Missile;
	}
	void BossGun::Execute()
	{
		Vec3 position = m_Enemy->GetPosition();
		Vec3 intruderPosition = m_Enemy->m_Intruder->GetPosition();

		Vec3 direction = intruderPosition - position;
		float distance = direction.length();
		direction = direction.normalize();
		if (!m_IsFinish) {
			if (m_Attack->IsInRange(distance) && !m_IsReady) {
				m_Enemy->SetAnimation(L"Missile_First");
				auto gravity = m_Enemy->GetComponent<Gravity>();
				gravity->StartJump((-direction + Vec3(0, 0.3f, 0)) * 5.0f);
				Ready(0.5f);
			}
			if (m_IsReady) {
				if (m_Attack->IsFinish()) {
					m_CooldownTimer.SetTime(m_Attack->GetCharaCooldown(), true);
					m_IsFinish = true;
					m_FinishedForward = m_Enemy->GetForward();
					m_Enemy->SetAnimation(L"Missile_Finish");
				}
				else if (m_ReadyTimer.UpdateTimer() && !m_Attack->GetDrawActive() && m_Enemy->GetAnimationFinish()) {
					m_Attack->Play(position + Vec3(0, m_Enemy->GetScale().y * 2.0f, 0.0f));
					m_Enemy->SetAnimation(L"Missile");
				}
			}
			else {
				m_Enemy->Move(direction);
			}
			float rotationY = atan2f(direction.x, direction.z);
			m_Enemy->SetRotation(Vec3(0, rotationY, 0));
		}
		else {

			if (m_Enemy->GetAnimationFinish()) {
				m_Enemy->SetAnimation(L"Idle");
			}
			if (m_Enemy->GetCurrentAnimationKey() == L"Idle" && LerpRotatePlayer(direction)) {
				m_Enemy->ChangeState<BossHostility>();
			}
		}
	}
	void BossGun::Exit()
	{

	}
}

//end basecross
