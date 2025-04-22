/*!
@file Enemy.cpp
@brief “G‚È‚ÇŽÀ‘Ì
*/

#include "stdafx.h"
#include "Project.h"

namespace basecross {
	void BossSearch::Enter()
	{
		EnemyState::Enter();
		m_Line = m_Stage->AddGameObject<LineCube>(0.02f, Col4(0, 1, 0, 1));
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
	}
	void BossHostility::Execute()
	{
		float elapsedTime = App::GetApp()->GetElapsedTime() * m_Enemy->m_ZoneElapsedTime;
		auto navi = m_Enemy->GetComponent<Navigate>();
		Vec3 position = m_Enemy->GetPosition();
		Vec3 intruderPosition = m_Enemy->m_Intruder->GetPosition();
		RayCastHit hit;
		if (RayCast::HitTestVec(hit, Line(position, intruderPosition), m_Enemy->GetStage()->GetGameObjectVec(), { L"Bullet",L"Line",L"Enemy",L"Player" })) {
			m_Enemy->SetIntruderAlert(false);
			if (m_LastInturderPosition != Vec3()) {
				Vec3 direction = m_LastInturderPosition - position;
				if (direction.length() >= 0.1f) {
					direction = direction.normalize();
					float rotate = atan2f(direction.x, direction.z);
					m_Enemy->SetRotation(Vec3(0, rotate, 0));
					m_Enemy->Move(direction);
				}
				else {
					m_Enemy->SearchRange();
					if (!m_Enemy->GetIntruderAlert()) {
						m_Enemy->ChangeState<BossSearch>();
						return;
					}
				}
			}
		}
		else {
			auto& gunAttack = m_Enemy->m_Gun;
			auto& cruchAttack = m_Enemy->m_Cruch;

			m_Enemy->SetIntruderAlert(true);

			m_LastInturderPosition = intruderPosition;
			Vec3 direction = intruderPosition - position;
			if (m_CooldownTimer.UpdateTimer()) {
				float distance = direction.length();
				direction = direction.normalize();
				Vec3 velocity = Vec3();
				float rotationY = 0;
				if (gunAttack->IsInRange(distance) && distance > gunAttack->GetRange() * 0.2f && gunAttack->GetCooldown() == 0) {
					m_Enemy->ChangeState<BossGun>();
					return;
				}
				else {
					if (cruchAttack->GetCooldown() == 0) {
						m_Enemy->ChangeState<BossCrush>();
						return;
					}
					else if (gunAttack->GetCooldown() == 0) {
						m_Enemy->ChangeState<BossGun>();
						return;
					}
				}
				if (gunAttack->GetCooldown() != 0 && cruchAttack->GetCooldown() != 0) {
					rotationY = atan2f(direction.x, direction.z);
					if (distance > m_NearDistance.length()) {
						m_Enemy->Move(direction);
					}
					else {
						if (m_SideStepTimer.UpdateTimer()) {
							if (Util::RandZeroToOne() > 0.95f) {
								m_SideStepDirection *= -1;
								m_SideStepTimer.SetTime(0.5f, true);
							}
						}
						m_Enemy->Move(cross(direction, Vec3(0, 1, 0)) * m_SideStepDirection / 5.0f);
					}
				}
				position += velocity;
				m_Enemy->SetRotation(Vec3(0, rotationY, 0));
			}

		}
	}
	void BossHostility::Exit()
	{
	}

	void BossCrush::Enter()
	{
		EnemyState::Enter();
		m_Attack = m_Enemy->m_Cruch;
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
				m_IsReady = true;
				m_ReadyTimer.SetTime(0.5f, true);
				m_AttackPosition = m_Enemy->GetPosition() + direction.normalize() * 0.25f;
				m_Stage->AddGameObject<AreaOfEffect>(m_AttackPosition, 0.5f, 36, 0.5f);
			}
			else if (distance > m_Attack->GetRange() * 10.0f && m_Enemy->m_Gun->GetCooldown() == 0) {
				m_Enemy->ChangeState<BossHostility>();
				return;
			}
			if (m_IsReady) {
				if (m_ReadyTimer.UpdateTimer()) {
					m_Attack->Play(m_AttackPosition);
					m_CooldownTimer.SetTime(m_Attack->GetCharaCooldown(), true);
					m_IsFinish = true;
					m_FinishedForward = m_Enemy->GetForward();
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
	void BossGun::Enter()
	{
		EnemyState::Enter();
		m_Attack = m_Enemy->m_Gun;
	}
	void BossGun::Execute()
	{
		Vec3 position = m_Enemy->GetPosition();
		Vec3 intruderPosition = m_Enemy->m_Intruder->GetPosition();

		Vec3 direction = intruderPosition - position;
		float distance = direction.length();
		if (!m_IsFinish) {
			if (m_Attack->IsInRange(distance) && !m_IsReady) {
				m_IsReady = true;
				m_ReadyTimer.SetTime(0.5f, true);
			}
			if (m_IsReady) {
				if (m_Attack->IsFinish()) {
					m_CooldownTimer.SetTime(m_Attack->GetCharaCooldown(), true);
					m_IsFinish = true;
					m_FinishedForward = m_Enemy->GetForward();
				}
				else if (m_ReadyTimer.UpdateTimer() && !m_Attack->GetDrawActive()) {
					m_Attack->Play(position + Vec3(0, 0.6f, 0.0f));
				}
			}
			else {
				m_Enemy->Move(direction);
			}
			float rotationY = atan2f(direction.x, direction.z);
			m_Enemy->SetRotation(Vec3(0, rotationY, 0));
		}
		else {
			if (m_CooldownTimer.UpdateTimer()) {
				if (LerpRotatePlayer(direction)) {
					m_Enemy->ChangeState<BossHostility>();
				}
			}
		}
	}
	void BossGun::Exit()
	{

	}
}

//end basecross
