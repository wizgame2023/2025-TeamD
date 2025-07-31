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

	int BossHostility::m_MissileCount = 0;

	void BossHostility::Enter()
	{
		EnemyState::Enter();
		m_Stage->PostEvent(0.0f, nullptr, m_Stage, L"StartBoss");

		Difficulty difficulty = GameManager::GetInstance().GetDifficulty();
		float addRate = max(1.0f, (int)difficulty * 0.75f);
		m_CooldownTimer.SetTime(2.5f / addRate, true);

		Vec3 position = m_Enemy->GetPosition();
		Vec3 intruderPosition = m_Enemy->m_Intruder->GetPosition();
		Vec3 direction = intruderPosition - position;
		float distance = direction.length();
		
		if (distance < 5.0f) {
			auto gravity = m_Enemy->GetComponent<Gravity>(false);
			if (gravity) {
				float offset = Util::RandZeroToOne() * 4.0f - 2.0f;
				Vec3 crs = cross(direction, Vec3(0, 1, 0));
				direction += crs * offset;
				direction = direction.normalize();
				gravity->StartJump((-direction + Vec3(0, 0.01f, 0)) * 4.0f);
			}
		}
	}
	void BossHostility::Execute()
	{
		float elapsedTime = App::GetApp()->GetElapsedTime() * GameManager::GetInstance().GetTimeRate();
		Vec3 position = m_Enemy->GetPosition();
		Vec3 intruderPosition = m_Enemy->m_Intruder->GetPosition();
		Vec3 direction = intruderPosition - position;
		float distance = direction.length();
		direction = direction.normalize();

		if (m_CooldownTimer.UpdateTimer(GameManager::GetInstance().GetTimeRate())) {
			float rnd = Util::RandZeroToOne() * 100.0f;
			float prob = 0;
			for (int i = 0; i < m_Enemy->m_Skills.size(); i++) {
				if (typeid(BossGun) == typeid(m_Enemy->m_Skills[i].m_Skill)) {
					prob += m_Enemy->m_Skills[i].m_Prob / (m_MissileCount + 1);
				}
				else {
					prob += m_Enemy->m_Skills[i].m_Prob;
				}
				if (prob > rnd) {
					m_Enemy->ChangeState(i);
					return;
				}
			}
			m_Enemy->ChangeState(m_Enemy->m_Skills.size() - 1);
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
		
	}


	void BossCrush::Enter()
	{
		AttackState::Enter();
		BossHostility::m_MissileCount = 0;
	}
	void BossCrush::Execute()
	{
		if (!m_Attack) return;

		AttackState::Execute();

		float elapsed = App::GetApp()->GetElapsedTime() * GameManager::GetInstance().GetTimeRate();
		Vec3 position = m_Enemy->GetPosition();
		Vec3 intruderPosition = m_Enemy->m_Intruder->GetPosition();

		Vec3 direction = intruderPosition - position;
		float distance = direction.length();
		direction = direction.normalize();

		m_Enemy->m_Effect->SetEffectSpeed(m_SmokeHandle, 1.0f * GameManager::GetInstance().GetTimeRate());

		switch (m_State)
		{
		case AState::Ready: {
			if (IsStandBy())break;
			if (m_Attack->IsInRange(distance)) {
				SetState(AState::Playing, 1.0f / m_Enemy->GetMotionRate());
				m_Enemy->SetAnimation(L"Crush");
				Vec3 crs = cross(Vec3(0, 1, 0), direction) * 0.5f;
				m_AttackPosition = m_Enemy->GetPosition() + direction.normalize() + crs - Vec3(0, 1.0f, 0);

				CIRCLE circle = { m_Attack->GetScale().x,36,360.0f };
				m_Stage->AddGameObject<AreaOfEffect>(m_AttackPosition,Vec3(0,0,1), circle, 1.0f / m_Enemy->GetMotionRate());

				break;
			}

			float rotationY = atan2f(direction.x, direction.z);
			m_Enemy->SetRotation(Vec3(0, rotationY, 0));
			m_Enemy->Move(direction);

			break;
		}
		case AState::Playing: {
			if (!IsStandBy()) {
				SetState(AState::Finished, m_Attack->GetCharaCooldown());
				SoundManager::GetInstance().PlaySE(L"SE_CRUSH");
				m_Attack->Play(m_AttackPosition);

				m_FinishedForward = m_Enemy->GetForward();

				m_Enemy->m_Effect->PlayEffect(m_SmokeHandle, L"Trampling", m_AttackPosition, 0.0f);
				m_Enemy->m_Effect->SetScale(m_SmokeHandle, m_Attack->GetSize() / 8.0f);
			}
			break;
		}
		case AState::Finished: {
			if (LerpRotatePlayer(direction)) {
				m_Enemy->ChangeState<BossHostility>();
			}
			break;
		}
		}
	}
	void BossCrush::Exit()
	{

	}

	void BossGun::Enter()
	{
		AttackState::Enter();
		BossHostility::m_MissileCount++;
	}
	void BossGun::Execute()
	{
		if (!m_Attack) return;
		AttackState::Execute();

		Vec3 position = m_Enemy->GetPosition();
		Vec3 intruderPosition = m_Enemy->m_Intruder->GetPosition();

		Vec3 direction = intruderPosition - position;
		float distance = direction.length();
		direction = direction.normalize();

		float rotationY = atan2f(direction.x, direction.z);
		m_Enemy->SetRotation(Vec3(0, rotationY, 0));

		switch (m_State)
		{
		case AState::Ready:
			if (IsStandBy()) break;
			if (m_Attack->IsInRange(distance)) {
				SetState(AState::Playing, 0.25f * m_Enemy->GetMotionRate());

				m_Enemy->SetAnimation(L"Missile_First");
				auto gravity = m_Enemy->GetComponent<Gravity>();
				gravity->StartJump((-direction + Vec3(0, 0.1f / m_Enemy->GetMotionRate(), 0)) * 5.0f * m_Enemy->GetMotionRate());

				break;
			}
			m_Enemy->Move(direction);

			break;
		case AState::Playing:
			if (IsStandBy()) break;

			if (!m_Attack->GetDrawActive() && m_Enemy->GetAnimationFinish()) {
				m_Attack->Play(position + Vec3(0, m_Enemy->GetScale().y / 2.0f, 0.0f));
				m_Enemy->SetAnimation(L"Missile");
			}

			if (m_Attack->IsFinish()) {
				SetState(AState::Finished, m_Attack->GetCharaCooldown());

				m_FinishedForward = m_Enemy->GetForward();
				m_Enemy->SetAnimation(L"Missile_Finish");
			}

			break;
		case AState::Finished:
			if (m_Enemy->GetCurrentAnimationKey() == L"Idle") {
				if (LerpRotatePlayer(direction)) {
					m_Enemy->ChangeState<BossHostility>();
				}
			}
			else if(m_Enemy->GetAnimationFinish()) {
				m_Enemy->SetAnimation(L"Idle");
			}
			break;
		}
	}
	void BossGun::Exit()
	{

	}

	void BossShakeOff::Enter()
	{
		AttackState::Enter();
		BossHostility::m_MissileCount = 0;
		m_Enemy->m_AroundPlayerTime = 0.0f;
		m_Enemy->m_Skills[0].m_Prob = 30.0f;

		Vec3 position = m_Enemy->GetPosition();
		Vec3 intruderPosition = m_Enemy->m_Intruder->GetPosition();

		Vec3 direction = intruderPosition - position;
		float distance = direction.length();

		if (m_Attack->IsInRange(distance)) {
			m_Enemy->ChangeState<BossHostility>();
		}
	}
	void BossShakeOff::Execute()
	{
		if (!m_Attack) return;

		Vec3 position = m_Enemy->GetPosition();
		Vec3 intruderPosition = m_Enemy->m_Intruder->GetPosition();

		Vec3 direction = intruderPosition - position;
		float distance = direction.length();
		direction = direction.normalize();
		
		AttackState::Execute();
		
		switch (m_State)
		{
		case AState::Ready: {
			if (IsStandBy()) break;

			m_Enemy->SetAnimation(L"ShakeOff_First");
			//SetState(AState::Playing, 0.25f * m_Enemy->GetMotionRate());
			SetState(AState::Playing, [&]() { return m_Enemy->GetAnimationFinish(); });
			m_AttackPosition = position + direction * 2.0f + Vec3(0.0f, -0.5f, 0.0f);

			RECTANGLE rect = { 0,0,m_Attack->GetScale().x,m_Attack->GetScale().z };
			float time = m_Enemy->GetComponent<BcBaseDraw>()->GetAnimationTime(L"ShakeOff_First");
			time += m_Enemy->GetComponent<BcBaseDraw>()->GetAnimationTime(L"ShakeOff_Bef");
			m_Stage->AddGameObject<AreaOfEffect>(Vec3(m_AttackPosition.x, 0.51f, m_AttackPosition.z), Vec3(direction.x, 0, direction.z), rect, time);

			break;
		}
		case AState::Playing:
			if (IsStandBy()) break;

			if (m_Enemy->GetCurrentAnimationKey() != L"ShakeOff_Bef") {
				m_Enemy->SetAnimation(L"ShakeOff_Bef");
			}
			else if(m_Enemy->GetAnimationFinish()){
				m_Enemy->SetAnimation(L"ShakeOff_Aft");
				m_Attack->Play(m_AttackPosition, Vec3(direction.x, 0.0f, direction.z));

				SetState(AState::Finished, 0);

				m_FinishedForward = m_Enemy->GetForward();
			}
			break;
		case AState::Finished:
			if (m_Enemy->GetCurrentAnimationKey() == L"Idle") {
				if (LerpRotatePlayer(direction)) {
					m_Enemy->ChangeState<BossHostility>();
				}
			}
			else if (m_Enemy->GetAnimationFinish()) {
				m_Enemy->SetAnimation(L"Idle");
			}
			break;
		default:
			break;
		}
	}
	void BossShakeOff::Exit()
	{

	}
}

//end basecross
