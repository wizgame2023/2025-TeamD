/*!
@file Enemy.cpp
@brief
*/

#include "stdafx.h"
#include "Project.h"

namespace basecross {
	void BossStarting::Enter() {
		EnemyState::Enter();
		m_Enemy->SetAnimation(L"Jump");
	}
	void BossStarting::Execute() {
		if (m_Enemy->GetCurrentAnimationKey() == L"Landing") {
			if (m_Enemy->GetAnimationFinish()) {
				auto cameraman = m_Stage->GetSharedGameObject<ProductionCameraman>(L"ProductionCamera", false);
				if (cameraman != nullptr) {
					if (cameraman->GetEndState()) {
						m_Enemy->ChangeState<BossHostility>();
						return;
					}
				}
			}
		}
		if (m_Enemy->GetCurrentAnimationKey() == L"Landing_First") {
			if (m_Enemy->GetAnimationFinish()) {
				auto& stage = dynamic_pointer_cast<GameStage>(m_Stage);
				auto& effect = stage->GetCreateEffect();
				effect->PlayEffect(m_SmokeHandle, L"Trampling", m_Enemy->GetPosition() - Vec3(0, 1.5f, 0), 0.0f);
				effect->SetScale(m_SmokeHandle, Vec3(0.5f));
				effect->SetEffectSpeed(m_SmokeHandle, 0.4f);
				m_Enemy->SetAnimation(L"Landing");
			}
		}
	}

	void BossStarting::Exit() {

	}

	void BossHostility::Enter()
	{
		EnemyState::Enter();
		m_Stage->PostEvent(0.0f, nullptr, m_Stage, L"StartBoss");

		Difficulty difficulty = GameManager::Instance()->GetDifficulty();
		float addRate = max(1.0f, (int)difficulty * 0.75f);
		m_CooldownTimer.SetTime(2.5f / addRate, true);

		Vec3 position = m_Enemy->GetPosition();
		Vec3 intruderPosition = m_Enemy->m_Intruder->GetPosition();
		Vec3 direction = intruderPosition - position;
		float distance = direction.length();
		
		if (distance < 5.0f) {
			auto gravity = m_Enemy->GetComponent<Gravity>(false);
			if (gravity) {
				direction = direction.normalize();
				gravity->StartJump((-direction + Vec3(0, 0.1f, 0)) * 4.0f);
			}
		}
	}
	void BossHostility::Execute()
	{
		float elapsedTime = App::GetApp()->GetElapsedTime() * GameManager::Instance()->GetTimeRate();
		Vec3 position = m_Enemy->GetPosition();
		Vec3 intruderPosition = m_Enemy->m_Intruder->GetPosition();
		Vec3 direction = intruderPosition - position;
		float distance = direction.length();
		direction = direction.normalize();

		if (m_CooldownTimer.UpdateTimer(GameManager::Instance()->GetTimeRate())) {
			float rnd = Util::RandZeroToOne() * 100.0f;
			if (rnd < 40.0f) {
				m_Enemy->ChangeState<BossGun>();
			}
			else {
				m_Enemy->ChangeState<BossCrush>();
			}
		}
		else {
			float rotationY = atan2f(direction.x, direction.z);
			m_Enemy->SetRotation(Vec3(0, rotationY, 0));
			if (distance > 5.0f) {
				m_Enemy->Move(direction);
			}
			else {
				m_Enemy->Move(-direction / 2.0f);
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
		float elapsed = App::GetApp()->GetElapsedTime() * GameManager::Instance()->GetTimeRate();
		Vec3 position = m_Enemy->GetPosition();
		Vec3 intruderPosition = m_Enemy->m_Intruder->GetPosition();

		Vec3 direction = intruderPosition - position;
		float distance = direction.length();
		direction = direction.normalize();

		m_Enemy->m_Effect->SetEffectSpeed(m_SmokeHandle, 1.0f * GameManager::Instance()->GetTimeRate());
		if (!m_IsFinish) {
			if (m_Attack->IsInRange(distance) && !m_IsReady) {
				Ready(1.0f / m_Enemy->GetMotionRate());
				m_AttackPosition = m_Enemy->GetPosition() + direction.normalize() * 1.0f + cross(Vec3(0, 1, 0), direction) * 0.5f - Vec3(0, 1.0f, 0);
				m_Stage->AddGameObject<AreaOfEffect>(m_AttackPosition, m_Attack->GetScale().x, 36, 1.0f / m_Enemy->GetMotionRate());

				m_Enemy->SetAnimation(L"Crush");
			}
			if (m_IsReady) {
				if (m_ReadyTimer.UpdateTimer(GameManager::Instance()->GetTimeRate())) {
					SoundManager::Instance().PlaySE(L"SE_CRUSH");
					m_Attack->Play(m_AttackPosition);
					m_CooldownTimer.SetTime(m_Attack->GetCharaCooldown(), true);
					m_IsFinish = true;
					m_FinishedForward = m_Enemy->GetForward();

					m_Enemy->m_Effect->PlayEffect(m_SmokeHandle,L"Trampling", m_AttackPosition, 0.0f);
					m_Enemy->m_Effect->SetScale(m_SmokeHandle,m_Attack->GetSize() / 8.0f);
				}
			}
			else {
				float rotationY = atan2f(direction.x, direction.z);
				m_Enemy->SetRotation(Vec3(0, rotationY, 0));
				m_Enemy->Move(direction);
			}
		}
		else {
			if (m_CooldownTimer.UpdateTimer(GameManager::Instance()->GetTimeRate())) {
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
				gravity->StartJump((-direction + Vec3(0, 0.3f / m_Enemy->GetMotionRate(), 0)) * 5.0f * m_Enemy->GetMotionRate());
				Ready(0.5f * m_Enemy->GetMotionRate());
			}
			if (m_IsReady) {
				if (m_Attack->IsFinish()) {
					m_CooldownTimer.SetTime(m_Attack->GetCharaCooldown(), true);
					m_IsFinish = true;
					m_FinishedForward = m_Enemy->GetForward();
					m_Enemy->SetAnimation(L"Missile_Finish");
				}
				else if (m_ReadyTimer.UpdateTimer(GameManager::Instance()->GetTimeRate()) && !m_Attack->GetDrawActive() && m_Enemy->GetAnimationFinish()) {
					m_Attack->Play(position + Vec3(0, m_Enemy->GetScale().y / 2.0f, 0.0f));
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

	void BossShakeOff::Enter()
	{
		EnemyState::Enter();
		//m_Attack = m_Enemy->m_Missile;

	}
	void BossShakeOff::Execute()
	{
		Vec3 position = m_Enemy->GetPosition();
		Vec3 intruderPosition = m_Enemy->m_Intruder->GetPosition();

		Vec3 direction = intruderPosition - position;
		float distance = direction.length();
		direction = direction.normalize();

		if (!m_IsFinish) {
			if (m_Attack->IsInRange(distance) && !m_IsReady) {
				m_Enemy->SetAnimation(L"ShakeOff_First");
				Ready(0.5f * m_Enemy->GetMotionRate());
			}
			if (m_IsReady) {
				if (m_ReadyTimer.UpdateTimer(GameManager::Instance()->GetTimeRate()) && !m_Attack->GetDrawActive() && m_Enemy->GetAnimationFinish()) {
					m_Attack->Play(position + Vec3(0, m_Enemy->GetScale().y / 2.0f, 0.0f));
					m_Enemy->SetAnimation(L"ShakeOff");
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
	void BossShakeOff::Exit()
	{

	}
}

//end basecross
